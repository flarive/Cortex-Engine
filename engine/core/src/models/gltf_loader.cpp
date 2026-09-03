#include "../../include/models/gltf_loader.h"

#include "../../include/managers/log_manager.h"
#include "../../include/managers/filesystem_manager.h"
#include "../../include/managers/texture_manager.h"

#include "../../include/singleton.h"


#include <glm/gtc/type_ptr.hpp>

void engine::GLtfMeshLoader::loadModel(const std::string& path, bool loadAnimation, bool flipUVs)
{
    // ------------------------------------------------------------
    // 1. Parse GLTF
    // ------------------------------------------------------------
    m_directory = FileSystemManager::getDirectoryPath(path);
    m_filename = FileSystemManager::getFileName(path);

    tinygltf3::Model model;

    tg3_parse_options opts{};
    tg3_parse_options_init(&opts);
    opts.images_as_is = 0;

    tg3_error_stack errors{};
    tg3_error_stack_init(&errors);

    tg3_error_code err = tg3_parse_file(
        &model.raw(),
        &errors,
        path.c_str(),
        (uint32_t)path.size(),
        &opts
    );

    if (err != TG3_OK)
    {
        logger.error("GLTF loading error");

        for (uint32_t i = 0; i < errors.count; i++)
        {
            fprintf(stderr, "[%d] %s\n",
                (int)errors.entries[i].severity,
                errors.entries[i].message ? errors.entries[i].message : "(null)");
        }
        return;
    }

    const tg3_model& raw = model.raw();

    //createTrace("d:\\GLtf_vertices.txt");

    // ------------------------------------------------------------
    // 2. Bones?
    // ------------------------------------------------------------
    m_hasBones = (raw.skins_count > 0);
    m_hasAnimations = (loadAnimation && raw.animations_count > 0);

    // ------------------------------------------------------------
    // 3. Count vertices (optional)
    // ------------------------------------------------------------
    m_numberOfMeshes = raw.meshes_count;
    m_numberOfVertices = 0;

    for (uint32_t i = 0; i < raw.meshes_count; ++i)
    {
        const tg3_mesh& mesh = raw.meshes[i];

        for (uint32_t p = 0; p < mesh.primitives_count; ++p)
        {
            const tg3_primitive& prim = mesh.primitives[p];

            for (uint32_t a = 0; a < prim.attributes_count; ++a)
            {
                const tg3_str_int_pair& pair = prim.attributes[a];

                if (tg3_str_equals_cstr(pair.key, "POSITION"))
                {
                    const tg3_accessor& posAcc = raw.accessors[pair.value];
                    m_numberOfVertices += (uint)posAcc.count;
                }
            }
        }
    }


    // ------------------------------------------------------------
    // 4. Extract bones (if any)
    // ------------------------------------------------------------
    if (m_hasBones)
    {
        extractSkinBones(raw);
        computeBindPoseMatrices();
    }


    // ------------------------------------------------------------
    // 5. Build GLTF node hierarchy (ALWAYS)
    // ------------------------------------------------------------
    m_nodes.resize(raw.nodes_count);

    for (uint32_t i = 0; i < raw.nodes_count; ++i)
    {
        const tg3_node& n = raw.nodes[i];

        GLTFNode node{};
        node.index = i;
        node.parent = -1;
        node.local = getNodeLocalTransform(n);

        // children
        for (uint32_t c = 0; c < n.children_count; ++c)
            node.children.push_back(n.children[c]);

        m_nodes[i] = node;
    }

    // assign parents
    for (uint32_t i = 0; i < raw.nodes_count; ++i)
    {
        for (int child : m_nodes[i].children)
            m_nodes[child].parent = i;
    }


    // ------------------------------------------------------------
    // 6. Compute global transforms
    // ------------------------------------------------------------
    std::function<void(int, glm::mat4)> computeGlobal =
        [&](int index, glm::mat4 parentTransform)
        {
            GLTFNode& node = m_nodes[index];
            node.global = parentTransform * node.local;

            for (int child : node.children)
                computeGlobal(child, node.global);
        };

    const tg3_scene& scene = raw.scenes[raw.default_scene];

    for (uint32_t r = 0; r < scene.nodes_count; ++r)
    {
        int root = scene.nodes[r];
        computeGlobal(root, glm::mat4(1.0f));
    }


    // ------------------------------------------------------------
    // 7. Process meshes THROUGH NODES (correct GLTF behavior)
    // ------------------------------------------------------------
    processNode(raw);

    // ------------------------------------------------------------
    // 8. Finalize mesh bone data  (only if skin exists)
    // ------------------------------------------------------------
    if (m_hasBones)
    {
        for (unsigned int i = 0; i < m_meshes.size(); i++)
        {
            std::shared_ptr<Mesh> mesh = m_meshes[i];

            // Bind-pose matrices (same for all meshes of the model)
            mesh->bindPoseMatrices = m_finalBindPoseMatrices;
            mesh->hasBones = m_hasBones;
            mesh->hasAnimations = m_hasAnimations;
        }
    }
}

