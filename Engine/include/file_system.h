#pragma once

#include <string>
#include <cstdlib>
#include <filesystem>

namespace engine
{
    class file_system
    {
    private:
        using Builder = std::string(*)(const std::string& path);

    public:
        static std::string getPath(const std::string& path);

    private:
        static const std::string& getRoot();
        static Builder getPathBuilder();
        static std::string getPathRelativeRoot(const std::string& path);
        static std::string getPathRelativeBinary(const std::string& path);
    };
}
