#include "../include/file_system.h"

std::string engine::file_system::getPath(const std::string& path)
{
    static Builder pathBuilder = getPathBuilder();
    return (*pathBuilder)(path);
}

const std::string& engine::file_system::getRoot()
{
    std::filesystem::path dir(std::filesystem::current_path());
    std::string logl_root = dir.generic_string();

    static const char* envRoot = nullptr;// getenv("LOGL_ROOT_PATH"); // to trash !!!!!!!!!!!!!!!!
    static const char* givenRoot = (envRoot != nullptr ? envRoot : logl_root.c_str());
    static const std::string root = (givenRoot != nullptr ? givenRoot : "");
    return root;
}

engine::file_system::Builder engine::file_system::getPathBuilder()
{
    if (!getRoot().empty())
        return &file_system::getPathRelativeRoot;
    else
        return &file_system::getPathRelativeBinary;
}

std::string engine::file_system::getPathRelativeRoot(const std::string& path)
{
    return getRoot() + "/" + path;
}

std::string engine::file_system::getPathRelativeBinary(const std::string& path)
{
    return "../../../" + path;
}