void engine::GLtfMeshLoader::processNode(const tg3_model& model)
{
    for (uint32_t i = 0; i < model.nodes_count; ++i)
    {
        const tg3_node& node = model.nodes[i];

        if (node.mesh >= 0)
        {
            const tg3_mesh& mesh = model.meshes[node.mesh];
            m_meshes.push_back(processMesh(mesh, model, (int)i));
        }
    }
}

std::shared_ptr<engine::Mesh> engine::GLtfMeshLoader::processMesh(const tg3_mesh& mesh, const tg3_model& model, int nodeIndex)
{
    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    SceneSettings& sceneSettings = singleton->sceneSettings();

    // ------------------------------------------------------------
    // Node transform (global)
    // ------------------------------------------------------------
    glm::mat4 modelMatrix(1.0f);
    glm::mat3 normalMatrix(1.0f);

    if (nodeIndex >= 0 && nodeIndex < (int)m_nodes.size())
    {
        modelMatrix = m_nodes[nodeIndex].global;
        normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
    }

    std::vector<Vertex> vertices{};
    std::vector<unsigned int> indices{};
    std::shared_ptr<Material> material{};

    // ------------------------------------------------------------
    // Process all primitives of this mesh
    // ------------------------------------------------------------
    for (uint32_t p = 0; p < mesh.primitives_count; ++p)
    {
        const tg3_primitive& prim = mesh.primitives[p];

        uint32_t posIndex = UINT32_MAX;
        uint32_t norIndex = UINT32_MAX;
        uint32_t tanIndex = UINT32_MAX;
        uint32_t uvIndex = UINT32_MAX;

        // ------------------------------------------------------------
        // Attribute lookup
        // ------------------------------------------------------------
        for (uint32_t a = 0; a < prim.attributes_count; ++a)
        {
            const tg3_str_int_pair& pair = prim.attributes[a];

            if (tg3_str_equals_cstr(pair.key, "POSITION"))
                posIndex = pair.value;
            else if (tg3_str_equals_cstr(pair.key, "NORMAL"))
                norIndex = pair.value;
            else if (tg3_str_equals_cstr(pair.key, "TANGENT"))
                tanIndex = pair.value;
            else if (tg3_str_equals_cstr(pair.key, "TEXCOORD_0"))
                uvIndex = pair.value;
        }

        const bool hasPosition = (posIndex != UINT32_MAX);
        const bool hasNormals = (norIndex != UINT32_MAX);
        const bool hasTangents = (tanIndex != UINT32_MAX);
        const bool hasTexCoords = (uvIndex != UINT32_MAX);

        const tg3_accessor& posAcc = model.accessors[posIndex];
        const tg3_accessor& norAcc = model.accessors[norIndex];
        const tg3_accessor& tanAcc = model.accessors[tanIndex];
        const tg3_accessor& uvAcc = model.accessors[uvIndex];

        const float* positions = nullptr;
        const float* normals = nullptr;
        const float* tangents = nullptr;
        const float* texcoords = nullptr;

        // ------------------------------------------------------------
        // Read buffer pointers
        // ------------------------------------------------------------
        if (hasPosition)
        {
            const tg3_buffer_view& view = model.buffer_views[posAcc.buffer_view];
            const tg3_buffer& buf = model.buffers[view.buffer];
            positions = reinterpret_cast<const float*>(
                buf.data.data + view.byte_offset + posAcc.byte_offset);
        }

        if (hasNormals)
        {
            const tg3_buffer_view& view = model.buffer_views[norAcc.buffer_view];
            const tg3_buffer& buf = model.buffers[view.buffer];
            normals = reinterpret_cast<const float*>(
                buf.data.data + view.byte_offset + norAcc.byte_offset);
        }

        if (hasTangents)
        {
            const tg3_buffer_view& view = model.buffer_views[tanAcc.buffer_view];
            const tg3_buffer& buf = model.buffers[view.buffer];
            tangents = reinterpret_cast<const float*>(
                buf.data.data + view.byte_offset + tanAcc.byte_offset);
        }

        if (hasTexCoords)
        {
            const tg3_buffer_view& view = model.buffer_views[uvAcc.buffer_view];
            const tg3_buffer& buf = model.buffers[view.buffer];
            texcoords = reinterpret_cast<const float*>(
                buf.data.data + view.byte_offset + uvAcc.byte_offset);
        }

        vertices.reserve(posAcc.count);

        // ------------------------------------------------------------
        // Bone attributes
        // ------------------------------------------------------------
        uint32_t jointsIndex = UINT32_MAX;
        uint32_t weightsIndex = UINT32_MAX;

        for (uint32_t a = 0; a < prim.attributes_count; ++a)
        {
            const tg3_str_int_pair& pair = prim.attributes[a];

            if (tg3_str_equals_cstr(pair.key, "JOINTS_0"))
                jointsIndex = pair.value;
            else if (tg3_str_equals_cstr(pair.key, "WEIGHTS_0"))
                weightsIndex = pair.value;
        }

        const bool hasJoints = (jointsIndex != UINT32_MAX);
        const bool hasWeights = (weightsIndex != UINT32_MAX);

        const tg3_accessor* jointsAcc = hasJoints ? &model.accessors[jointsIndex] : nullptr;
        const tg3_accessor* weightsAcc = hasWeights ? &model.accessors[weightsIndex] : nullptr;


        const float* weightsF = nullptr;
        const uint8_t* weights8 = nullptr;
        const uint16_t* weights16 = nullptr;

        if (weightsAcc)
        {
            const tg3_buffer_view& view = model.buffer_views[weightsAcc->buffer_view];
            const tg3_buffer& buf = model.buffers[view.buffer];
            const uint8_t* base = buf.data.data + view.byte_offset + weightsAcc->byte_offset;

            if (weightsAcc->component_type == TG3_COMPONENT_TYPE_FLOAT)
                weightsF = reinterpret_cast<const float*>(base);
            else if (weightsAcc->component_type == TG3_COMPONENT_TYPE_UNSIGNED_BYTE)
                weights8 = reinterpret_cast<const uint8_t*>(base);
            else if (weightsAcc->component_type == TG3_COMPONENT_TYPE_UNSIGNED_SHORT)
                weights16 = reinterpret_cast<const uint16_t*>(base);
        }
        
        const uint16_t* joints16 = nullptr;
        const uint8_t* joints8 = nullptr;

        if (jointsAcc)
        {
            const tg3_buffer_view& view = model.buffer_views[jointsAcc->buffer_view];
            const tg3_buffer& buf = model.buffers[view.buffer];
            const uint8_t* base = buf.data.data + view.byte_offset + jointsAcc->byte_offset;

            if (jointsAcc->component_type == TG3_COMPONENT_TYPE_UNSIGNED_SHORT)
                joints16 = reinterpret_cast<const uint16_t*>(base);
            else if (jointsAcc->component_type == TG3_COMPONENT_TYPE_UNSIGNED_BYTE)
                joints8 = reinterpret_cast<const uint8_t*>(base);
        }



        // Before vertex loop, decide if this primitive is actually skinned
        const bool primHasSkin = hasJoints && hasWeights;





        // ------------------------------------------------------------
        // Vertex assembly + transform
        // ------------------------------------------------------------
        for (uint64_t i = 0; i < posAcc.count; ++i)
        {
            Vertex v{ glm::vec3(0.0f) };

            if (primHasSkin)
            {
                setVertexBoneDataToDefault(v); // will be filled by JOINTS_0 / WEIGHTS_0
            }
            else
            {
                // Rigid mesh: bind to bone 0 with full weight
                setVertexBoneDataToDefault(v);
                setVertexBoneData(v, 0, 1.0f);
            }


            // Position
            v.position = glm::vec3(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]);

            // Normal
            if (hasNormals && normals)
            {
                v.normal = glm::vec3(normals[i * 3 + 0], normals[i * 3 + 1], normals[i * 3 + 2]);
            }

            // Tangent
            if (hasTangents && tangents)
            {
                v.tangent = glm::vec3(tangents[i * 4 + 0], tangents[i * 4 + 1], tangents[i * 4 + 2]);
            }

            // Bitangent ?

            // UV
            if (hasTexCoords && texcoords)
            {
                v.texCoords = glm::vec2(texcoords[i * 2 + 0], texcoords[i * 2 + 1]);
            }

            // BoneIDs and weights
            if (m_hasBones && hasJoints && hasWeights)
            {
                setVertexBoneDataToDefault(v);

                for (int k = 0; k < 4; ++k)
                {
                    int jointIndex =
                        joints16 ? int(joints16[i * 4 + k]) :
                        joints8 ? int(joints8[i * 4 + k]) :
                        -1;

                    if (jointIndex < 0 || jointIndex >= (int)m_jointToBone.size())
                        continue;

                    float weight =
                        weightsF ? weightsF[i * 4 + k] :
                        weights8 ? float(weights8[i * 4 + k]) / 255.0f :
                        weights16 ? float(weights16[i * 4 + k]) / 65535.0f :
                        0.0f;

                    if (weight <= 0.0f)
                        continue;

                    int boneID = m_jointToBone[jointIndex];

                    setVertexBoneData(v, boneID, weight);
                }
            }

            //trace(toStdString(mesh.name), i, v);

            vertices.push_back(std::move(v));
        }

        // ------------------------------------------------------------
        // Indices
        // ------------------------------------------------------------
        const tg3_accessor& idxAcc = model.accessors[prim.indices];
        const tg3_buffer_view& idxView = model.buffer_views[idxAcc.buffer_view];
        const tg3_buffer& idxBuf = model.buffers[idxView.buffer];

        const uint8_t* idxPtr = idxBuf.data.data + idxView.byte_offset + idxAcc.byte_offset;

        indices.reserve(idxAcc.count);

        if (idxAcc.component_type == TG3_COMPONENT_TYPE_UNSIGNED_SHORT)
        {
            const uint16_t* src = reinterpret_cast<const uint16_t*>(idxPtr);
            for (uint32_t i = 0; i < idxAcc.count; ++i)
                indices.push_back(src[i]);
        }
        else if (idxAcc.component_type == TG3_COMPONENT_TYPE_UNSIGNED_INT)
        {
            const uint32_t* src = reinterpret_cast<const uint32_t*>(idxPtr);
            for (uint32_t i = 0; i < idxAcc.count; ++i)
                indices.push_back(src[i]);
        }

        // ------------------------------------------------------------
        // Material
        // ------------------------------------------------------------
        std::shared_ptr<engine::Material> mat{};

        if (sceneSettings.method == RenderMethod::PBR)
            mat = loadPBRMaterial(prim.material, model);
        else
            mat = loadBlinnPhongMaterial(prim.material, model);

        m_materials.push_back(mat);

        if (mat->hasTextureMap())
            mat->loadTexturesAsync(false);
    }

    //logger.info("Mesh {} vertices {} / indices {}", toStdString(mesh.name), vertices.size(), indices.size());

    // ------------------------------------------------------------
    // Create Mesh
    // ------------------------------------------------------------
    auto meshPtr = std::make_shared<engine::Mesh>(
        toStdString(mesh.name),
        std::move(vertices),
        std::move(indices),
        m_materials.back()
    );

    return meshPtr;
}

