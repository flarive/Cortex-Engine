#include "../../include/models/gltf_loader.h"

#include "../../include/managers/log_manager.h"
#include "../../include/managers/filesystem_manager.h"
#include "../../include/managers/texture_manager.h"

#include "../../include/singleton.h"


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
                //else if (tg3_str_equals_cstr(pair.key, "NORMAL"))
                //    norIndex = pair.value;

                //else if (tg3_str_equals_cstr(pair.key, "TANGENT"))
                //    tanIndex = pair.value;

                //else if (tg3_str_equals_cstr(pair.key, "TEXCOORD_0"))
                //    uvIndex = pair.value;
            }
        }
    }

    // Process meshes
    for (uint32_t i = 0; i < raw.meshes_count; ++i)
    {
        const tg3_mesh& mesh = raw.meshes[i];
        processMesh(mesh, raw);
    }
}

std::shared_ptr<engine::Mesh> engine::GLtfMeshLoader::processMesh(const tg3_mesh& mesh, const tg3_model& raw)
{
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
        m_materials.push_back(loadMaterial(prim.material, raw));
        //}

        // load all textures asynchronously
        if (m_materials.back()->hasTextureMap())
            m_materials.back()->loadTexturesAsync(true);

        // load bones
        //if (m_hasBones)
        //    extractBoneWeightForVertices(vertices, mesh, scene);
    }

    auto meshPtr = std::make_shared<engine::Mesh>(toStdString(mesh.name), std::move(vertices), std::move(indices), m_materials.back());
    m_meshes.push_back(meshPtr);

    return meshPtr;
}


std::shared_ptr<engine::Material> engine::GLtfMeshLoader::loadMaterial(uint32_t matIndex, const tg3_model& raw)
{
    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    SceneSettings& sceneSettings = singleton->sceneSettings();

    const tg3_material& mat = raw.materials[matIndex];

    Color baseColorFactor{
        mat.pbr_metallic_roughness.base_color_factor[0],
        mat.pbr_metallic_roughness.base_color_factor[1],
        mat.pbr_metallic_roughness.base_color_factor[2],
        mat.pbr_metallic_roughness.base_color_factor[3]
    };

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

    if (sceneSettings.method == RenderMethod::PBR)
    {
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
    }
    else
    {
        material = std::make_shared<BlinnPhongMaterial>(baseColorFactor, baseColorTex, metallicTex, normalTex, heightTex, emissiveTex, 1.0f);
    }

    material->setName(toStdString(mat.name));

    return material;
}

//std::string engine::GLtfMeshLoader::getTexture(const tg3_model& raw, const tg3_texture_info& info)
//{
//    if (info.index < 0)
//        return "";
//
//    const tg3_texture& tex = raw.textures[info.index];
//    const tg3_image& img = raw.images[tex.source];
//
//    std::string path = m_directory + "\\" + toStdString(img.uri);
//
//    bool skip = std::find(m_requestLoadingTextures.begin(), m_requestLoadingTextures.end(), path) != m_requestLoadingTextures.end();
//
//    if (skip)
//        return path;
//
//    if (toStdString(img.uri).size() == 0)
//    {
//        // Embedded texture
//        TextureUploadResult result;
//
//        if (img.height == 0)
//        {
//            // compressed
//            result = TextureManager::loadTextureFromMemory(
//                toUChar(img.image),
//                size_t(img.width * img.height * img.component),
//                toStdString(img.mime_type).c_str()
//            );
//        }
//        else
//        {
//            // uncompressed
//            result = TextureManager::loadUncompressedTexture(
//                toUChar(img.image),
//                img.width,
//                img.height
//            );
//        }
//
//        // Cache result
//        TextureManagerInternal::textureIDCache[path] = result.textureID;
//        TextureManagerInternal::textureDataCache[path] = TextureData{ result.textureID, toUChar(img.image), path, result.width, result.height, result.nbComponents, result.thumbnailLevel };
//    }
//
//    // just to avoid loading 2 times the same texture path
//    m_requestLoadingTextures.push_back(path);
//    return path;
//}


std::string engine::GLtfMeshLoader::getTexture(const tg3_model& raw, const tg3_texture_info& info)
{
    if (info.index < 0)
        return "";

    const tg3_texture& tex = raw.textures[info.index];
    const tg3_image& img = raw.images[tex.source];

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

        return path;
    }

    // ------------------------------------------------------------
    // CASE 2: External texture (PNG/JPG file on disk)
    // ------------------------------------------------------------
    
    // just to avoid loading 2 times the same texture path
    m_requestLoadingTextures.push_back(path);

    return path;
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
    return std::string(s.data, s.len);
}

unsigned char* engine::GLtfMeshLoader::toUChar(tg3_span_u8 span)
{
    return const_cast<unsigned char*>(span.data);
}

engine::GLtfMeshLoader::~GLtfMeshLoader()
{
    logger.trace("GLtfMeshLoader destructor called");
}
