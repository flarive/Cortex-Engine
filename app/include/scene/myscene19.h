#pragma once

#include "core/include/app/app.h"
#include "core/include/app/scene.h"

using namespace std;
using namespace engine;


class MyScene19 final : public Scene
{
public:
    MyScene19(const string& _title, std::weak_ptr<App> _app);
    MyScene19(const string& _title, std::weak_ptr<App> _app, const SceneSettings& settings);
    ~MyScene19() override;

    void init() override;
    void update(Shader& shader) override;
    void updateUI() override;
    void clean() override;

    void onInputUpdate() override;

    void framebuffer_size_callback(int newWidth, int newHeight);
};
