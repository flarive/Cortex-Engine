#pragma once

#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/engine.h"



class MyScene7 : public engine::Scene
{
private:
    bool firstMouse{ true };

    float lastX{ 0.0f };
    float lastY{ 0.0f };

    const std::string FONT_PATH = "fonts/Antonio-Regular.ttf";

  


    engine::Text textFPSCount{};
    engine::Text textPolyCount{};
    engine::Text textMeshCount{};
    engine::Text textPrimitiveCount{};
    engine::Sprite ourSprite{};


    float rotation{};

    float offset_dynamic = -15.0f;


public:
    MyScene7(std::string _title, engine::App* _app);

    void before_init_hook() override;
    void init() override;
    void after_init_hook() override;


    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    void key_callback(int key, int scancode, int action, int mods);
    void mouse_callback(double xposIn, double yposIn);
    void scroll_callback(double xoffset, double yoffset);
    void gamepad_callback(const GLFWgamepadstate& state);

    void framebuffer_size_callback(int newWidth, int newHeight);

    void update(engine::Shader& shader) override;

    void updateUI() override;

    void clean() override;
};
