#pragma once

#include "core/include/app/app.h"
#include "core/include/app/scene.h"

using namespace std;
using namespace engine;


class MyScene18 final : public Scene
{
public:
    MyScene18(const string& _title, std::weak_ptr<App> _app);
    MyScene18(const string& _title, std::weak_ptr<App> _app, const SceneSettings& settings);
    ~MyScene18() override;

    void init() override;
    void update(Shader& shader) override;
    void updateUI() override;
    void clean() override;

    void onInputUpdate() override;

    void framebuffer_size_callback(int newWidth, int newHeight);
};
