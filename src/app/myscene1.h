#pragma once

#include "core/include/app/app.h"
#include "core/include/app/scene.h"

class MyScene1 : public engine::Scene
{
private:
    bool firstMouse{ true };

    float lastX{ 0.0f };
    float lastY{ 0.0f };


    const std::string FONT_PATH = "fonts/Antonio-Regular.ttf";


    engine::Text ourText{};

    float rotation{};

public:
    MyScene1(std::string _title, engine::App* _app);

    void init() override;
    void update(engine::Shader& shader) override;
    void updateUI() override;
    void clean() override;

    void key_callback(int key, int scancode, int action, int mods);
    void mouse_callback(double xposIn, double yposIn);
    void scroll_callback(double xoffset, double yoffset);
    void gamepad_callback(const GLFWgamepadstate& state);
    void framebuffer_size_callback(int newWidth, int newHeight);
};
