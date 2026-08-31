#include "../../include/models/gltf_loader.h"

#include "../../include/managers/log_manager.h"
#include "../../include/managers/filesystem_manager.h"
#include "../../include/managers/texture_manager.h"

#include "../../include/singleton.h"


#include <glm/gtc/type_ptr.hpp>

void engine::GLtfMeshLoader::loadModel(const std::string& path, bool flipUVs)
{
    // directory + filename
    m_directory = FileSystemManager::getDirectoryPath(path);
    m_filename = FileSystemManager::getFileName(path);

    tinygltf3::Model model;

    tg3_parse_options opts{};
    tg3_parse_options_init(&opts);
    opts.images_as_is = 0; // keep raw bytes

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
        // handle errors
        logger.error("GLTF loading error: unknown");

        for (uint32_t i = 0; i < errors.count; i++) {
            fprintf(stderr, "[%d] %s\n", (int)errors.entries[i].severity,
                errors.entries[i].message ? errors.entries[i].message : "(null)");
        }

        return;
    }


    const tg3_model& raw = model.raw();


    // check for bones or not
    if (raw.skins_count > 0)
        m_hasBones = true;


    m_numberOfMeshes = raw.meshes_count;

    // Count vertices
    for (uint32_t i = 0; i < raw.meshes_count; ++i)
    {
        const tg3_mesh& mesh = raw.meshes[i];
        for (uint32_t p = 0; p < mesh.primitives_count; ++p)
        {
            const tg3_primitive& prim = mesh.primitives[p];

            uint32_t posIndex = UINT32_MAX;
            uint32_t norIndex = UINT32_MAX;
            uint32_t tanIndex = UINT32_MAX;
            uint32_t uvIndex = UINT32_MAX;

            for (uint32_t a = 0; a < prim.attributes_count; ++a)
            {
                const tg3_str_int_pair& pair = prim.attributes[a];

                if (tg3_str_equals_cstr(pair.key, "POSITION"))
                {
                    posIndex = pair.value;
                    const tg3_accessor& posAcc = raw.accessors[posIndex];
                    m_numberOfVertices += (uint)posAcc.count;
                }
            }
        }
    }

    // Process meshes
    for (uint32_t i = 0; i < raw.meshes_count; ++i)
    {
        const tg3_mesh& mesh = raw.meshes[i];
        processMesh(mesh, raw);
    }

    if (m_hasBones)
        extractSkinBones(raw);
}

