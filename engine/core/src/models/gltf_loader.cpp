#include "../../include/models/gltf_loader.h"

#include "../../include/managers/log_manager.h"
#include "../../include/managers/filesystem_manager.h"


void engine::GLtfMeshLoader::loadModel(const std::string& path, bool flipUVs)
{
    tinygltf3::Model model;

    tg3_parse_options opts{};
    tg3_parse_options_init(&opts);
    opts.images_as_is = 1; // do not decode images

    tg3_error_stack errors{};
    tg3_error_stack_init(&errors);

    tg3_error_code err = tg3_parse_file(
        &model.raw(),
        &errors,
        path.c_str(),
        (uint32_t)path.size(),
        &opts
    );

    if (err != TG3_OK) {
        // handle errors
        return;
    }

    // Access raw C structs
    const tg3_model& raw = model.raw();

    // Example: number of meshes
    uint32_t meshCount = raw.meshes_count;
}

engine::GLtfMeshLoader::~GLtfMeshLoader()
{
    logger.trace("GLtfMeshLoader destructor called");
}
