#pragma once

#include <string>
#include <cstdlib>
#include <filesystem>

#include "../misc/noncopyable.h"

namespace engine
{
    class FileSystemManager final : private NonCopyable
    {
    private:
        using Builder = std::string(*)(const std::string& path);

    public:
        static std::string getPath(const std::string& path);
        static std::string getDirectoryPath(const std::string& path);
        static std::string getFilename(const std::string& path);
        static std::string getFullPath(const std::string& path);
        static std::string getShortenedPath(const std::string& path);

    private:
        static const std::string& getRoot();
        static Builder getPathBuilder();
        static std::string getPathRelativeRoot(const std::string& path);
        static std::string getPathRelativeBinary(const std::string& path);

    };
}
