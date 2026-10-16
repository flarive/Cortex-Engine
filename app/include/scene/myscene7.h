#pragma once

#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/engine.h"

using namespace std;
using namespace engine;

class MyScene7 final : public Scene
{
private:
    const string FONT_PATH{ "fonts/Antonio-Regular.ttf" };

  


    UIText textFPSCount{};
    UIText textPolyCount{};
    UIText textMeshCount{};
    UIText textPrimitiveCount{};
    UISprite ourSprite{};


    float rotation{};

    float offset_dynamic = -15.0f;


public:
    MyScene7(const string& _title, std::weak_ptr<App> _app);
    ~MyScene7() override;

    void before_init_hook() override;
    void init() override;
    void after_init_hook() override;


    void handleInput() override;

    void gamepad_callback(const GLFWgamepadstate& state);

    void framebuffer_size_callback(int newWidth, int newHeight);

    void update(Shader& shader) override;

    void updateUI() override;

    void clean() override;
};
