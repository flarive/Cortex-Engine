#pragma once

#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/engine.h"

using namespace std;
using namespace engine;

class MyScene3 final : public Scene
{
private:
    const string FONT_PATH{ "fonts/Antonio-Regular.ttf" };

 

    UIText ourText{};
    //UISprite ourSprite{};

    float rotation{};

public:
    MyScene3(const string& _title, std::weak_ptr<App> _app);
    ~MyScene3() override;


    void init() override;
   

    void handleInput() override;

    void gamepad_callback(const GLFWgamepadstate& state);

    void framebuffer_size_callback(int newWidth, int newHeight);


    void update(engine::Shader& shader) override;

    void updateUI() override;

    void clean() override;


private:
    void drawScene(Shader& shader);

    void drawUI();
};
