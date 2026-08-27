#include "../../include/managers/filesystem_manager.h"

std::string engine::FileSystemManager::getPath(const std::string& path)
{
    if (path.empty())
        return "";

    static Builder pathBuilder = getPathBuilder();
    return (*pathBuilder)(path);
}

std::string engine::FileSystemManager::getDirectoryPath(const std::string& path)
{
    if (path.empty())
        return "";

    std::filesystem::path full = std::filesystem::absolute(path).lexically_normal();
    std::filesystem::path folder = full.parent_path();
    return folder.string();
}

std::string engine::FileSystemManager::getFileName(const std::string& path)
{
    if (path.empty())
        return "";

    std::filesystem::path full = std::filesystem::absolute(path).lexically_normal();
    return full.filename().string();
}

std::string engine::FileSystemManager::getFullPath(const std::string& path)
{
    if (path.empty())
        return "";
    
    std::filesystem::path full = std::filesystem::absolute(path).lexically_normal();
    return full.string();
}

std::string engine::FileSystemManager::getShortenedPath(const std::string& path)
{
    if (path.empty())
        return "";

    std::filesystem::path full = std::filesystem::relative(path).lexically_normal();
    return full.string();
}

const std::string engine::FileSystemManager::getFileExt(const std::string& filename)
{
    // Find last dot
    const size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos || pos == filename.size() - 1)
        return ""; // no extension or trailing dot

    // Extract extension
    std::string ext = filename.substr(pos + 1);

    // Normalize to lowercase (optional)
    std::transform(ext.begin(), ext.end(), ext.begin(),
        [](unsigned char c) { return std::tolower(c); });

    return ext;
}


const std::string& engine::FileSystemManager::getRoot()
{
    std::filesystem::path dir(std::filesystem::current_path());
    std::string logl_root = dir.generic_string();

    static const char* envRoot = nullptr;// getenv("LOGL_ROOT_PATH"); // to trash !!!!!!!!!!!!!!!!
    static const char* givenRoot = (envRoot != nullptr ? envRoot : logl_root.c_str());
    static const std::string root = (givenRoot != nullptr ? givenRoot : "");
    return root;
}

engine::FileSystemManager::Builder engine::FileSystemManager::getPathBuilder()
{
    //if (!getRoot().empty())
        return &FileSystemManager::getPathRelativeRoot;
    /*else
        return &FileSystem::getPathRelativeBinary;*/
}

std::string engine::FileSystemManager::getPathRelativeRoot(const std::string& path)
{
    return getRoot() + "/" + path;
}

std::string engine::FileSystemManager::getPathRelativeBinary(const std::string& path)
{
    return "../../../" + path;
}