std::shared_ptr<engine::Mesh> engine::GLtfMeshLoader::processMesh(const tg3_mesh& mesh, const tg3_model& raw)
{
    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    SceneSettings& sceneSettings = singleton->sceneSettings();

    std::vector<Vertex> vertices{};
    std::vector<unsigned int> indices{};
    std::shared_ptr<Material> material{};

    for (uint32_t p = 0; p < mesh.primitives_count; ++p)
    {
        const tg3_primitive& prim = mesh.primitives[p];

        uint32_t posIndex = UINT32_MAX;
        uint32_t norIndex = UINT32_MAX;
        uint32_t tanIndex = UINT32_MAX;
        uint32_t uvIndex = UINT32_MAX;

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

        bool hasPosition = (posIndex != UINT32_MAX);
        bool hasNormals = (norIndex != UINT32_MAX);
        bool hasTangents = (tanIndex != UINT32_MAX);
        bool hasTexCoords = (uvIndex != UINT32_MAX);


        const tg3_accessor& posAcc = raw.accessors[posIndex];
        const tg3_accessor& norAcc = raw.accessors[norIndex];
        const tg3_accessor& tanAcc = raw.accessors[tanIndex];
        const tg3_accessor& uvAcc = raw.accessors[uvIndex];



        const float* positions = nullptr;
        const float* normals = nullptr;
		const float* tangents = nullptr;
		const float* texcoords = nullptr;

        // position
        if (hasPosition)
        {
            const tg3_buffer_view& view = raw.buffer_views[posAcc.buffer_view];
            const tg3_buffer& buf = raw.buffers[view.buffer];
            const uint8_t* base = buf.data.data;
            const uint8_t* ptr = base + view.byte_offset + posAcc.byte_offset;
            positions = reinterpret_cast<const float*>(ptr);
        }

		// normals
        if (hasNormals)
		{
			const tg3_buffer_view& view = raw.buffer_views[norAcc.buffer_view];
			const tg3_buffer& buf = raw.buffers[view.buffer];
			const uint8_t* base = buf.data.data;
			const uint8_t* ptr = base + view.byte_offset + norAcc.byte_offset;
			normals = reinterpret_cast<const float*>(ptr);
		}

        // tangents
        if (hasTangents)
        {
            const tg3_buffer_view& view = raw.buffer_views[tanAcc.buffer_view];
            const tg3_buffer& buf = raw.buffers[view.buffer];
            const uint8_t* base = buf.data.data;
            const uint8_t* ptr = base + view.byte_offset + tanAcc.byte_offset;
            tangents = reinterpret_cast<const float*>(ptr);
        }

		// texcoords
        if (hasTexCoords)
        {
            const tg3_buffer_view& view = raw.buffer_views[uvAcc.buffer_view];
            const tg3_buffer& buf = raw.buffers[view.buffer];
            const uint8_t* base = buf.data.data;
            const uint8_t* ptr = base + view.byte_offset + uvAcc.byte_offset;
            texcoords = reinterpret_cast<const float*>(ptr);
        }

        vertices.reserve(posAcc.count);




        // bones
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

        bool hasJoints = (jointsIndex != UINT32_MAX);
        bool hasWeights = (weightsIndex != UINT32_MAX);

        const tg3_accessor* jointsAcc = nullptr;
        const tg3_accessor* weightsAcc = nullptr;

        if (hasJoints)
            jointsAcc = &raw.accessors[jointsIndex];

        if (hasWeights)
            weightsAcc = &raw.accessors[weightsIndex];



        const float* weights = nullptr;
        const uint16_t* joints = nullptr;

        if (weightsAcc)
        {
            const tg3_buffer_view& view = raw.buffer_views[weightsAcc->buffer_view];
            const tg3_buffer& buf = raw.buffers[view.buffer];
            const uint8_t* base = buf.data.data;
            const uint8_t* ptr = base + view.byte_offset + weightsAcc->byte_offset;
            weights = reinterpret_cast<const float*>(ptr);
        }

        if (jointsAcc)
        {
            const tg3_buffer_view& view = raw.buffer_views[jointsAcc->buffer_view];
            const tg3_buffer& buf = raw.buffers[view.buffer];
            const uint8_t* base = buf.data.data;
            const uint8_t* ptr = base + view.byte_offset + jointsAcc->byte_offset;
            joints = reinterpret_cast<const uint16_t*>(ptr);
        }





        for (uint32_t i = 0; i < posAcc.count; ++i)
        {
            Vertex v{ glm::vec3() };

            v.position = glm::vec3(
                positions[i * 3 + 0],
                positions[i * 3 + 1],
                positions[i * 3 + 2]
            );

            if (hasNormals && normals)
                v.normal = glm::vec3(
                    normals[i * 3 + 0],
                    normals[i * 3 + 1],
                    normals[i * 3 + 2]
                );

            if (hasTexCoords && texcoords)
                v.texCoords = glm::vec2(
                    texcoords[i * 2 + 0],
                    texcoords[i * 2 + 1]
                );

            if (hasTangents && tangents)
                v.tangent = glm::vec3(
                    tangents[i * 4 + 0],
                    tangents[i * 4 + 1],
                    tangents[i * 4 + 2]
                );

            vertices.push_back(std::move(v));



            // bones
            if (m_hasBones && joints && weights)
            {
                for (int k = 0; k < 4; ++k) // glTF always uses 4 influences
                {
                    int jointIndex = joints[i * 4 + k];
                    float weight = weights[i * 4 + k];

                    if (weight > 0.0f)
                    {
                        setVertexBoneData(vertices[i], jointIndex, weight);
                    }
                }
            }
        }




        // Indices
        const tg3_accessor& idxAcc = raw.accessors[prim.indices];
        const tg3_buffer_view& idxView = raw.buffer_views[idxAcc.buffer_view];
        const tg3_buffer& idxBuf = raw.buffers[idxView.buffer];

        const uint8_t* idxBase = idxBuf.data.data;
        const uint8_t* idxPtr = idxBase + idxView.byte_offset + idxAcc.byte_offset;


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



        


        


        // Material
        //if (m_customMaterial) // TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //{
        //    // use a user defined material
        //    m_materials.push_back(m_customMaterial);
        //}
        //else
        //{
            std::shared_ptr<engine::Material> material{};

            if (sceneSettings.method == RenderMethod::PBR)
            {
                material = loadPBRMaterial(prim.material, raw);
            }
            else
            {
                logger.warn("Using GLTF models is not recommended for legacy BlinnPhong scene");
            
                material = loadBlinnPhongMaterial(prim.material, raw);
            }

            m_materials.push_back(material);
        //}

        // load all textures asynchronously
        if (m_materials.back()->hasTextureMap())
            m_materials.back()->loadTexturesAsync(false);
    }

    auto meshPtr = std::make_shared<engine::Mesh>(toStdString(mesh.name), std::move(vertices), std::move(indices), m_materials.back());
    m_meshes.push_back(meshPtr);

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

        for (uint32_t j = 0; j < skin.joints_count; ++j)
        {
            int nodeIndex = skin.joints[j];
            const tg3_node& node = raw.nodes[nodeIndex];

            std::string boneName = toStdString(node.name);

            BoneInfo info{};
            //info.id = m_boneCounter++;
            info.id = j;  // ✅ match JOINTS_0 indices


            glm::mat4 ibm{};
            memcpy(glm::value_ptr(ibm), ibmData + j * 16, sizeof(float) * 16);

            info.offset = ibm;

            m_boneInfoMap[boneName] = info;

            m_boneCounter = std::max(m_boneCounter, (int)skin.joints_count);
        }
    }
}


engine::GLtfMeshLoader::~GLtfMeshLoader()
{
    logger.trace("GLtfMeshLoader destructor called");
}
