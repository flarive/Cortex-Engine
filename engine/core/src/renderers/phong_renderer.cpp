#include "../../include/renderers/phong_renderer.h"

#include "../../include/singleton.h"

#include "../../include/tools/file_system.h"
#include "../../include/debug/opengl_debug.h"

#include "../../include/lights/spot_light.h"
#include "../../include/lights/point_light.h"
#include "../../include/lights/directional_light.h"

#include "../../include/misc/colors.h"




engine::PhongRenderer::PhongRenderer(GLFWwindow* window)
    : Renderer(window)
{
}


void engine::PhongRenderer::setup(int width, int height, std::shared_ptr<Camera> camera, const std::vector<std::shared_ptr<Light>>& lights)
{
    m_lights = lights; // copy ?????????
    m_camera = camera; // copy ?????????


    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    const SceneSettings& settings = singleton->sceneSettings();

    // avoid computing back faces not visible by camera
    enableFaceCulling(false);
    
    // build and compile shaders
    // -------------------------
    loadShaders();

    // solid/wireframe polygons
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);// settings.drawAsWireframe ? GL_LINE : GL_FILL);
}

void engine::PhongRenderer::loop(int width, int height, std::shared_ptr<Camera> camera, std::function<void(Shader&, Shader&)> update, std::function<void()> updateUI)
{
    glm::mat4 projection = camera->getProjectionMatrix(width, height);
    glm::mat4 view = camera->getViewMatrix();

    // solid color background
    Color backgroundColor = Colors::Black;
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a); // background color
    glClear(GL_COLOR_BUFFER_BIT);

    updateEditorPropertySettings();

    phongShader.use();
    phongShader.setMat4("projection", projection);
    phongShader.setMat4("view", view);
    phongShader.setVec3("viewPos", camera->position);
    phongShader.setVec3("lightPos", glm::vec3(0.0f, 10.0f, 10.0f));
    phongShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    phongShader.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
    

    // update user stuffs
    update(phongShader, phongShader);

    // Resolve MSAA to screen or another texture FBO
    //glBindFramebuffer(GL_READ_FRAMEBUFFER, colorFramebuffer);
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Default framebuffer (screen)
    ////glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolveFBO);
    //glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // display UI/HUD above the scene and outside the framebuffer
    updateUI();
}

void engine::PhongRenderer::loadShaders()
{
    // blinn phong illumination model and lightning shader
    phongShader.init("blinnphong", "shaders/phong.vert", "shaders/phong.frag");

    // shared shaders
    Renderer::loadShaders();
}

void engine::PhongRenderer::setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount, unsigned int areaLightCount)
{
    m_pointLightCount = pointLightCount;
    m_dirLightCount = dirLightCount;
    m_spotLightCount = spotLightCount;
    m_areaLightCount = areaLightCount;

    // not really needed
}

engine::Shader& engine::PhongRenderer::getShader()
{
    return phongShader;
}

void engine::PhongRenderer::clean()
{
    Renderer::clean();
    
    // delete shaders
    phongShader.clean();
}