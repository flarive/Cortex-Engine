#pragma once

#include "../engine.h"

#include "../misc/noncopyable.h"
#include "../tools/file_system.h"
#include "../tools/system_monitor.h"

#include <iostream>
#include <chrono>
#include <thread>

namespace engine
{


    struct AppSettings
    {

    };
    
    /// <summary>
    /// https://stackoverflow.com/questions/31581200/glfw-call-to-non-static-class-function-in-static-key-callback
    /// </summary>
    class App : private NonCopyable
    {
    private:
        const int TARGET_FPS{ 60 };
        const int FRAME_DELAY{ 1000 / TARGET_FPS }; // in milliseconds



        bool key_w_pressed { false };


        bool show_window{ false };



        const unsigned int SHADOW_WIDTH{ 2048 }, SHADOW_HEIGHT{ 2048 };

        unsigned int depthMapFramebuffer{};
        unsigned int colorFramebuffer{};



        unsigned int rbo{}; // renderbuffer object



        unsigned int textureDepthMapBuffer{};
        unsigned int textureColorbuffer{};


        Shader screenShader{};
        Shader simpleDepthShader{};
        Shader debugDepthQuad{};

        
        SystemMonitor sysMonitor{};

        GLuint query;

    
    protected:
        float framerate{};

        // timing
        float deltaTime{}; // time between current frame and last frame
        float lastFrame{};

        //GLint polycount{};

        // settings
        std::string title{};
        int width{}; // windowed width
        int height{}; // windowed height
        bool fullscreen{};
        
        AppSettings settings;

    public:
        GLFWwindow* window{};

        // default camera
        //engine::Camera camera{ glm::vec3(0.0f, 0.0f, 3.0f), true };

        //std::vector<std::shared_ptr<engine::Light>> lights{};

        //Shader blinnPhongShader{};
        //Shader pbrShader{};
        //Shader skyboxReflectShader{};
        //Shader backgroundShader{};

        // PBR
        //Shader equirectangularToCubemapShader{};
        //Shader irradianceShader{};
        //Shader prefilterShader{};
        //Shader brdfShader{};

 


        //unsigned int irradianceMap;
        //unsigned int prefilterMap;
        //unsigned int brdfLUTTexture;
        //unsigned int envCubemap{};


        
        App(std::string _title, unsigned int _width, unsigned int _height, bool _fullscreen, AppSettings _settings)
            : title(_title), width(_width), height(_height), fullscreen(_fullscreen), settings(_settings)
        {

        }



    private:

    };
}