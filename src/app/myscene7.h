#pragma once

#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/engine.h"



class MyScene7 : public engine::Scene
{
private:
    bool firstMouse{ true };

    float lastX{ 0.0f };
    float lastY{ 0.0f };

    const std::string FONT_PATH = "fonts/Antonio-Regular.ttf";

  


    engine::Text textFPSCount{};
    engine::Text textPolyCount{};
    engine::Text textMeshCount{};
    engine::Text textPrimitiveCount{};
    engine::Sprite ourSprite{};


    float rotation{};

    float offset_dynamic = -15.0f;


public:
    MyScene7(std::string _title, engine::App* _app)
        : engine::Scene(_title, _app, engine::SceneSettings
            {
                .method = engine::RenderMethod::PBR,
                .HDRSkyboxHide = false,
                .HDRSkyboxFilePath = "textures/hdr/blue_photo_studio_2k.hdr",
                .HDRSkyboxBlurStrength = 0.0f,
                .shadowIntensity = 1.0f,
                .iblDiffuseIntensity = 1.0f,
                .iblSpecularIntensity = 1.0f
            })
    {
        // my application specific state gets initialized here

        lastX = app->width / 2.0f;
        lastY = app->height / 2.0f;
    }

    void before_init_hook() override
    {
        // do something here if needed
    }

