#include "../../include/tools/file_system.h"

std::string engine::FileSystem::getPath(const std::string& path)
{
    static Builder pathBuilder = getPathBuilder();
    return (*pathBuilder)(path);
}

std::string engine::FileSystem::getDirectoryPath(const std::string& path)
{
    std::filesystem::path full = std::filesystem::absolute(path).lexically_normal();
    std::filesystem::path folder = full.parent_path();
    return folder.string();
}

std::string engine::FileSystem::getFilename(const std::string& path)
{
    std::filesystem::path full = std::filesystem::absolute(path).lexically_normal();
    return full.filename().string();
}

std::string engine::FileSystem::getShortenedPath(const std::string& path)
{
    std::filesystem::path full = std::filesystem::relative(path).lexically_normal();
    std::filesystem::path dir(std::filesystem::current_path());
    return full.string();
}

const std::string& engine::FileSystem::getRoot()
{
    // C:\Users\flarive\Documents\vs2022\Projects\LOGL\x64\Debug
    std::filesystem::path dir(std::filesystem::current_path());
    std::string logl_root = dir.generic_string();

    static const char* envRoot = nullptr;// getenv("LOGL_ROOT_PATH"); // to trash !!!!!!!!!!!!!!!!
    static const char* givenRoot = (envRoot != nullptr ? envRoot : logl_root.c_str());
    static const std::string root = (givenRoot != nullptr ? givenRoot : "");
    return root;
}

engine::FileSystem::Builder engine::FileSystem::getPathBuilder()
{
    //if (!getRoot().empty())
        return &FileSystem::getPathRelativeRoot;
    /*else
        return &FileSystem::getPathRelativeBinary;*/
}

std::string engine::FileSystem::getPathRelativeRoot(const std::string& path)
{
    return getRoot() + "/" + path;
}

std::string engine::FileSystem::getPathRelativeBinary(const std::string& path)
{
    return "../../../" + path;
}