/// <summary>
/// GLTF2 is a PBR geometry container first
/// </summary>
std::shared_ptr<engine::Material> engine::GLtfMeshLoader::loadPBRMaterial(uint32_t matIndex, const tg3_model& raw)
{
    const tg3_material& mat = raw.materials[matIndex];

    //Color baseColorFactor{
    //    mat.pbr_metallic_roughness.base_color_factor[0],
    //    mat.pbr_metallic_roughness.base_color_factor[1],
    //    mat.pbr_metallic_roughness.base_color_factor[2],
    //    mat.pbr_metallic_roughness.base_color_factor[3]
    //};

    Color baseColorFactor{ 0.3f, 0.3f, 0.3f, 1.0f };

	if (mat.pbr_metallic_roughness.base_color_factor[0] != 1.0f &&
		mat.pbr_metallic_roughness.base_color_factor[1] != 1.0f &&
		mat.pbr_metallic_roughness.base_color_factor[2] != 1.0f &&
        mat.pbr_metallic_roughness.base_color_factor[3] != 1.0f)
	{
		baseColorFactor = Color{
			mat.pbr_metallic_roughness.base_color_factor[0],
			mat.pbr_metallic_roughness.base_color_factor[1],
			mat.pbr_metallic_roughness.base_color_factor[2],
			mat.pbr_metallic_roughness.base_color_factor[3]
		};
	}


    std::string baseColorTex = getTexture(raw, mat.pbr_metallic_roughness.base_color_texture);
    std::string normalTex = getTexture(raw, mat.normal_texture);
    std::string metallicTex = getTexture(raw, mat.pbr_metallic_roughness.metallic_roughness_texture);
    std::string roughnessTex = metallicTex; // GLTF packs metallic+roughness together
    std::string aoTex = getTexture(raw, mat.occlusion_texture);
    std::string emissiveTex = getTexture(raw, mat.emissive_texture);
    std::string heightTex = ""; // GLTF rarely uses height

    bool useARM = false;
    bool useMR = false;

    // GLTF ARM detection (AO + Roughness + Metallic in one texture)
    if (!aoTex.empty() && !metallicTex.empty() && aoTex == metallicTex)
        useARM = true;

    // GLTF MR detection (Metallic + Roughness in one texture)
    if (!metallicTex.empty() && !roughnessTex.empty() && metallicTex == roughnessTex)
        useMR = true;

    std::shared_ptr<Material> material{};

    if (useARM)
    {
        material = std::make_shared<PBRMaterial>(CombinedTexture::ARM, baseColorFactor, baseColorTex, normalTex, metallicTex, heightTex, emissiveTex, 1.0f);
    }
    else if (useMR)
    {
        material = std::make_shared<PBRMaterial>(CombinedTexture::RM, baseColorFactor, baseColorTex, normalTex, metallicTex, heightTex, emissiveTex, 1.0f);
    }
    else
    {
        material = std::make_shared<PBRMaterial>(baseColorFactor, baseColorTex, normalTex, metallicTex, roughnessTex, aoTex, heightTex, emissiveTex, 1.0f);
    }

    if (!material->hasTextureMap())
        material = std::make_shared<PBRMaterial>(baseColorFactor);


    material->setName(toStdString(mat.name));

    return material;
}

