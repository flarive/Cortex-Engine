//#pragma once
//
//#include "file_system.h"
//
//#include "SOIL2.h"
//
//#include <string>
//
//class texture_helper
//{
//public:
//	
//    static unsigned int soil_load_image(std::string filename, bool alpha = false, bool repeat = true)
//    {
//        unsigned int texture;
//        glGenTextures(1, &texture);
//        glBindTexture(GL_TEXTURE_2D, texture);
//
//        if (repeat)
//        {
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//        }
//
//        // set the texture wrapping/filtering options (on the currently bound texture object)
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//
//        // load and generate the texture
//        int width, height;
//        unsigned char* data = SOIL_load_image(file_system::getPath(filename).c_str(), &width, &height, 0, alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
//        if (data)
//        {
//            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
//            glGenerateMipmap(GL_TEXTURE_2D);
//        }
//        else
//        {
//            std::cerr << "Failed to load image" << std::endl;
//            exit(EXIT_FAILURE);
//        }
//
//        SOIL_free_image_data(data);
//
//        return texture;
//    }
//
//
//    static unsigned int soil_load_texture(std::string filename, bool alpha = false, bool repeat = true)
//    {
//        unsigned int texture;
//        glGenTextures(1, &texture);
//        glBindTexture(GL_TEXTURE_2D, texture);
//
//        if (repeat)
//        {
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//        }
//
//        // set the texture wrapping/filtering options (on the currently bound texture object)
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//        texture = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
//        (
//            file_system::getPath(filename).c_str(),
//            SOIL_LOAD_AUTO,
//            SOIL_CREATE_NEW_ID,
//            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
//        );
//
//        if (texture <= 0)
//        {
//            std::cerr << "Failed to load texture" << std::endl;
//            exit(EXIT_FAILURE);
//        }
//
//        return texture;
//    }
//};
