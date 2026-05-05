#pragma once

#include "core/include/app/app.h"
#include "core/include/app/scene.h"

using namespace std;
using namespace engine;


class MyScene1 final : public Scene
{
private:
    bool firstMouse{ true };

    float lastX{ 0.0f };
    float lastY{ 0.0f };


    const string FONT_PATH{ "fonts/Antonio-Regular.ttf" };


    UIText textFPSCount{};
    UIText textElapsedFramesCount{};

    

    UIText textPolyCount{};
    UIText textMeshCount{};
    UIText textPrimitiveCount{};


    UIText textDrawnCount{};
    UIText textTotalCount{};

    float rotation{};

public:
    MyScene1(const string& _title, std::weak_ptr<App> _app);

    void init() override;
    void update(Shader& shader) override;
    void updateUI() override;
    void clean() override;

    void key_callback(int key, int scancode, int action, int mods);
    void mouse_callback(double xposIn, double yposIn);
    void scroll_callback(double xoffset, double yoffset);
    void gamepad_callback(const GLFWgamepadstate& state);
    void framebuffer_size_callback(int newWidth, int newHeight);
};