/// <summary>
/// GLTF2 is not a BlinnPhong geometry container
/// No specular map support so normal maps won't look good
/// </summary>
std::shared_ptr<engine::Material> engine::GLtfMeshLoader::loadBlinnPhongMaterial(uint32_t matIndex, const tg3_model& raw)
{
    const tg3_material& mat = raw.materials[matIndex];

    Color ambientColor{ 0.1f, 0.1f, 0.1f, 1.0f };
    Color diffuseColor{};
    Color specularColor{ 1.0f };

    std::string diffuseTex{};
    std::string specularTex{};
    std::string normalTex{};
    float shininess{};

    // -----------------------------
    // 1. ALBEDO (baseColorTexture)
    // -----------------------------
    if (mat.pbr_metallic_roughness.base_color_texture.index >= 0)
    {
        diffuseTex = getTexture(raw, mat.pbr_metallic_roughness.base_color_texture);
    }
    else
    {
        // fallback base color
        auto bc = mat.pbr_metallic_roughness.base_color_factor;
        diffuseColor = Color(bc[0], bc[1], bc[2], 1.0f);
    }

    // -----------------------------
    // 2. NORMAL MAP
    // -----------------------------
    if (mat.normal_texture.index >= 0)
    {
        normalTex = getTexture(raw, mat.normal_texture);
    }

    // -----------------------------
    // 3. METALLIC / ROUGHNESS
    // -----------------------------
    float metallic = static_cast<float>(mat.pbr_metallic_roughness.metallic_factor);
    float roughness = static_cast<float>(mat.pbr_metallic_roughness.roughness_factor);

    /*if (mat.pbr_metallic_roughness.metallic_roughness_texture.index >= 0)
    {
        metallicRoughnessTex = getTexture(raw, mat.pbr_metallic_roughness.metallic_roughness_texture);
    }*/

    // Convert to Blinn‑Phong
    
    // Specular color approximation
    glm::vec3 dielectricSpecular = glm::vec3(0.04f);

    glm::vec3 tempSpecularColor = glm::mix(dielectricSpecular, glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b), metallic);
    specularColor.r = tempSpecularColor.r;
    specularColor.g = tempSpecularColor.g;
    specularColor.b = tempSpecularColor.b;
    specularColor.a = 1.0f;

    // Shininess from roughness
    float gloss = 1.0f - roughness;
    shininess = 10.0f;// glm::pow(gloss, 4.0f) * 256.0f;

    //const tg3_texture& tex = raw.textures[mat.pbr_metallic_roughness.base_color_texture.index];

    // -----------------------------
    // 4. SPECULAR‑GLOSSINESS EXTENSION
    // -----------------------------
    //bool foundExt = false;
    //if (mat.ext.extensions_count > 0)
    //{
    //    // Extensions (KTX2, WebP, vendor extensions, future formats)
    //    for (uint32_t i = 0; i < mat.ext.extensions_count; ++i)
    //    {
    //        const tg3_extension& ext = mat.ext.extensions[i];
    //        
    //        // Convert tg3_str → std::string
    //        std::string name(ext.name.data, ext.name.len);

    //        // ---- KTX2 (KHR_texture_basisu) ----
    //        if (name == "KHR_materials_pbrSpecularGlossiness")
    //        {
    //            // diffuseTexture → albedo
    //            //if (ext.Has("diffuseTexture"))
    //            //{
    //            //    int idx = ext.Get("diffuseTexture").Get("index").Get<int>();
    //            //    result.albedoTexture = loadTexture(model, idx);
    //            //}

    //            //// specularGlossinessTexture → specular + glossiness
    //            //if (ext.Has("specularGlossinessTexture"))
    //            //{
    //            //    int idx = ext.Get("specularGlossinessTexture").Get("index").Get<int>();
    //            //    result.specularGlossinessTexture = loadTexture(model, idx);
    //            //    result.usesSpecGlossWorkflow = true;
    //            //}

    //            //// factors
    //            //if (ext.Has("specularFactor"))
    //            //{
    //            //    auto sf = ext.Get("specularFactor").Get<tinygltf::Value::Array>();
    //            //    result.specularColor = glm::vec3(sf[0].Get<double>(),
    //            //        sf[1].Get<double>(),
    //            //        sf[2].Get<double>());
    //            //}

    //            //if (ext.Has("glossinessFactor"))
    //            //{
    //            //    result.glossiness = ext.Get("glossinessFactor").Get<double>();
    //            //}
    //        }
    //    }
    //}

    std::shared_ptr<engine::Material> material{};
    
    if (!diffuseTex.empty())
        material = std::make_shared<BlinnPhongMaterial>(ambientColor, diffuseTex, specularTex, normalTex, std::string(), std::string(), shininess);
    else
        material = std::make_shared<BlinnPhongMaterial>(ambientColor, diffuseColor, specularColor, shininess);

    material->setName(toStdString(mat.name));

    return material;
}


