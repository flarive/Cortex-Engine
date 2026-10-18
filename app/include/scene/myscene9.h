#pragma once

#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/engine.h"

using namespace std;
using namespace engine;

class MyScene9 final : public Scene
{
private:
    const string FONT_PATH{ "fonts/Antonio-Regular.ttf" };



    UIText ourText{};
    UIText ourText2{};
    UISprite ourSprite{};
    UIText textMeshCount{};
    UIRectangle rect{};
    UIButton button{};

    float rotation{};

    


public:
    MyScene9(const string& _title, std::weak_ptr<App> _app);
    ~MyScene9() override;

    void init() override;
    
    void onInputUpdate() override;

    void framebuffer_size_callback(int newWidth, int newHeight);

    void update(Shader& shader) override;

    void updateUI() override;

    void clean() override;
};
