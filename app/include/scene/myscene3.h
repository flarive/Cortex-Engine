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
   

    void onInputUpdate() override;

    void framebuffer_size_callback(int newWidth, int newHeight);


    void update(std::unique_ptr<engine::ShaderBase>& shader) override;

    void updateUI() override;

    void clean() override;


private:
    void drawScene(std::unique_ptr<ShaderBase>& shader);

    void drawUI();
};