int engine::GLtfMeshLoader::getTextureSource(const tg3_texture& tex)
{
    // Standard PNG/JPG
    if (tex.source >= 0)
        return tex.source;

    // Extensions (KTX2, WebP, vendor extensions, future formats)
    for (uint32_t i = 0; i < tex.ext.extensions_count; ++i)
    {
        const tg3_extension& ext = tex.ext.extensions[i];

        // Convert tg3_str → std::string
        std::string name(ext.name.data, ext.name.len);

        // ---- KTX2 (KHR_texture_basisu) ----
        if (name == "KHR_texture_basisu")
        {
            // ext.value is a tg3_value OBJECT containing { "source": int }
            return toInt(ext.value);   // your recursive JSON → int converter
        }

        // ---- WebP (KHR_texture_webp) ----
        if (name == "KHR_texture_webp")
        {
            return toInt(ext.value);
        }

        // ---- Vendor extensions (EXT_texture_*, etc.) ----
        // If they contain a "source" field, your toInt() will extract it.
        if (name.rfind("EXT_texture_", 0) == 0 ||
            name.rfind("MSFT_texture_", 0) == 0 ||
            name.rfind("KHR_texture_", 0) == 0)
        {
            int src = toInt(ext.value);
            if (src >= 0)
                return src;
        }
    }

    // No usable source found
    return -1;
}