    void init() override
    {
        // cameras
        auto trsCamera1 = engine::Transform{ { 0.0f, -16.0f, 2.0f } };
        auto camera1 = std::make_shared<engine::FlyCamera>();
        camera1->zoom = 100.0f;
        camera1->movementSpeed = 10.0f;
        auto EntityCamera1 = std::make_shared<engine::Entity>("Camera1", camera1, trsCamera1);
        getEntityManager().addChild(EntityCamera1);





        // lights
        auto trsLight1 = engine::Transform{ { -10.0f, -5.0f, -10.0f } };
        auto light1 = std::make_shared<engine::PointLight>(0);
        light1->intensity = 40.0f;
        auto entityLight1 = std::make_shared<engine::Entity>("Light1", light1, trsLight1);
        getEntityManager().addChild(entityLight1);





        auto trsLight2 = engine::Transform{ { 10.0f, -5.0f, -10.0f } };
        auto light2 = std::make_shared<engine::PointLight>(1);
        light2->intensity = 40.0f;
        auto entityLight2 = std::make_shared<engine::Entity>("Light2", light2, trsLight2);
        getEntityManager().addChild(entityLight2);



        auto trsLight3 = engine::Transform{ { -10.0f, -25.0f, -10.0f } };
        auto light3 = std::make_shared<engine::PointLight>(2);
        light3->intensity = 40.0f;
        auto entityLight3 = std::make_shared<engine::Entity>("Light3", light3, trsLight3);
        getEntityManager().addChild(entityLight3);



        auto trsLight4 = engine::Transform{ { 10.0f, -25.0f, -10.0f } };
        auto light4 = std::make_shared<engine::PointLight>(3);
        light4->intensity = 40.0f;
        auto entityLight4 = std::make_shared<engine::Entity>("Light4", light4, trsLight4);
        getEntityManager().addChild(entityLight4);






        


        

        std::shared_ptr<engine::Model> model = std::make_shared<engine::Model>("models/helmet/DamagedHelmet.glTF", false, true);


        // sample flat entity hierarchy
        float offset = -15.0f;
        for (unsigned int i = 1; i <= 7; ++i)
        {
            auto trs = engine::Transform{};
            trs.setLocalPosition({ offset, -22.0f, -10.0f });
            trs.setLocalScale(glm::vec3(2.0f));
            trs.setLocalRotation({ 0.0f, 180.0f, 0.0f });


            std::shared_ptr<engine::Entity> entity = std::make_shared<engine::Entity>(std::format("Child{}", i), model, trs);
            getEntityManager().addChild(entity);

            offset += 5.0f;
        }


        // sample nested entity hierarchy
        offset = -15.0f;
        std::shared_ptr<engine::Entity> lastEntity = getEntityManager().getRootEntity();
        for (unsigned int i = 1; i < 8; ++i)
        {
            auto trs = engine::Transform{};

            if (i == 1)
            {
                // parent
                trs.setLocalPosition({ offset, -12.0f, -10.0f });
                trs.setLocalScale(glm::vec3(2.0f));
                trs.setLocalRotation({ 0.0f, 180.0f, 0.0f });

                offset = 0.0f;
            }
            else
            {
                // childs
                trs.setLocalPosition({ offset, 0.0f, 0.0f });
                trs.setLocalScale(glm::vec3(1.0f));
                trs.setLocalRotation({ 0.0f, 0.0f, 0.0f });
            }

            std::shared_ptr<engine::Entity> entity = std::make_shared<engine::Entity>(std::format("NestedChild{}", i), model, trs);
            getEntityManager().addChild(lastEntity, entity);

            if (i == 1)
                lastEntity = lastEntity->children.back();

            offset -= 2.5f;
        }

        


        // ground
        auto myPlane = std::make_shared<engine::Plane>();
        myPlane->setup(std::make_shared<engine::PBRMaterial>(engine::Color(0.1f),
            "textures/rusted_metal_diffuse.jpg",
            "textures/rusted_metal_specular.jpg"), engine::UvMapping(1.0f));

        auto trsPlane = engine::Transform(glm::vec3(0.0f, -10.0f, -10.0f), glm::vec3(10.0f), glm::vec3(-90.0f, 0.0f, 0.0f));
        auto entityPlane = std::make_shared<engine::Entity>("MyPlane", myPlane, trsPlane);
        getEntityManager().addChild(entityPlane);


        // cube
        auto myCube = std::make_shared<engine::Cube>();
        myCube->setup(std::make_shared<engine::PBRMaterial>(engine::Color(0.1f),
            "textures/container2_diffuse.png",
            "textures/container2_specular.png"));

        auto trsCube = engine::Transform(glm::vec3(-3.0f, -17.0f, -10.0f), glm::vec3(2.0f), glm::vec3(90.0f, 0.0f, 0.0f));
        auto entityCube = std::make_shared<engine::Entity>("MyCube", myCube, trsCube);
        getEntityManager().addChild(entityCube);


        // sphere
        auto mySphere = std::make_shared<engine::Sphere>();
        mySphere->setup(std::make_shared<engine::PBRMaterial>(engine::Color(0.1f),
            "textures/uv_mapper.jpg"));

        auto trsSphere = engine::Transform(glm::vec3(3.0f, -17.0f, -10.0f), glm::vec3(2.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        auto entitySphere = std::make_shared<engine::Entity>("MySphere", mySphere, trsSphere);
        getEntityManager().addChild(entitySphere);






        textFPSCount.setup(app->window, FONT_PATH, 28);
        textPolyCount.setup(app->window, FONT_PATH, 28);
        textMeshCount.setup(app->window, FONT_PATH, 28);
        textPrimitiveCount.setup(app->window, FONT_PATH, 28);
        ourSprite.setup(app->window, "textures/awesomeface.png");
    }


    void after_init_hook() override
    {
        // do something here if needed
    }




    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    void key_callback(int key, int scancode, int action, int mods)
    {
        engine::Scene::key_callback(key, scancode, action, mods);

        // Detect Shift key state
        //bool shiftPressed = (mods & GLFW_MOD_SHIFT);

        if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            getActiveCamera()->processKeyboard(engine::LEFT, deltaTime);
            getActiveCamera()->processKeyboard(engine::YAW_DOWN, deltaTime);
        }

        if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            getActiveCamera()->processKeyboard(engine::RIGHT, deltaTime);
            getActiveCamera()->processKeyboard(engine::YAW_UP, deltaTime);
        }

        if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            getActiveCamera()->processKeyboard(engine::FORWARD, deltaTime);
        }

