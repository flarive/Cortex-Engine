#pragma once

#include "app.h"
#include "../engine.h"


#include "../renderers/renderer.h"
#include "../renderers/blinnphong_renderer.h"
#include "../renderers/pbr_renderer.h"

#include "../misc/noncopyable.h"
#include "../tools/file_system.h"
#include "../editor/imgui_editor.h"
#include "../debug/imgui_perf_overlay.h"

#include "../managers/entity_manager.h"

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
        bool key_w_pressed{ false };

        //#ifdef EDITOR_MODE
        ImGuiEditor m_debug{};
        //#endif

        ImGuiPerfOverlay m_perfOverlay{};

        

        void before_init_internal();
        void after_init_internal();





    protected:
        float framerate{};

        // timing
        float deltaTime{}; // time between current frame and last frame
        float lastFrame{};

        GLdouble cpuTime{};
        GLdouble gpuTime{};
        GLdouble uiTime{};

        GLint polycount{};
        GLint meshcount{};
        GLint primitivecount{};

        // camera frustrum culling
        GLint inFrustrumCount{};
        GLint totalFrustrumCount{};

        



        // settings
        std::string title{};

        App* app{};
        SceneSettings sceneSettings{};
        RenderSettings renderSettings{};

        Renderer* m_renderer{};
        EntityManager m_entityManager{};

        unsigned short m_activeCameraIndex{};

        unsigned int m_selectedEntityID{};

        virtual void before_init_hook() {}; // Overridable by derived classes
        virtual void after_init_hook() {}; // Overridable by derived classes

    public:
        bool is_editor_mode{ false };
        bool show_demo_window{ false };
        bool show_perf_overlay{ true };
        

        
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
        EntityManager getEntityManager() { return m_entityManager; }; // call it entity root may be ?

        std::shared_ptr<engine::Camera> getActiveCamera() const
        {
            assert(m_activeCameraIndex >= cameras.size() == 0 && "Bad camera index !");
            
            return cameras[m_activeCameraIndex];
        }

        void setActiveCamera(unsigned short index)
        {
            m_activeCameraIndex = index;
        }

        void initEntities();
        void initEntityRecursive(const std::shared_ptr<engine::Entity>& entity);

        void drawEntities(Shader& shader);
        void drawEntityRecursive(const std::shared_ptr<engine::Entity>& entity, Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Frustum& camFrustum);



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

        void window_refresh_callback();


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
        
        GLuint m_timerQuery{}; // for GL_TIME_ELAPSED
        GLuint m_primitiveQuery{}; // for GL_PRIMITIVES_GENERATED

        
        static void glfw_error_callback(int error, const char* description);

        // Function to get opengl performance counters
        void beginQuery();

        // Function to get opengl performance counters
        void endQuery();

        void countItems(std::shared_ptr<Entity>& entity);

        //#ifdef EDITOR_MODE
        void renderGizmo();
        void listenForEditorChanges();
        //#endif
    };
}