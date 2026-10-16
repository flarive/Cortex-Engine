#pragma once

#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/engine.h"

using namespace std;
using namespace engine;

class MyScene10 final : public Scene
{
private:
    const string FONT_PATH{ "fonts/Antonio-Regular.ttf" };

    const int NUM_AREA_LIGHTS{ 16 };

    float rotation{};

    


public:
    MyScene10(const string& _title, std::weak_ptr<App> _app);
    ~MyScene10() override;

    void init() override;
    

    void handleInput() override;

    void gamepad_callback(const GLFWgamepadstate& state);

    void framebuffer_size_callback(int newWidth, int newHeight);

    void update(Shader& shader) override;

    void updateUI() override;

    void clean() override;


private:

    //engine::AudioManager m_audioManager;

    void drawScene(Shader& shader);

    void incrementRoughness(float step);
    void incrementLightIntensity(float step);
    void switchTwoSided(bool doSwitch);

    void playOggFile();

    void drawUI();
};
