#pragma once

#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/engine.h"

using namespace std;
using namespace engine;

class MyScene4 final : public Scene
{
private:
    const std::string FONT_PATH{ "fonts/Antonio-Regular.ttf" };

    UIText ourText{};

    float rotation{};

    


public:
    MyScene4(const std::string& _title, std::weak_ptr<App> _app);
    ~MyScene4() override;

    void init() override;
   
    void handleInput() override;

    void gamepad_callback(const GLFWgamepadstate& state);

    void framebuffer_size_callback(int newWidth, int newHeight);


    void update(Shader& shader) override;

    void updateUI() override;

    void clean() override;


private:
    void drawScene(Shader& shader);

    void drawUI();
};
