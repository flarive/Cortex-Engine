#pragma once

#include "app.h"
#include "../engine.h"


#include "../renderers/renderer.h"
#include "../renderers/blinnphong_renderer.h"
#include "../renderers/pbr_renderer.h"

#include "../misc/noncopyable.h"
#include "../tools/file_system.h"
#include "../debug/imgui_debug.h"

//#include "../managers/entity_manager.h"

#include <iostream>
#include <chrono>
#include <thread>

namespace engine
{
    
    class EntityManager;
    
    /// <summary>
    /// https://stackoverflow.com/questions/31581200/glfw-call-to-non-static-class-function-in-static-key-callback
    /// </summary>
    class Scene : private NonCopyable
    {
    private:
        bool key_w_pressed { false };

        

        
        ImGuiDebug m_debug{};

        GLuint query{};

        void before_init_internal();
        void after_init_internal();


        

    
    protected:
        float framerate{};

        // timing
        float deltaTime{}; // time between current frame and last frame
        float lastFrame{};

        GLint polycount{};
        GLint meshcount{};

        // settings
        std::string title{};

        App* app{};
        SceneSettings settings{};

        Renderer* m_renderer{};
        EntityManager* m_entityManager{};

        virtual void before_init_hook() {}; // Overridable by derived classes
        virtual void after_init_hook() {}; // Overridable by derived classes

    public:
        bool show_window{ false };

        // default camera
        engine::Camera camera{ glm::vec3(0.0f, 0.0f, 3.0f), true };

        std::shared_ptr<Entity> rootEntity{};

        std::vector<std::shared_ptr<engine::Light>> lights{};
        std::vector<std::shared_ptr<engine::Camera>> cameras{};


        Scene(std::string _title, App* _app, SceneSettings _settings);

        void initialize();


        // must be overridden in derived class
        virtual void init() = 0;

        void before_init();
        void after_init();

        // must be overridden in derived class
        virtual void update(Shader& shader) = 0;

        // must be overridden in derived class
        virtual void updateUI() = 0;

        // must be overridden in derived class
        virtual void clean() = 0;


        void gameLoop();

        void exit();

        GLFWwindow* getWindow();

        Renderer* getRenderer() { return m_renderer; };

        void drawEntities(Shader& shader);
        void drawEntityRecursive(const std::shared_ptr<engine::Entity>& entity, Shader& shader);


        // Find a single entity in the root entities hierarchy by name
        Entity& find(const std::string& name);

        

        // glfw: whenever a key is pressed or released, this callback is called
        // --------------------------------------------------------------------
        void key_callback(int key, int scancode, int action, int mods);
    

        // glfw: whenever the mouse moves, this callback is called
        // -------------------------------------------------------
        void mouse_callback(double xposIn, double yposIn);


        // glfw: whenever the mouse scroll wheel scrolls, this callback is called
        // ----------------------------------------------------------------------
        void scroll_callback(double xoffset, double yoffset);


        // https://github.com/SonarSystems/OpenGL-Tutorials/blob/master/GLFW%20Joystick%20Input/main.cpp
        void gamepad_callback(const GLFWgamepadstate& state);


        // glfw: whenever the window size changed (by OS or user resize) this callback function executes
        // ---------------------------------------------------------------------------------------------
        void framebuffer_size_callback(int newWidth, int newHeight);


        void refreshFullscreen();


        // renderCube() renders a 1x1 3D cube in NDC.
        // -------------------------------------------------
        unsigned int cubeVAO{};
        unsigned int cubeVBO{};
        void renderCube();
        

        // renderQuad() renders a 1x1 XY quad in NDC
        // -----------------------------------------
        unsigned int quadVAO{};
        unsigned int quadVBO{};
        void renderQuad();
      


        // renders (and builds at first invocation) a sphere
        // -------------------------------------------------
        unsigned int sphereVAO{};
        GLsizei indexCount{};
        void renderSphere();
     



    private:
        static void glfw_error_callback(int error, const char* description);

        // Function to count vertices and polygons
        void beginQuery();

        // Function to count vertices and polygons
        void endQuery();

        void countMeshes(std::shared_ptr<Entity>& entity);
    };
}