std::string engine::GLtfMeshLoader::getTexture(const tg3_model& raw, const tg3_texture_info& info)
{
    if (info.index < 0)
        return "";

    const tg3_texture& tex = raw.textures[info.index];
    int src = getTextureSource(tex);

    if (src >= 0)
    {
        const tg3_image& img = raw.images[src];

        // External texture path (may be empty for embedded)
        std::string uri = toStdString(img.uri);
        std::string path{};

        if (!uri.empty())
        {
            path = m_directory + "\\" + uri;
        }
        else
        {
            path = m_directory + "\\*" + toStdString(img.name); // * means embedded texture path
        }

        // Avoid loading the same texture twice
        if (std::find(m_requestLoadingTextures.begin(), m_requestLoadingTextures.end(), path) != m_requestLoadingTextures.end())
        {
            return path;
        }

        // ------------------------------------------------------------
        // CASE 1: Embedded texture (GLB) → img.uri == ""
        // ------------------------------------------------------------
        if (uri.empty())
        {
            const tg3_buffer_view& view = raw.buffer_views[img.buffer_view];
            const tg3_buffer& buf = raw.buffers[view.buffer];

            const unsigned char* data = buf.data.data + view.byte_offset;

            size_t size = view.byte_length;

            // Load using your compressed loader (PNG/JPEG/etc)
            TextureUploadResult result =
                TextureManager::loadTextureFromMemory(
                    data,
                    size,
                    toStdString(img.mime_type).c_str(), TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture | TextureFlag_InvertY
                );

            // Cache
            TextureManagerInternal::textureIDCache[path] = result.textureID;
            TextureManagerInternal::textureDataCache[path] = TextureData{ result.textureID, const_cast<unsigned char*>(data), path, result.width, result.height, result.nbComponents, result.thumbnailLevel
            };

            m_requestLoadingTextures.push_back(path);
        }

        return path;


        // ------------------------------------------------------------
        // CASE 2: External texture (PNG/JPG file on disk)
        // ------------------------------------------------------------

        // just to avoid loading 2 times the same texture path
        m_requestLoadingTextures.push_back(path);

        return path;
    }

    return "";
}

