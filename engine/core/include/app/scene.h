#pragma once

#include "app.h"
#include "../engine.h"

#include "../renderers/renderer.h"
#include "../renderers/phong_renderer.h"
#include "../renderers/blinnphong_renderer.h"
#include "../renderers/pbr_renderer.h"
#include "../renderers/parallax_renderer.h"

#include "../misc/noncopyable.h"
#include "../tools/file_system.h"
#include "../editor/imgui_editor.h"
#include "../debug/imgui_perf_overlay.h"

#include "../managers/entity_manager.h"
#include "../managers/audio_manager.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <cstdint>

namespace engine
{
    class EntityManager;

    /// <summary>
    /// https://stackoverflow.com/questions/31581200/glfw-call-to-non-static-class-function-in-static-key-callback
    /// </summary>
    class Scene : private NonCopyable
    {
    private:
        static Scene* currentInstance; // Static pointer to the current instance
        
        bool key_F1_pressed{};

        

        #if EDITOR_MODE
        ImGuiEditor m_editor{};
        #endif


        bool m_displayViewTransformGuizmo{ true };
        bool m_displayObjectTransformGuizmo{ false };


        ImGuiPerfOverlay m_perfOverlay{};

        

        void before_init_internal();
        void after_init_internal();





    protected:
        float framerate{};

        // timing
        float deltaTime{}; // time between current frame and last frame
        float lastFrameTime{};

        GLdouble cpuTime{};
        GLdouble gpuTime{};
        GLdouble uiTime{};

        GLint polycount{};
        GLint meshcount{};
        GLint primitivecount{};

        GLint64 totalElapsedFrames{};

        // camera frustrum culling
        GLint inFrustrumCount{};
        GLint totalFrustrumCount{};

        // settings
        std::string title{};

        App* app{};

        Renderer* m_renderer{};
        EntityManager m_entityManager{};
        AudioManager m_audioManager{};

        unsigned short m_activeCameraIndex{};

		std::shared_ptr<Entity> m_selectedEntity{};

        virtual void before_init_hook() {}; // Overridable by derived classes
        virtual void after_init_hook() {}; // Overridable by derived classes

    public:
        bool is_editor_mode{ false };
        bool show_demo_window{ false };
        bool show_perf_overlay{ true };
        

        
        std::vector<std::shared_ptr<engine::Light>> lights{};
        std::vector<std::shared_ptr<engine::Camera>> cameras{};





        Scene(const std::string& _title, App* _app, SceneSettings _settings);

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
        AudioManager& getAudioManager() { return m_audioManager; };

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

        void drawEntities(Shader& shader, Shader& shaderTessellation);
        void drawEntityRecursive(const std::shared_ptr<engine::Entity>& entity, Shader& shader, Shader& shaderTessellation, const glm::mat4& projection, const glm::mat4& view, const Frustum& camFrustum, const int& callsThisFrame);



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

        uint64_t getTotalElapsedFrames() const;

        //static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);


    private:
        
        int m_queryFrameIndex{ 0 }; // toggles between 0 and 1
        GLuint m_timerQuery[2]{};       // double-buffered GPU timer queries (for GL_TIME_ELAPSED)
        GLuint m_primitiveQuery[2]{};       // double-buffered primitive count queries (for GL_PRIMITIVES_GENERATED)

        void computeLightsIndexes();



        

        
        static void glfw_error_callback(int error, const char* description);

        void initQueries();
        void beginQuery() const;
        void endQuery();
        void cleanupQueries() const;

        void countItems(std::shared_ptr<Entity>& entity);

        #if EDITOR_MODE
        void listenForEditorChanges();

        void setEditorMode(glm::mat4& projection, glm::mat4& view);
        

        //void performRayCasting(double xpos, double ypos);

        //bool testRayAABBIntersection(
        //    const glm::vec3& rayOrigin,
        //    const glm::vec3& rayDirection,
        //    const engine::AABB* aabb,
        //    float& outDistance);

        #endif
    };
}