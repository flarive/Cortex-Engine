#pragma once

#include "core/include/app/app.h"
#include "core/include/app/scene.h"

using namespace std;
using namespace engine;


class MyScene16 final : public Scene
{
private:
    const string FONT_PATH{ "fonts/Antonio-Regular.ttf" };


    UIText textFPSCount{};



    UIText textPolyCount{};
    UIText textMeshCount{};
    UIText textPrimitiveCount{};


    UIText textDrawnCount{};
    UIText textTotalCount{};

    

    



    float rotation{};

public:
    MyScene16(const string& _title, std::weak_ptr<App> _app);
    MyScene16(const string& _title, std::weak_ptr<App> _app, const SceneSettings& settings);
    ~MyScene16() override;

    void init() override;
    void update(Shader& shader) override;
    void updateUI() override;
    void clean() override;

    void onInputUpdate() override;

    void framebuffer_size_callback(int newWidth, int newHeight);
};
