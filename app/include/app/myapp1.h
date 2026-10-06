#pragma once

#include "core/include/app/app.h"
#include "core/include/engine.h"

#include "../scene/myscene0.h" // blinnphong empty scene
#include "../scene/myscene1.h" // blinnphong with skybox
#include "../scene/myscene2.h" // blinnphong cushion
#include "../scene/myscene3.h" // PBR balls with HDR background
#include "../scene/myscene4.h" // PBR cushion
#include "../scene/myscene5.h" // PBR buddha
#include "../scene/myscene6.h" // PBR rotating helmet
#include "../scene/myscene7.h" // PBR multiple helmets
#include "../scene/myscene8.h" // blinnphong monochromatic point shadow
#include "../scene/myscene9.h" // PBR orbit camera
#include "../scene/myscene10.h" // PBR area lights demo
#include "../scene/myscene11.h" // blinnphong animated character
#include "../scene/myscene12.h" // PBR wood plane with misc materials
#include "../scene/myscene13.h" // blinnphong particles
#include "../scene/myscene14.h" // blinnphong/pbr terrain
#include "../scene/myscene15.h" // blinnphong/pbr parallax mapping
#include "../scene/myscene16.h" // ktx compressed textures
#include "../scene/myscene17.h" // material opacity and glass

using namespace std;
using namespace engine;

using MyScene = MyScene17;

class MyApp1 final : public App
{
public:
    MyApp1(const string& _title, unsigned int _width = 800, unsigned int _height = 600, bool _fullscreen = false);
    ~MyApp1() override;


    void start() override;
    
    void onKey(int key, int scancode, int action, int mods) override;
    void onMouseMove(double x, double y) override;
    void onScroll(double x, double y) override;
    void onResize(int w, int h) override;
    void onRefresh() override;
   

private:
    weak_ptr<Scene> m_scene{};
};