std::string engine::GLtfMeshLoader::getTexture(const tg3_model& raw, const tg3_normal_texture_info& info)
{
    return getTexture(raw, tg3_texture_info{ info.index, info.tex_coord, info.ext });
}

std::string engine::GLtfMeshLoader::getTexture(const tg3_model& raw, const tg3_occlusion_texture_info& info)
{
    return getTexture(raw, tg3_texture_info{ info.index, info.tex_coord, info.ext });
}

std::string engine::GLtfMeshLoader::toStdString(tg3_str s)
{
    return s.len > 0 && s.data != nullptr ? std::string(s.data, s.len) : std::string();
}

unsigned char* engine::GLtfMeshLoader::toUChar(tg3_span_u8 span)
{
    return const_cast<unsigned char*>(span.data);
}

int engine::GLtfMeshLoader::toInt(const tg3_value& v)
{
    switch (v.type)
    {
    case TG3_VALUE_INT:
        return static_cast<int>(v.int_val);

    case TG3_VALUE_REAL:
        return static_cast<int>(v.real_val);

    case TG3_VALUE_BOOL:
        return v.bool_val ? 1 : 0;

    case TG3_VALUE_STRING:
        return std::atoi(v.string_val.data);

    case TG3_VALUE_OBJECT:
    {
        // Example: extract the field named "source"
        for (uint32_t i = 0; i < v.object_count; ++i)
        {
            const tg3_kv_pair& entry = v.object_data[i];

            // entry.key is tg3_str
            std::string key(entry.key.data, entry.key.len);

            if (key == "source")
            {
                // Recursively convert the child value
                return toInt(entry.value);
            }
        }

        // No usable integer field found
        return 0;
    }

    default:
        return 0;
    }
}

