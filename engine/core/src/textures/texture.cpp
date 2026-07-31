#include "../../include/textures/texture.h"

#include "../../include/common_defines.h"

//engine::Texture::Texture()
//{
//    //logger.trace("Texture constructor called");
//}

engine::Texture::Texture(unsigned int id, const std::string& type, const std::string& path)
    : id(id), type(type), path(path)
{
    //logger.trace("Texture constructor called");
}

//engine::Texture::~Texture()
//{
//    // destructor called many many times, don't know why
//    //logger.trace("Texture destructor called");
//
//    // static methods so shouldn't clean anything here
//    //engine::TextureManager::textureIDCache.clear();
//    //engine::TextureManager::textureDataCache.clear();
//    //engine::TextureManager::textureCache.clear();
//}