        if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            getActiveCamera()->processKeyboard(engine::BACKWARD, deltaTime);
        }

        if (key == GLFW_KEY_N && action == GLFW_PRESS)
        {
            auto trs = engine::Transform{};
            trs.setLocalPosition({ offset_dynamic, -32.0f, -10.0f });
            trs.setLocalScale(glm::vec3(2.0f));
            trs.setLocalRotation({ 0.0f, 180.0f, 0.0f });

            std::shared_ptr<engine::Model> model = std::make_shared<engine::Model>("models/helmet/DamagedHelmet.glTF", false, true);
            std::shared_ptr<engine::Entity> entity = std::make_shared<engine::Entity>(std::format("Child{}", 88), model, trs);
            getEntityManager().addChild(entity);

            offset_dynamic += 5.0f;
        }

        if (key == GLFW_KEY_M && action == GLFW_PRESS)
        {
            getEntityManager().remove("Child88");
        }
    }


    void mouse_callback(double xposIn, double yposIn)
    {
        engine::Scene::mouse_callback(xposIn, yposIn);

        if (show_window)
            return;

        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        getActiveCamera()->processMouseMovement(xoffset, yoffset);
    }

    void scroll_callback(double xoffset, double yoffset)
    {
        engine::Scene::scroll_callback(xoffset, yoffset);

        getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
    }

    void gamepad_callback(const GLFWgamepadstate& state)
    {
        getActiveCamera()->processJoystickMovement(state);

        //std::cout << "Left Stick X Axis: " << state.axes[0] << std::endl; // tested with PS4 controller connected via micro USB cable
        //std::cout << "Left Stick Y Axis: " << state.axes[1] << std::endl; // tested with PS4 controller connected via micro USB cable
        //std::cout << "Right Stick X Axis: " << state.axes[2] << std::endl; // tested with PS4 controller connected via micro USB cable
        //std::cout << "Right Stick Y Axis: " << state.axes[3] << std::endl; // tested with PS4 controller connected via micro USB cable
        //std::cout << "Left Trigger/L2: " << state.axes[4] << std::endl; // tested with PS4 controller connected via micro USB cable
        //std::cout <<
        // "Right Trigger/R2: " << state.axes[5] << std::endl; // tested with PS4 controller connected via micro USB cable

        if (GLFW_PRESS == state.buttons[1])
        {
            std::cout << "Pressed" << std::endl;
        }
        else if (GLFW_RELEASE == state.buttons[0])
        {
            //std::cout << "Released" << std::endl;
        }
    }

    void framebuffer_size_callback(int newWidth, int newHeight)
    {
        engine::Scene::framebuffer_size_callback(newWidth, newHeight);
    }

    void update(engine::Shader& shader) override
    {
        (void)shader;   //Do nothing


        //auto child2 = getEntityManager().findEntityByName("Child2");
        //if (child2)
        //{
        //    child2->transform.setLocalRotation(glm::vec3(rotation, 0.0f, 0.0f));
        //}


        //auto mySphere = getEntityManager().findEntityByName("MySphere");
        //if (mySphere)
        //{
        //    mySphere->transform.setLocalRotation(glm::vec3(0.0f, rotation, 0.0f));
        //}


        rotation += deltaTime * 10.0f;
    }

    void updateUI() override
    {
        // render HUD / UI
        textFPSCount.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(1.0f));
        textPolyCount.draw(std::format("{} polys", (int)polycount), app->width - 250.0f, 25.0f, 1.0f, glm::vec3(1.0f));
        textMeshCount.draw(std::format("{} meshes", (int)meshcount), app->width - 450.0f, 25.0f, 1.0f, glm::vec3(1.0f));
        textPrimitiveCount.draw(std::format("{} primitives", (int)primitivecount), app->width - 650.0f, 25.0f, 1.0f, glm::vec3(1.0f));
        ourSprite.draw(glm::vec2(50, app->height - 50), glm::vec2(50.0f, -50.0f), 0.0f, glm::vec3(1.0f));
    }

    void clean() override
    {
        // clean up any resources
        //helmetModel.clean();
    }
};