void engine::GLtfMeshLoader::extractSkinBones(const tg3_model& raw)
{
    for (uint32_t s = 0; s < raw.skins_count; ++s)
    {
        const tg3_skin& skin = raw.skins[s];

        // inverse bind matrices
        const tg3_accessor& ibmAcc = raw.accessors[skin.inverse_bind_matrices];
        const tg3_buffer_view& view = raw.buffer_views[ibmAcc.buffer_view];
        const tg3_buffer& buf = raw.buffers[view.buffer];

        const float* ibmData = reinterpret_cast<const float*>(
            buf.data.data + view.byte_offset + ibmAcc.byte_offset
            );

        // Resize mapping for this skin
        m_jointToBone.resize(skin.joints_count);

        for (uint32_t j = 0; j < skin.joints_count; ++j)
        {
            int nodeIndex = skin.joints[j];
            const tg3_node& node = raw.nodes[nodeIndex];

            std::string boneName = toStdString(node.name);

            BoneInfo info{};
            info.id = j;  // JOINTS_0 indices match this

            glm::mat4 ibm{};
            memcpy(glm::value_ptr(ibm), ibmData + j * 16, sizeof(float) * 16);
            info.offset = ibm;

            m_boneInfoMap[boneName] = info;

            m_jointToBone[j] = info.id;

            m_boneCounter = std::max(m_boneCounter, (int)skin.joints_count);
        }
    }
}


glm::mat4 engine::GLtfMeshLoader::getNodeLocalTransform(const tg3_node& n)
{
    // If matrix is explicitly set → use it
    if (n.has_matrix == 1)
    {
        return glm::make_mat4x4(n.matrix);
    }

    // Otherwise use TRS (always present)
    glm::vec3 T(
        (float)n.translation[0],
        (float)n.translation[1],
        (float)n.translation[2]
    );

    glm::vec3 S(
        (float)n.scale[0],
        (float)n.scale[1],
        (float)n.scale[2]
    );

    glm::quat R(
        (float)n.rotation[3], // w
        (float)n.rotation[0], // x
        (float)n.rotation[1], // y
        (float)n.rotation[2]  // z
    );

    glm::mat4 M =
        glm::translate(glm::mat4(1.0f), T) *
        glm::toMat4(R) *
        glm::scale(glm::mat4(1.0f), S);

    return M;
}

void engine::GLtfMeshLoader::computeBindPoseMatrices()
{
    m_finalBindPoseMatrices.clear();
    m_finalBindPoseMatrices.reserve(m_boneInfoMap.size());

    for (auto& kv : m_boneInfoMap)
    {
        const BoneInfo& info = kv.second;

        // Bind‑pose skinning matrix should be identity
        glm::mat4 skinMat = glm::mat4(1.0f);

        m_finalBindPoseMatrices.push_back(skinMat);
    }
}

engine::GLtfMeshLoader::~GLtfMeshLoader()
{
    logger.trace("GLtfMeshLoader destructor called");
}
