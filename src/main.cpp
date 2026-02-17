#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/managers/log_manager.h"

#include "app/myapp1.h"

#include "app/myscene1.h" // blinnphong with skybox
#include "app/myscene2.h" // blinnphong cushion
#include "app/myscene3.h" // pbr balls with HDR background
#include "app/myscene4.h" // pbr cushion
#include "app/myscene5.h" // pbr buddha
#include "app/myscene6.h" // pbr rotating helmet
#include "app/myscene7.h" // pbr multiple helmets
#include "app/myscene8.h" // monochromatic point shadow
#include "app/myscene9.h" // orbit camera
#include "app/myscene10.h" // area lights demo
#include "app/myscene11.h" // blinnphong animated character
#include "app/myscene12.h" // PBR wood plane with misc materials
#include "app/myscene13.h" // Particles

using namespace engine;

// make it easier to switch between apps
using MyApp = MyApp1;
using MyScene = MyScene4;

App* myApp{};
Scene* myScene{};


// Auto select Nvidia or AMD GPU instead of builtin intel GPU
extern "C" {
    __declspec(dllexport) uint32_t NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}


// Needed by main method
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
static void windowRefreshCallback(GLFWwindow* window);
static void gamepadUpdate();



// Startup method
int main(int, char**)
{
    // Init the app
    //myApp = new MyApp("MyApp", 1280, 720, false);
    myApp = new MyApp("MyApp", 320, 240, false);
    if (myApp)
    {
        // Init a scene in the app
        myScene = new MyScene("MyScene", myApp);
        if (myScene)
        {
            myScene->initialize();

            glfwSetFramebufferSizeCallback(myScene->getWindow(), framebufferSizeCallback);
            glfwSetKeyCallback(myScene->getWindow(), keyCallback);
            glfwSetCursorPosCallback(myScene->getWindow(), mouseCallback);
            glfwSetScrollCallback(myScene->getWindow(), scrollCallback);
            glfwSetWindowRefreshCallback(myScene->getWindow(), windowRefreshCallback);

            int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
            if (present > 0)
            {
                const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
                logger.info("Joystick present {}", name);
            }

            // start game loop
            while (myApp->isRunning())
            {
                gamepadUpdate(); // Update gamepad state
                myScene->gameLoop();
            }

            myScene->exit();
            myApp->exit();
        }
        else
        {
            logger.error("Failed to create the scene");
            return -1;
        }
    }
    else
    {
        logger.error("Failed to create application");
        return -1;
    }

    return 0;
}

// glfw: whenever a keyboard key is pressed, this callback is called
// -----------------------------------------------------------------
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)window;   //Do nothing

    ((MyScene*)myScene)->key_callback(key, scancode, action, mods);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    (void)window;   //Do nothing

    ((MyScene*)myScene)->mouse_callback(xposIn, yposIn);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    (void)window;   //Do nothing

    ((MyScene*)myScene)->scroll_callback(xoffset, yoffset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    (void)window;   //Do nothing

    ((MyScene*)myScene)->framebuffer_size_callback(width, height);
}

static void windowRefreshCallback(GLFWwindow* window)
{
    (void)window;   //Do nothing

    ((MyScene*)myScene)->window_refresh_callback();
}

// Poll gamepad input and forward to MyApp
// ---------------------------------------
static void gamepadUpdate()
{
    if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
    {
        GLFWgamepadstate state;
        if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
        {
            ((MyScene*)myScene)->gamepad_callback(state);
        }
    }
}
//
//
//
//
//Passer au contenu principal
//Minimize
//Restore
//Close
//
//M365 Copilot
//
//
//Conversation
//Agents
//
//
//Jira Cloud
//
//
//
//Tous les agents
//
//Créer un assistant
//
//Conversations
//
//In this Python code how can i test that audio_clip
//
//
//In opengl what is this error about ? OpenGL error a
//
//
//Following learnopengl.com tutorial how can i be su
//
//
//To convert Y up to Z up between Motion Builder and
//
//
//In github in my repo https ://github.com/flarive/Co
//
//
//Toutes les conversations
//FL
//Frédéric Larivé
//
//Mettre à niveau Copilot
//Copilot Chat(Essentiel)
//
//Automatique
//
//
//
//Aujourd’hui
//You said :
//In opengl what is this error about ? OpenGL error at glDrawArrays : 502
//Message : GL_INVALID_OPERATION error generated.State(s) are invalid : program texture usage.
//Source : API
//Type : Error
//ID : 1282
//Severity : High
//Copilot said :
//Copilot
//
//Raisonné pour 1s
//
//Addressing OpenGL errors
//
//Fixing texture sampler issues
//GL_INVALID_OPERATION(0x0502) at glDrawArrays with the debug message “State(s) are invalid : program texture usage” means the draw call found your program’s sampler / texture state is inconsistent or illegal.
//
//In plain terms : your currently bound program uses one or more sampler uniforms in ways that don’t match what’s actually bound to the corresponding texture units.
//
//Common causes
//Sampler type doesn’t match the bound texture target
//sampler2D must read from a GL_TEXTURE_2D binding, not GL_TEXTURE_CUBE_MAP, GL_TEXTURE_2D_ARRAY, etc.
//samplerCube → GL_TEXTURE_CUBE_MAP
//sampler2DArray → GL_TEXTURE_2D_ARRAY
//sampler2DMS → GL_TEXTURE_2D_MULTISAMPLE
//samplerBuffer → GL_TEXTURE_BUFFER, …and so on.
//Integer / unsigned samplers used with non - integer textures(or vice versa)
//isampler * requires an integer internal format(e.g., GL_R8I, GL_RGBA16I).
//usampler * requires an unsigned integer internal format(e.g., GL_R8UI, GL_RGBA32UI).
//Floating / normalized formats(e.g., GL_RGBA8, GL_RGBA16F) must be sampled with plain sampler * (not i * or u*).
//Shadow samplers misconfigured
//sampler2DShadow / samplerCubeShadow require a depth texture format(GL_DEPTH_COMPONENT*), and the texture must have GL_TEXTURE_COMPARE_MODE = GL_COMPARE_REF_TO_TEXTURE set.Missing compare mode or using a color texture with a shadow sampler will trigger this.
//Same texture unit used by two different sampler types in the same program
//Example : one uniform is sampler2D, another is samplerCube, both set to texture unit 0. This is illegal.Each texture unit used by the program must have a single, consistent sampler type.
//Texture completeness issues
//If the min filter requires mipmaps(e.g., GL_LINEAR_MIPMAP_LINEAR) but the texture has no complete mipmap chain, or base / max level invalid → incomplete texture → draw can error out.
//Array / cubemap completeness rules not satisfied(e.g., cube faces not all defined, mismatched sizes).
//No texture bound to a unit that the shader actually samples
//If a sampler uniform points to unit N and the shader executes a texture(...) on it, but that target binding on unit N is 0 (nothing bound) → invalid operation.
//Setting sampler uniforms before linking(or after relink without reassigning)
//Sampler uniform locations can change after linking.If you relink and don’t reassign uniform values, they can point to wrong units.
//Program pipeline / separable programs mismatch(if using separate shader objects)
//The active stages combined must present a consistent set of sampler types vs.texture units.
//Quick checklist to fix
//Validate the program right before drawing :
//
//
//
//
//C++
//glValidateProgram(program);
//GLint ok = GL_FALSE;
//glGetProgramiv(program, GL_VALIDATE_STATUS, &ok);
//if (!ok) {
//    GLint len = 0;
//    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
//    std::string log(len, '\0');
//    glGetProgramInfoLog(program, len, nullptr, log.data());
//    fprintf(stderr, "Validate log:\n%s\n", log.c_str());
//}
//Set sampler uniforms to unique texture units after linking :
//
//
//
//
//C++
//glUseProgram(program);
//glUniform1i(glGetUniformLocation(program, "albedoTex"), 0);
//glUniform1i(glGetUniformLocation(program, "normalTex"), 1);
//// etc…
//Bind the correct target on each unit to match the sampler type :
//
//
//
//
//C++
//glActiveTexture(GL_TEXTURE0);
//glBindTexture(GL_TEXTURE_2D, albedoTexId);      // matches sampler2D
//
//glActiveTexture(GL_TEXTURE1);
//glBindTexture(GL_TEXTURE_2D, normalTexId);      // matches sampler2D
//Verify internal formats vs.sampler kind :
//
//sampler2D ↔ normalized / floating(e.g., GL_RGBA8, GL_RGBA16F, GL_RGBA32F)
//isampler2D ↔ integer(e.g., GL_RGBA8I, GL_R32I)
//usampler2D ↔ unsigned integer(e.g., GL_RGBA8UI, GL_R32UI)
//For shadow samplers :
//
//
//
//
//C++
//glBindTexture(GL_TEXTURE_2D, shadowTex);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
//// Ensure internal format is GL_DEPTH_COMPONENT[16|24|32F] and depth data is provided.
//Ensure texture completeness :
//
//If min filter uses mipmaps, generate them :
//
//
//
//C++
//glBindTexture(GL_TEXTURE_2D, tex);
//glGenerateMipmap(GL_TEXTURE_2D);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//For cube maps, define all 6 faces with identical size / format.
//Avoid reusing the same unit for different sampler types in the same program.
//
//If you use program pipelines(glBindProgramPipeline / separable shaders), make sure combined stages don’t create sampler conflicts and that each stage’s samplers resolve to compatible bindings.
//
//Helpful debugging tips
//Enable KHR_debug to get detailed messages(like the one you pasted) :
//
//
//
//
//    C++
//    glEnable(GL_DEBUG_OUTPUT);
//glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
//glDebugMessageCallback(GLenum, GLenum type, GLuint id, GLenum severity,
//    GLsizei, const GLchar * msg, const void* {
//fprintf(stderr, "GL DEBUG: type=0x%x, id=%u, severity=0x%x, msg=%s\n",
//        type, id, severity, msg);
//    }, nullptr);
//This will often tell you exactly which sampler / uniform is at fault and which texture unit / target it expected.
//
//Dump your bindings right before the draw :
//
//Print each sampler uniform → which unit.
//For each unit, query which target is bound and (if you track it) the internal format.
//Temporarily simplify :
//
//Bind a single known - good GL_TEXTURE_2D with GL_RGBA8 and linear filtering to the sampler the shader actually uses; comment out other samplers and draw.Add bindings back one by one to find the first conflict.
//A minimal example pattern that avoids this error
//
//
//
//C++
//// After linking:
//glUseProgram(prog);
//GLint uAlbedo = glGetUniformLocation(prog, "uAlbedo");
//GLint uDepthShadow = glGetUniformLocation(prog, "uShadow");
//glUniform1i(uAlbedo, 0);    // texture unit 0
//glUniform1i(uDepthShadow, 1); // texture unit 1
//
//// Albedo: float sampler2D ← float/normalized 2D texture
//glActiveTexture(GL_TEXTURE0);
//glBindTexture(GL_TEXTURE_2D, texAlbedo);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//glGenerateMipmap(GL_TEXTURE_2D);
//
//// Shadow: sampler2DShadow ← depth 2D texture with compare mode
//glActiveTexture(GL_TEXTURE1);
//glBindTexture(GL_TEXTURE_2D, texShadow);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
//
//// Draw
//glUseProgram(prog);
//glDrawArrays(GL_TRIANGLES, 0, vertexCount);
//
//Afficher plus de lignes
//If you share :
//
//the GLSL sampler declarations,
//the glUniform1i assignments(which units),
//the binds per unit(glBindTexture target, internal formats, and filters),
//…I can pinpoint exactly which sampler / texture pairing is tripping the validation in your case.
//
//TL; DR: The driver is telling you your shader’s sampler(s) don’t match the textures currently bound(target / type / format / completeness / unit conflicts).Align sampler types with correct texture targets and formats, ensure completeness, give each sampler a unique, compatible unit, and set sampler uniforms after linking.
//
//
//
//
//
//
//
//
//You said :
//I this loop of my c++ opengl prb renderer, is everything ok ? void engine::PbrRenderer::loop(int width, int height, std::shared_ptr<Camera> camera, std::function<void(Shader&)> update, std::function<void()> updateUI)
//{
//    auto* singleton = engine::Singleton::getInstance();
//    assert(singleton != nullptr && "Singleton not initialized !");
//    const SceneSettings& settings = singleton->sceneSettings();
//
//    // bind to color framebuffer and draw scene as we normally would to color texture
//    glBindFramebuffer(GL_FRAMEBUFFER, colorFramebuffer);
//    glEnable(GL_DEPTH_TEST); // enable depth testing
//    glEnable(GL_STENCIL_TEST); // enable stencil test
//
//
//    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // background color
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // don't forget to clear the stencil buffer!
//
//    updateEditorPropertySettings();
//
//    glm::mat4 projection = camera->getProjectionMatrix(width, height, 0.1f, 100.0f);
//    glm::mat4 view = camera->getViewMatrix();
//
//    if (settings.showDebugGrid)
//        renderDebugPlaneGrid(projection, view);
//
//
//
//
//
//
//    // PBR shader
//    pbrShader.use();
//    pbrShader.setMat4("projection", projection);
//    pbrShader.setMat4("view", view);
//    pbrShader.setVec3("viewPos", camera->position);
//    pbrShader.setFloat("material.shadowIntensity", settings.shadowIntensity);
//    pbrShader.setInt("material.shadowCalculationMethod", settings.shadowCalculationMethod);
//    pbrShader.setFloat("material.shadowMapsBias", settings.shadowMapsBiasFactor);
//    pbrShader.setFloat("material.shadowMapsBlur", settings.shadowMapsBlur);
//
//
//    // bind pre-computed IBL data
//    glActiveTexture(GL_TEXTURE0 + 7);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
//    glActiveTexture(GL_TEXTURE0 + 8);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
//    glActiveTexture(GL_TEXTURE0 + 9);
//    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
//
//    // bind pre-computed area light LTC data
//    glActiveTexture(GL_TEXTURE0 + 20);
//    glBindTexture(GL_TEXTURE_2D, LTC1Map);
//    glActiveTexture(GL_TEXTURE0 + 21);
//    glBindTexture(GL_TEXTURE_2D, LTC2Map);
//
//
//    // update user stuffs
//    update(pbrShader);
//    update(outlineColorShader);
//
//
//
//    // render skybox (render as last to prevent overdraw)
//    backgroundShader.use();
//    backgroundShader.setMat4("view", view);
//    backgroundShader.setMat4("projection", projection);
//    backgroundShader.setFloat("blurStrength", settings.HDRSkyboxBlurStrength);
//
//    // Bind the cube map texture to texture unit 0
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
//    //glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); // display irradiance map
//    //glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap); // display prefilter map
//
//    if (!settings.HDRSkyboxHide)
//        renderCube();
//
//    // render BRDF map to screen
//    //brdfShader.use();
//    //renderQuad();
//
//    // compute light shadows using a depth map framebuffer
//    if (m_lights.size() > 0)
//    {
//        auto firstLight = m_lights[0];
//        if (std::dynamic_pointer_cast<PointLight>(firstLight))
//            computeDepthMapFramebuffer2(pbrShader, width, height, settings.enableShadows, (GLsizei)settings.shadowMapsTextureSize, update, firstLight);
//        else
//            computeDepthMapFramebuffer(pbrShader, width, height, settings.enableShadows, (GLsizei)settings.shadowMapsTextureSize, update, firstLight);
//    }
//
//    // render to framebuffer
//    //computeHDRColorFramebuffer(width, height);
//    computeColorFramebuffer();
//
//
//    // Resolve MSAA to screen or another texture FBO (SDR old)
//    glBindFramebuffer(GL_READ_FRAMEBUFFER, colorFramebuffer);
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Default framebuffer (screen)
//    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
//
//
//    // Resolve MSAA color to colorFramebuffer (HDR)
//    //glBindFramebuffer(GL_READ_FRAMEBUFFER, colorFramebuffer);
//    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolveFBO);
//    //glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
//    // Now colorFramebuffer contains the HDR image you can sample from
//
//    // display UI/HUD above the scene and outside the framebuffer
//    updateUI();
//}
//Copilot said :
//Copilot
//
//Raisonné pour 1s
//
//Spotting shader inconsistencies
//
//Pinpointing shader conflicts
//Thanks for sharing the loop—there are a few things that are likely to cause the GL_INVALID_OPERATION(502) with the debug text “program texture usage”, and a couple of architectural nits that can bite later.
//
//Below is a focused review and a concrete fix checklist.
//
//What looks risky in your loop
//1) Sampler → texture unit assignments are not visible
//You bind several textures to specific units :
//
//
//
//
//C++
//// IBL
//glActiveTexture(GL_TEXTURE0 + 7);  glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
//glActiveTexture(GL_TEXTURE0 + 8);  glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
//glActiveTexture(GL_TEXTURE0 + 9);  glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
//
//// LTC
//glActiveTexture(GL_TEXTURE0 + 20); glBindTexture(GL_TEXTURE_2D, LTC1Map);
//glActiveTexture(GL_TEXTURE0 + 21); glBindTexture(GL_TEXTURE_2D, LTC2Map);
//…but I don’t see where the sampler uniforms in pbrShader are set to those units after linking.If your GLSL doesn’t use layout(binding = X) uniform sampler..., then you must set :
//
//
//
//
//C++
//pbrShader.use();
//pbrShader.setInt("uIrradianceMap", 7);
//pbrShader.setInt("uPrefilterMap", 8);
//pbrShader.setInt("uBrdfLUT", 9);
//pbrShader.setInt("uLTC1", 20);
//pbrShader.setInt("uLTC2", 21);
//If this isn’t done(or was done before a relink), the program can see inconsistent sampler usage and trigger the exact error you’re seeing.This is one of the most common causes of that debug message.
//
//Tip: Prefer declaring fixed bindings in GLSL to avoid runtime mistakes :
//
//
//
//
//GLSL
//layout(binding = 7)  uniform samplerCube uIrradianceMap;
//layout(binding = 8)  uniform samplerCube uPrefilterMap;
//layout(binding = 9)  uniform sampler2D   uBrdfLUT;
//layout(binding = 20) uniform sampler2D   uLTC1;
//layout(binding = 21) uniform sampler2D   uLTC2;
//2) Possible texture unit conflicts across samplers within the same program
//If two samplers of different types in the same program end up assigned to the same texture unit, the draw will error(program texture usage).This can happen subtly if:
//
//update(pbrShader) reassigns sampler units without you realizing,
//some materials use sampler2D at unit 0 while another uniform uses samplerCube also at unit 0, or
//relinking changed uniform locations and your code didn’t re - assign them.
//Make sure every sampler uniform in each program is mapped to a unique texture unit within that program.
//
//3) Background(skybox) shader vs.PBR shader state ordering
//You bind the skybox cubemap at unit 0, then later continue with other work :
//
//
//
//
//C++
//backgroundShader.use();
//glActiveTexture(GL_TEXTURE0);
//glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
//if (!settings.HDRSkyboxHide)
//renderCube();
//This is okay as long as in the PBR shader your sampler2D(albedo, etc.) and samplerCube are not both assigned to unit 0 in the same program.Reusing unit 0 across different programs is fine, but within the same program it must not be shared by different sampler types.
//
//Also, ensure backgroundShader explicitly sets its sampler uniform to unit 0 (or use layout(binding = 0)), e.g.:
//
//
//
//
//C++
//backgroundShader.use();
//backgroundShader.setInt("uEnvironment", 0);
//4) Shadow / compare samplers requirements(if any shadow samplers in pbr)
//If pbrShader uses sampler2DShadow / samplerCubeShadow, the bound texture must :
//
//have a depth internal format(e.g., GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT32F), and
//GL_TEXTURE_COMPARE_MODE = GL_COMPARE_REF_TO_TEXTURE.
//If you bind a color texture to a shadow sampler, or forget COMPARE_MODE, you’ll get this error at draw time.
//
//5) Texture completeness and mipmaps
//Your IBL resources must be complete for their samplers / filters:
//
//Prefilter cubemap often uses mipmaps(for roughness levels).If minification filter uses mipmaps, ensure the cubemap has a complete mipmap chain.
//Cube map completeness : all six faces defined, consistent dimensions / formats.
//BRDF LUT usually is 2D, no mipmaps, linear filtering—fine.
//An incomplete texture bound to a sampler that is actually sampled → GL_INVALID_OPERATION.
//
//6) Enabling stencil but not configuring it
//You call :
//
//
//
//
//C++
//glEnable(GL_STENCIL_TEST);
//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//If you don’t set glStencilFunc / glStencilOp, you inherit previous state.That won’t cause the texture usage error, but it can cause surprising draw rejects later.Either configure the stencil ops or disable stencil if you don’t use it.
//
//7) Shadows computed after drawing the background
//You render the skybox before computing depth maps :
//
//
//
//
//C++
//if (m_lights.size() > 0) {
//    // computeDepthMapFramebuffer...
//}
//This is unusual : typically you compute light depth maps before the main PBR pass.However, since you render the skybox with a different program, it’s not the root cause of your error—just something to consider for correctness / efficiency.
//
//8) Multi - sample resolve
//You blit color only from colorFramebuffer to default framebuffer:
//
//
//
//
//C++
//glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
//    GL_COLOR_BUFFER_BIT, GL_NEAREST);
//If you expect depth - based post or UI depth testing later, you are not resolving depth / stencil.This isn’t related to the texture error, but worth noting.
//
//Concrete fix checklist
//Lock down sampler bindings(do this once per program, after linking)
//In your PBR init(constructor or after link) :
//
//
//
//
//    C++
//    pbrShader.use();
//pbrShader.setInt("uIrradianceMap", 7);
//pbrShader.setInt("uPrefilterMap", 8);
//pbrShader.setInt("uBrdfLUT", 9);
//pbrShader.setInt("uLTC1", 20);
//pbrShader.setInt("uLTC2", 21);
//
//// Also set all material textures to non-overlapping units within pbrShader:
//pbrShader.setInt("uAlbedo", 0);
//pbrShader.setInt("uNormal", 1);
//pbrShader.setInt("uMetallic", 2);
//pbrShader.setInt("uRoughness", 3);
//pbrShader.setInt("uAo", 4);
//// ... etc.
//
//Afficher plus de lignes
//In your background init :
//
//
//
//
//C++
//backgroundShader.use();
//backgroundShader.setInt("uEnvironment", 0);
//Even better : switch to GLSL layout(binding = N) in the shader and stop calling setInt entirely for samplers.
//
//Ensure uniqueness within each program Never let two sampler uniforms in the same program share the same unit if their types differ(e.g., sampler2D and samplerCube cannot both be unit 0 in pbrShader).
//
//Validate the program right before first draw Add this once when debugging :
//
//
//
//
//C++
//glValidateProgram(pbrShader.id());
//GLint ok = GL_FALSE;
//glGetProgramiv(pbrShader.id(), GL_VALIDATE_STATUS, &ok);
//if (!ok) {
//    GLint len = 0;
//    glGetProgramiv(pbrShader.id(), GL_INFO_LOG_LENGTH, &len);
//    std::string log(len, '\0');
//    glGetProgramInfoLog(pbrShader.id(), len, nullptr, log.data());
//    fprintf(stderr, "[PBR] Validate log:\n%s\n", log.c_str());
//}
//This often prints exactly which sampler is mismatched.
//
//Shadow sampler configuration(if applicable) If your PBR shader uses a shadow sampler :
//
//
//
//C++
//glBindTexture(GL_TEXTURE_2D, shadowTex);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
//// ensure internal format is GL_DEPTH_COMPONENT* and the texture is complete
//Completeness and mipmaps
//Prefilter cubemap : has all mip levels populated, min filter uses mipmaps(e.g., GL_LINEAR_MIPMAP_LINEAR).
//Irradiance cubemap : typically single mip level, but completeness rules still apply(all faces).
//BRDF LUT : 2D, no mipmaps is fine; set min filter to GL_LINEAR or GL_NEAREST.
//LTC textures : 2D tables; ensure dimensions / formats and filters are valid.
//Guard against update(pbrShader) surprises Make sure update(pbrShader) does not:
//Reassign sampler uniforms to different units,
//Set two sampler uniforms to the same unit,
//Switch an integer sampler to a float texture(or vice versa),
//Introduce a sampler2DShadow without proper compare mode and depth format.
//If it must, centralize unit assignments and keep a debug dump of current sampler→unit mappings right before the draw.
//
//Optional cleanups(not about the error, but good hygiene)
//Compute shadow maps before any main / skybox rendering, then render PBR, then skybox, then transparent / UI.
//Disable stencil unless actively used; or set:
//
//
//
//C++
//glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
//glStencilFunc(GL_ALWAYS, 0, 0xFF);
//If you need resolved depth for later passes, also blit GL_DEPTH_BUFFER_BIT to a depth attachment on a resolve FBO(or default depth if supported).
//A tiny diagnostic helper
//Right before your glDraw* for PBR, temporarily print the uniforms you care about :
//
//
//
//
//C++
//auto u = const char* n{
//    GLint loc = glGetUniformLocation(pbrShader.id(), n);
//    GLint val = -1;
//    if (loc >= 0) glGetUniformiv(pbrShader.id(), loc, &val);
//    printf("%s -> %d\n", n, val);
//};
//
//pbrShader.use();
//u("uAlbedo");
//u("uNormal");
//u("uMetallic");
//u("uRoughness");
//u("uAo");
//u("uIrradianceMap");
//u("uPrefilterMap");
//u("uBrdfLUT");
//u("uLTC1");
//u("uLTC2");
//
//Afficher plus de lignes
//If you see duplicates across different sampler types, that’s your smoking gun.
//
//If you can share the GLSL sampler declarations for pbrShader and backgroundShader, and where you assign their uniform units(or confirm you use layout(binding = ...)), I can pinpoint the exact mismatch causing the error.
//
//
//
//
//
//
//
//
//You said :
//Some stuffs are done in the setup() :
//    void engine::PbrRenderer::setup(int width, int height, std::shared_ptr<Camera> camera, const std::vector<std::shared_ptr<Light>>&lights)
//{
//    m_lights = lights; // copy ?????????
//    m_camera = camera; // copy ?????????
//
//    auto* singleton = engine::Singleton::getInstance();
//    assert(singleton != nullptr && "Singleton not initialized !");
//    const SceneSettings& settings = singleton->sceneSettings();
//
//    // configure global opengl state
//    // -----------------------------
//    // enable depth testing
//    enableDepthTest(true);
//    // set depth function to less than AND equal for skybox depth trick.
//    glDepthFunc(GL_LEQUAL);
//    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
//    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
//    // enable objects outlining
//    enableStencilTest(true);
//    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
//    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//    // avoid computing back faces not visible by camera
//    enableFaceCulling(settings.enableFaceCulling);
//    // automatic color correction
//    enableGammaCorrection(settings.enableGammaCorrection);
//
//    // build and compile shaders
//    // -------------------------
//    loadShaders();
//
//
//    // shader configuration
//    // --------------------
//    pbrShader.use();
//    pbrShader.setInt("material.texture_irradiance", 7); // Should be texture unit, not texture ID
//    pbrShader.setInt("material.texture_prefilter", 8); // Should be texture unit, not texture ID
//    pbrShader.setInt("material.texture_brdfLUT", 9); // Should be texture unit, not texture ID
//    pbrShader.setFloat("material.shadowIntensity", settings.shadowIntensity);
//    pbrShader.setInt("material.shadowCalculationMethod", settings.shadowCalculationMethod);
//    pbrShader.setFloat("material.shadowMapsBias", settings.shadowMapsBiasFactor);
//    pbrShader.setFloat("material.shadowMapsBlur", settings.shadowMapsBlur);
//    pbrShader.setFloat("material.iblDiffuseIntensity", settings.iblDiffuseIntensity); // [0.0, 2.0]
//    pbrShader.setFloat("material.iblSpecularIntensity", settings.iblSpecularIntensity); // [0.0, 5.0]
//
//    pbrShader.setInt("LTC1", 20); // Tell the shader to use texture unit 20 for LTC1
//    pbrShader.setInt("LTC2", 21); // Tell the shader to use texture unit 21 for LTC2
//
//
//    backgroundShader.use();
//    backgroundShader.setInt("environmentMap", 0); // Should be texture unit, not texture ID
//
//
//    screenShader.use();
//    screenShader.setInt("screenTexture", 0); // Should be texture unit, not texture ID
//
//    // Depth map framebuffer configuration (for shadow map)
//    // -----------------------------------
//    initDepthMapFramebuffer((GLsizei)settings.shadowMapsTextureSize);
//
//    // color framebuffer configuration
//    // -------------------------
//    //initHDRColorFramebufferMSAA(width, height); // HDR and AA
//    initColorFramebufferMSAA(width, height); // no HDR
//    //initColorFramebuffer(width, height); // no AA
//
//    // solid/wireframe polygons
//    glPolygonMode(GL_FRONT_AND_BACK, settings.drawAsWireframe ? GL_LINE : GL_FILL);
//
//
//    int vsize{ 512 };
//    //int scrWidth, scrHeight;
//    //glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
//    //float qualityFactor = 2.0f; // 200% of the screen resolution
//    //int vsize = static_cast<int>(std::max(scrWidth, scrHeight) * qualityFactor);
//
//
//    if (settings.showDebugGrid)
//        initDebugPlaneGrid();
//
//    // pbr: setup framebuffer
//    // ----------------------
//    unsigned int captureFBO{};
//    unsigned int captureRBO{};
//    glGenFramebuffers(1, &captureFBO);
//    glGenRenderbuffers(1, &captureRBO);
//
//    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
//    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, vsize, vsize);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
//
//    // pbr: load the HDR environment map
//    // ---------------------------------
//    unsigned int hdrTexture = !settings.HDRSkyboxFilePath.empty() ? engine::Texture::loadHDRImage(FileSystem::getPath(settings.HDRSkyboxFilePath)) : 0;
//
//    // pbr: setup cubemap to render to and attach to framebuffer
//    // ---------------------------------------------------------
//    glGenTextures(1, &envCubemap);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
//    for (unsigned int i = 0; i < 6; ++i)
//    {
//        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, vsize, vsize, 0, GL_RGB, GL_FLOAT, nullptr);
//    }
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
//
//    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
//    // ----------------------------------------------------------------------------------------------
//    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
//    glm::mat4 captureViews[] =
//    {
//        glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)), // +X (right)
//        glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)), // -X (left)
//        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)), // +Y (top)
//        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)), // -Y (bottom)
//        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)), // +Z (front)
//        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)) // -Z (back)
//    };
//
//
//
//
//
//
//    // pbr: convert HDR equirectangular environment map to cubemap equivalent
//    // ----------------------------------------------------------------------
//    equirectangularToCubemapShader.use();
//    equirectangularToCubemapShader.setInt("equirectangularMap", 0);
//    equirectangularToCubemapShader.setMat4("projection", captureProjection);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, hdrTexture);
//
//    glViewport(0, 0, vsize, vsize); // don't forget to configure the viewport to the capture dimensions.
//    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
//
//    for (unsigned int i = 0; i < 6; ++i)
//    {
//        equirectangularToCubemapShader.setMat4("view", captureViews[i]);
//        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        renderCube();
//    }
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//    // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
//    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
//    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
//
//
//
//
//    // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
//    // --------------------------------------------------------------------------------
//    glGenTextures(1, &irradianceMap);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
//    for (unsigned int i = 0; i < 6; ++i)
//    {
//        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
//    }
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
//    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
//
//    // pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
//    // -----------------------------------------------------------------------------
//    irradianceShader.use();
//    irradianceShader.setInt("environmentMap", 0);
//    irradianceShader.setMat4("projection", captureProjection);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
//
//    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
//    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
//    for (unsigned int i = 0; i < 6; ++i)
//    {
//        irradianceShader.setMat4("view", captureViews[i]);
//        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        renderCube();
//    }
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//    // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
//    // --------------------------------------------------------------------------------
//    glGenTextures(1, &prefilterMap);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
//    for (unsigned int i = 0; i < 6; ++i)
//    {
//        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
//    }
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
//    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
//
//    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
//    // ----------------------------------------------------------------------------------------------------
//    prefilterShader.use();
//    prefilterShader.setInt("environmentMap", 0);
//    prefilterShader.setMat4("projection", captureProjection);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
//
//    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
//    unsigned int maxMipLevels{ 5 };
//    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
//    {
//        // resize framebuffer according to mip-level size.
//        unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
//        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
//        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
//        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
//        glViewport(0, 0, mipWidth, mipHeight);
//
//        float roughness = (float)mip / (float)(maxMipLevels - 1);
//        prefilterShader.setFloat("roughness", roughness);
//        for (unsigned int i = 0; i < 6; ++i)
//        {
//            prefilterShader.setMat4("view", captureViews[i]);
//            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            renderCube();
//        }
//    }
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//    // pbr: generate a 2D LUT from the BRDF equations used.
//    // ----------------------------------------------------
//    glGenTextures(1, &brdfLUTTexture);
//    // pre-allocate enough memory for the LUT texture.
//    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, vsize, vsize, 0, GL_RG, GL_FLOAT, 0);
//    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
//    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
//    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, vsize, vsize);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);
//
//    glViewport(0, 0, vsize, vsize);
//    brdfShader.use();
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    renderQuad();
//
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//    // initialize static shader uniforms before rendering
//    // --------------------------------------------------
//    glm::mat4 projection = m_camera->getProjectionMatrix(width, height, 0.1f, 100.0f);
//
//    pbrShader.use();
//    pbrShader.setMat4("projection", projection);
//
//    LTC1Map = Texture::loadMTexture();
//    LTC2Map = Texture::loadLUTTexture();
//    pbrShader.setInt("LTC1", 20); // Should be texture unit, not texture ID
//    pbrShader.setInt("LTC2", 21); // Should be texture unit, not texture ID
//
//    backgroundShader.use();
//    backgroundShader.setMat4("projection", projection);
//
//    // then before rendering, configure the viewport to the original framebuffer's screen dimensions
//    int scrWidth{}, scrHeight{};
//    glfwGetFramebufferSize(m_window, &scrWidth, &scrHeight);
//    glViewport(0, 0, scrWidth, scrHeight);
//}
//Copilot said :
//Copilot
//
//Raisonné pour 1s
//
//Revising shader settings
//
//Addressing potential issues
//Great—thanks for posting your setup()!That fills in most of the gaps.Overall, your sampler → texture - unit mapping looks mostly correct and consistent with what you bind in loop().However, there are a few real gotchas here that can directly trigger the “program texture usage” error at draw time, plus a couple of lifecycle / state issues that could bite you intermittently.
//
//Below is a tight review tailored to your code, then a ready‑to‑apply checklist.
//
//✅ What’s good / consistent
//You assign sampler uniforms after linking in setup() :
//
//
//
//
//    C++
//    pbrShader.setInt("material.texture_irradiance", 7);
//pbrShader.setInt("material.texture_prefilter", 8);
//pbrShader.setInt("material.texture_brdfLUT", 9);
//pbrShader.setInt("LTC1", 20);
//pbrShader.setInt("LTC2", 21);
//
//backgroundShader.setInt("environmentMap", 0);
//…and in loop() you bind to those exact units(7, 8, 9, 20, 21, and 0).That’s good.
//
//Irradiance / prefilter maps are cube maps, BRDF / LTC are 2D—this matches the typical sampler types(samplerCube vs sampler2D).
//
//Prefilter cubemap uses GL_LINEAR_MIPMAP_LINEAR and you generate mipmaps—important for completeness.
//
//⚠️ Risks that can cause GL_INVALID_OPERATION(program texture usage)
//1) Using texture units 20 / 21 without checking device limits
//Many drivers expose plenty of units, but the spec only guarantees 16 for fragment shaders(and 48 combined across stages in older GL versions).If your GPU exposes, say, only 16 fragment units, mapping samplers to 20 or 21 is illegal and can produce the exact error you’re seeing when the program is used.
//
//Fix: query and validate at startup, and remap if needed :
//
//
//
//
//    C++
//    GLint maxFragUnits = 0, maxCombinedUnits = 0;
//glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxFragUnits);
//glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedUnits);
//
//assert(maxFragUnits >= 16); // many PCs have 16+, but don't assume 22+
//auto requireUnit = int unit{
//    if (unit >= maxFragUnits) {
//        fprintf(stderr, "ERROR: Texture unit %d exceeds GL_MAX_TEXTURE_IMAGE_UNITS=%d\n", unit, maxFragUnits);
//        // Remap to a safe unit or abort.
//    }
//};
//
//// Check your chosen units:
//requireUnit(0);
//requireUnit(7);
//requireUnit(8);
//requireUnit(9);
//requireUnit(20);
//requireUnit(21);
//
//Afficher plus de lignes
//If this check fails, move LTC to, e.g., 10 and 11, and update both the setInt and the glActiveTexture bindings.
//
//This is a very common reason for “program texture usage” when using high - numbered units.
//
//2) Sampler type ↔ target mismatch(verify your GLSL)
//Your binding pattern assumes the GLSL has :
//
//material.texture_irradiance → samplerCube
//material.texture_prefilter → samplerCube
//material.texture_brdfLUT → sampler2D
//LTC1 / LTC2 → sampler2D
//If any of these are declared differently(e.g., sampler2D for irradiance or prefilter), the draw will error.Double‑check the GLSL declarations.
//
//3) Shadow samplers(if used in pbrShader) must bind depth textures with compare mode
//If your PBR shader uses sampler2DShadow / samplerCubeShadow, the bound textures must :
//
//have an internal format of GL_DEPTH_COMPONENT* (or depth cube for point light),
//have GL_TEXTURE_COMPARE_MODE = GL_COMPARE_REF_TO_TEXTURE.
//Binding a color texture to a shadow sampler is invalid.Make sure your shadow map code(not shown here) sets compare mode when needed.
//
//4) State churn during shadow pass& update callbacks
//You call update(pbrShader); and later run shadow framebuffer passes(computeDepthMapFramebuffer*).If those change sampler uniforms or texture unit bindings used by pbrShader(e.g., rebinding unit 7 with a 2D texture), you’ll hit this error when pbrShader is used.It’s fine to change bindings—as long as the target matches the sampler type and units don’t collide across sampler types in the same program.
//
//5) Stencil / Depth state : less about the error, but still important
//You set glDepthFunc(GL_LEQUAL) globally in setup() (for skybox trick).In loop() you draw PBR before skybox while still using LEQUAL.That can produce depth fighting or unexpected acceptance of fragments equal to existing depth.Typical pattern :
//
//
//
//
//C++
//// Scene
//glDepthFunc(GL_LESS);
//drawPBR();
//
//// Skybox
//glDepthFunc(GL_LEQUAL);
//drawSkybox();
//
//// Optional: restore
//glDepthFunc(GL_LESS);
//Not the cause of the texture error, but worth correcting.
//
//You enable stencil, set:
//
//
//
//
//C++
//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//Make sure your PBR pass writes the stencil ref(e.g., glStencilFunc(GL_ALWAYS, 1, 0xFF) + glStencilOp) when drawing objects, and switch to NOTEQUAL only during outline pass.Otherwise, you might mask out draws unexpectedly.
//
//🔎 Quick diagnostics to pinpoint the mismatch
//Add this once in debug to catch inconsistent state :
//
//
//
//
//C++
//auto validate = GLuint prog, const char* tag{
//    glValidateProgram(prog);
//    GLint ok = GL_FALSE;
//    glGetProgramiv(prog, GL_VALIDATE_STATUS, &ok);
//    if (!ok) {
//        GLint len = 0;
//        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
//        std::string log(len, '\0');
//        glGetProgramInfoLog(prog, len, nullptr, log.data());
//        fprintf(stderr, "[%s] Validate log:\n%s\n", tag, log.c_str());
//    }
//};
//validate(pbrShader.id(), "PBR");
//validate(backgroundShader.id(), "BG");
//
//Afficher plus de lignes
//And right before the PBR draw, dump the sampler→unit map :
//
//
//
//
//C++
//auto dumpUnit = GLuint prog, const char* name{
//    GLint loc = glGetUniformLocation(prog, name);
//    if (loc >= 0) {
//        GLint u = -1;
//        glGetUniformiv(prog, loc, &u);
//        fprintf(stderr, "%s -> %d\n", name, u);
//    }
//};
//
//GLuint p = pbrShader.id();
//dumpUnit(p, "material.texture_irradiance");
//dumpUnit(p, "material.texture_prefilter");
//dumpUnit(p, "material.texture_brdfLUT");
//dumpUnit(p, "LTC1");
//dumpUnit(p, "LTC2");
//
//Afficher plus de lignes
//Look for duplicate units across different sampler types inside pbrShader or units exceeding GL_MAX_TEXTURE_IMAGE_UNITS.
//
//🧰 Concrete, minimal changes I recommend
//Clamp / validate texture units and move LTC if required:
//
//
//
//C++
//// after creating GL context
//GLint maxFragUnits = 0;
//glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxFragUnits);
//
//// Use safer units if necessary
//int U_IRR = 7, U_PREF = 8, U_BRDF = 9, U_LTC1 = 10, U_LTC2 = 11;
//if (U_LTC2 >= maxFragUnits) {
//    // fall back to lower units and keep them unique
//    U_IRR = 1; U_PREF = 2; U_BRDF = 3; U_LTC1 = 4; U_LTC2 = 5;
//}
//
//pbrShader.use();
//pbrShader.setInt("material.texture_irradiance", U_IRR);
//pbrShader.setInt("material.texture_prefilter", U_PREF);
//pbrShader.setInt("material.texture_brdfLUT", U_BRDF);
//pbrShader.setInt("LTC1", U_LTC1);
//pbrShader.setInt("LTC2", U_LTC2);
//
//Afficher plus de lignes
//And bind with glActiveTexture(GL_TEXTURE0 + U_*) accordingly in loop().
//
//Double - check GLSL sampler types match the targets you bind :
//material.texture_irradiance → samplerCube
//material.texture_prefilter → samplerCube
//material.texture_brdfLUT → sampler2D
//LTC1, LTC2 → sampler2D
//Depth func : switch for skybox only.
//
//
//
//C++
//// Before PBR
//glDepthFunc(GL_LESS);
//drawPBR();
//
//// Skybox
//glDepthFunc(GL_LEQUAL);
//drawSkybox();
//
//glDepthFunc(GL_LESS); // optional restore
//Shadow maps : if using shadow samplers, ensure COMPARE_MODE is set on the shadow textures and the internal formats are depth.
//
//Guard your update(pbrShader) so it doesn’t reassign sampler uniforms or bind incompatible targets to used units.
//
//Why this addresses your specific error
//The KHR_debug text “State(s) are invalid : program texture usage” is emitted by drivers when the active program’s sampler state is inconsistent with the currently bound textures—most commonly :
//
//A sampler points to a texture unit beyond device limits(very likely with units 20 / 21 on some systems).
//A samplerCube points to a unit where a 2D target is bound(or vice versa).
//A shadow sampler points to a color texture or a depth texture without compare mode.
//Two samplers of different types share the same unit in the same program.
//The checks above remove all of these classes of problems.
//
//If you can paste the GLSL uniform declarations for:
//
//material.texture_irradiance
//material.texture_prefilter
//material.texture_brdfLUT
//LTC1, LTC2 …and confirm the GL_MAX_TEXTURE_IMAGE_UNITS on your target machine, I can give you an exact mapping that’s guaranteed to be valid everywhere.
//
//
//
//
//
//
//
//You said :
//What do you mean with "after linking" ?
//Copilot said :
//Copilot
//Great question — this is one of the most important OpenGL rules, and most mysterious bugs(including your GL_INVALID_OPERATION : program texture usage) come from misunderstanding it.
//
//Let’s make it crystal clear.
//
//✅ What “after linking” means in OpenGL
//Every GLSL program goes through these phases :
//
//Attach shaders
//
//
//
//
//C++
//glAttachShader(program, vs);
//glAttachShader(program, fs);
//Compile GLSL shaders
//
//
//
//
//C++
//glCompileShader(vs);
//glCompileShader(fs);
//Link the program
//
//
//
//
//C++
//glLinkProgram(program);
//Only after link succeeds do you set sampler uniforms :
//
//
//
//
//C++
//glUseProgram(program);
//glUniform1i(locationOfSampler, textureUnit);
//❗ Why sampler uniforms MUST be set after linking
//During the linking step, the driver :
//
//Optimizes,
//Removes unused uniforms,
//Reassigns locations,
//Changes internal texture slot mappings.
//So sampler uniform locations are NOT valid until glLinkProgram() finishes.
//
//If you call :
//
//
//
//
//C++
//pbrShader.setInt("material.texture_irradiance", 7);
//❌ before linking, the driver will silently ignore it(or write to the wrong uniform).
//Later, the shader tries to sample from a unit the driver didn't assign, leading to:
//
//GL_INVALID_OPERATION: program texture usage
//This is exactly the error you saw.
//
//✔ Correct lifecycle(GOOD)
//
//
//
//C++
//GLuint program = glCreateProgram();
//
//// compile
//glCompileShader(vs);
//glCompileShader(fs);
//
//// attach
//glAttachShader(program, vs);
//glAttachShader(program, fs);
//
//// link
//glLinkProgram(program);
//
//// NOW uniforms exist
//glUseProgram(program);
//glUniform1i(glGetUniformLocation(program, "envMap"), 0);   // OK
//glUniform1i(glGetUniformLocation(program, "irradiance"), 7); // OK
//
//Afficher plus de lignes
//❌ Incorrect lifecycle(BAD)
//This fails silently and causes 502 errors later :
//
//
//
//
//C++
//glUniform1i(...);  // WRONG: called before glLinkProgram
//glLinkProgram(program);
//🎯 In your code — are you doing it correctly ?
//Yes — your sampler assignments in setup() are correct, because they happen after you call :
//
//
//
//
//C++
//loadShaders();   // this should compile + link
//BUT only if loadShaders() actually links the shader programs before your setInt() calls.
//
//If loadShaders() only compiles but does NOT link, then:
//
//
//
//
//C++
//pbrShader.use();
//pbrShader.setInt("material.texture_irradiance", 7);
//was done before linking → uniforms are invalid → sampler points to unit 0 → conflicting sampler types at unit 0 → GL_INVALID_OPERATION at draw.
//
//🔍 Quick way to confirm
//Add this after loadShaders() :
//
//
//
//
//    C++
//    pbrShader.linkStatus();
//If your shader wrapper does not expose it, do:
//
//
//
//
//C++
//GLint linked = 0;
//glGetProgramiv(pbrShader.ID, GL_LINK_STATUS, &linked);
//printf("PBR linked? %d\n", linked);
//If this prints 0, you found your bug.
//
//🧠 Summary(simple definition)
//“After linking” = after the call to glLinkProgram, because only then the shader uniforms are valid, assigned, and ready to receive sampler unit bindings.
//
//If you want, you can paste your loadShaders() implementation
//→ I can confirm whether you link early enough or whether sampler uniforms need repositioning.
//
//
//
//
//
//
//
//
//You said :
//Where you i test the device limit ?
//Copilot said :
//Copilot
//
//Raisonné pour 1s
//
//Navigating texture limits
//Great question—test(query) device limits right after you create the OpenGL context and before you assign any sampler → texture - unit bindings(i.e., early in setup() after loadShaders() or even before loading shaders if you prefer).The limits are per - context, so you must query them after the context is current.
//
//Below is a drop‑in snippet you can paste into your setup() (or a small queryCaps() you call from there) to check the relevant limits and log them.
//
//Where to put it
//Right after your GL context is created and made current(e.g., right after creating the GLFW window and before / around your loadShaders() and sampler setInt() calls).
//In your code, a good place is at the top of setup(), before assigning sampler units(7, 8, 9, 20, 21).
//What to query(most relevant for your issue)
//GL_MAX_TEXTURE_IMAGE_UNITS → Fragment shader texture units(this is the one that will break if you try to use 20 / 21 on a device that only has 16).
//GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS → Total across all stages combined(vertex + fragment + others).
//(Optional)Per - stage limits for other stages : GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, etc.
//Minimal code to query& validate
//
//
//
//C++
//// Call this after the OpenGL context is current (e.g., after creating the window)
//void QueryAndValidateTextureUnitLimits()
//{
//    GLint maxFragUnits = 0;
//    GLint maxCombUnits = 0;
//    GLint maxVertUnits = 0;
//
//    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxFragUnits);           // Fragment stage
//    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombUnits);  // All stages combined
//    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxVertUnits);    // Vertex stage
//
//    fprintf(stderr, "[GL CAPS] Fragment units: %d, Vertex units: %d, Combined units: %d\n",
//        maxFragUnits, maxVertUnits, maxCombUnits);
//
//    // If you plan to use specific units, validate them here.
//    // Example with your current choices:
//    const int U_ENV_BG = 0;
//    const int U_IRRADIANCE = 7;
//    const int U_PREFILTER = 8;
//    const int U_BRDF = 9;
//    const int U_LTC1 = 20;
//    const int U_LTC2 = 21;
//
//    auto requireFragUnit = int unit, const char* name{
//        if (unit < 0 || unit >= maxFragUnits) {
//            fprintf(stderr, "ERROR: Texture unit %d for '%s' exceeds GL_MAX_TEXTURE_IMAGE_UNITS=%d\n",
//                    unit, name, maxFragUnits);
//            // You can assert in debug:
//            // assert(false && "Texture unit exceeds device limit");
//        }
//    };
//
//    requireFragUnit(U_ENV_BG, "environmentMap");
//    requireFragUnit(U_IRRADIANCE, "material.texture_irradiance");
//    requireFragUnit(U_PREFILTER, "material.texture_prefilter");
//    requireFragUnit(U_BRDF, "material.texture_brdfLUT");
//    requireFragUnit(U_LTC1, "LTC1");
//    requireFragUnit(U_LTC2, "LTC2");
//}
//
//Afficher plus de lignes
//Call it in your setup() :
//
//
//
//
//    C++
//    void engine::PbrRenderer::setup(int width, int height, std::shared_ptr<Camera> camera,
//        const std::vector<std::shared_ptr<Light>>&lights)
//{
//    // ...window/context already created and current...
//    QueryAndValidateTextureUnitLimits();
//
//    // then load/link shaders, and set sampler uniforms
//    loadShaders();
//    // pbrShader.use(); pbrShader.setInt(...); etc.
//}
//Auto‑remap to safe units(optional helper)
//If you want the renderer to adapt automatically on devices with fewer units, you can pick units from a pool[0 ..maxFragUnits - 1] and assign them consistently :
//
//
//
//
//C++
//struct SamplerBindings {
//    int envBG;
//    int irr;
//    int pref;
//    int brdf;
//    int ltc1;
//    int ltc2;
//};
//
//SamplerBindings MakeSafeBindings()
//{
//    GLint maxFragUnits = 0;
//    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxFragUnits);
//
//    // Reserve a small set of consecutive units (use what you need)
//    // Keep them unique within the same program.
//    // Fall back to lower indices if high ones aren't available.
//    int next = 0;
//    auto alloc = const char* name{
//        if (next >= maxFragUnits) {
//            fprintf(stderr, "ERROR: Not enough fragment texture units for '%s'\n", name);
//            // Handle error: reduce features or abort in debug
//        }
//        return next++;
//    };
//
//    SamplerBindings b{};
//    b.envBG = alloc("environmentMap");       // background shader
//    b.irr = alloc("material.texture_irradiance");
//    b.pref = alloc("material.texture_prefilter");
//    b.brdf = alloc("material.texture_brdfLUT");
//    b.ltc1 = alloc("LTC1");
//    b.ltc2 = alloc("LTC2");
//    return b;
//}
//
//Afficher plus de lignes
//Then use b.*when calling setInt and glActiveTexture(GL_TEXTURE0 + b.*).
//
//Don’t forget : validate after linking
//When you’re done assigning sampler uniforms, it’s very helpful to validate :
//
//
//
//
//C++
//void ValidateProgram(GLuint prog, const char* tag)
//{
//    glValidateProgram(prog);
//    GLint ok = GL_FALSE;
//    glGetProgramiv(prog, GL_VALIDATE_STATUS, &ok);
//    if (!ok) {
//        GLint len = 0;
//        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
//        std::string log(len, '\0');
//        glGetProgramInfoLog(prog, len, nullptr, log.data());
//        fprintf(stderr, "[%s] Validate log:\n%s\n", tag, log.c_str());
//    }
//}
//Call this once after setting all sampler uniforms for pbrShader and backgroundShader.If there’s a mismatch(e.g., unit out of range, wrong target), the log often points you straight to it.
//
//TL; DR
//Where ? Right after the GL context is current—put the limit query at the top of setup().
//What ? Query GL_MAX_TEXTURE_IMAGE_UNITS(fragment) and ensure your chosen units(e.g., 20 / 21) are < max.
//    Then assign sampler uniforms(after linking), and bind textures to those units in your draw loop.
//    If you want, paste your loadShaders() (so I can confirm it links before you set uniforms) and I’ll show exactly where to insert these checks in your file.
//
//
//
//
//
//
//
//
//    You said :
//Is this correct ? LTC1Map = Texture::loadMTexture();
//LTC2Map = Texture::loadLUTTexture();
//pbrShader.setInt("LTC1", 20); // Should be texture unit, not texture ID
//pbrShader.setInt("LTC2", 21); // Should be texture unit, not texture ID
//Copilot said :
//Copilot
//Short answer :
//YES, it is correct only if you also bind those textures to texture units 20 and 21 before using pbrShader in the draw loop.
//If you forget the glActiveTexture + glBindTexture part, then it is not correct.
//
//Let me explain clearly.
//
//✅ What your code does correctly
//
//
//
//C++
//pbrShader.setInt("LTC1", 20);  // sampler uniform → texture unit index
//pbrShader.setInt("LTC2", 21);
//This is correct because :
//
//setInt("LTC1", 20) sets the sampler uniform,
//NOT the texture ID,
//telling the shader :
//“When sampling LTC1, fetch from texture unit 20.”
//This is exactly how samplers work.
//
//❗ But it is only correct if you also bind the textures correctly
//You must do this in your render loop, BEFORE any draw call using pbrShader :
//
//
//
//
//    C++
//    glActiveTexture(GL_TEXTURE0 + 20);
//glBindTexture(GL_TEXTURE_2D, LTC1Map);
//
//glActiveTexture(GL_TEXTURE0 + 21);
//glBindTexture(GL_TEXTURE_2D, LTC2Map);
//If you do not do this, then:
//
//pbrShader expects textures on units 20 and 21,
//but those units contain either garbage or nothing,
//and the draw call will produce :
//GL_INVALID_OPERATION: program texture usage
//This matches the error you reported earlier.
//
//❗ Make sure these units exist on the GPU
//Before using texture units 20 and 21, you must check :
//
//
//
//
//C++
//GLint maxUnits = 0;
//glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxUnits);
//printf("Fragment shader texture units = %d\n", maxUnits);
//If maxUnits <= 21, then using unit 21 is illegal, and OpenGL will throw 502 errors on draw.
//
//Many Intel iGPUs have only 16 units → this is a likely cause of your bug.
//
//❗ Confirm sampler types match texture targets
//To be fully correct :
//
//LTC1 and LTC2 must be defined as sampler2D in GLSL.
//loadMTexture() and loadLUTTexture() must produce 2D textures, not cube maps.
//If GLSL thinks this :
//
//
//
//
//    GLSL
//    uniform sampler2D LTC1;
//…but you bind a cube map to it → invalid operation.
//
//🔥 Final checklist
//Condition	Required ? Why
//setInt("LTC1", 20) done after linking	✔	Uniform location must exist
//glActiveTexture(GL_TEXTURE0 + 20) + glBindTexture(GL_TEXTURE_2D, LTC1Map)	✔	Shader expects unit 20
//GL_MAX_TEXTURE_IMAGE_UNITS > 21	✔	Otherwise unit 21 is out of range
//GLSL uniform type = sampler2D	✔	You bind 2D textures
//LTC textures valid and complete	✔	Incomplete textures → invalid operation
//If ANY of these is wrong → you get the exact error you’re seeing.
//
//If you want, paste your GLSL for the LTC uniforms
//I can check in 5 seconds whether the sampler types match your bindings.
//
//
//
//
//
//
//
//
//You said :
//This should be done in setup(one time) or in the loop ? pbrShader.use();
//pbrShader.setMat4("projection", projection);
//
//LTC1Map = Texture::loadMTexture();
//LTC2Map = Texture::loadLUTTexture();
//
//pbrShader.setInt("LTC1", 20); // Should be texture unit, not texture ID
//glBindTexture(GL_TEXTURE_2D, LTC1Map);
//
//pbrShader.setInt("LTC2", 21); // Should be texture unit, not texture ID
//glBindTexture(GL_TEXTURE_2D, LTC2Map);
//Copilot said :
//Copilot
//
//Raisonné pour 1s
//
//working...
//
//working...
//Great question.Think of it this way:
//
//One - time(in setup() after linking) :
//
//    Load / create textures.
//    Decide which texture units you’ll use(and verify limits).
//    Assign sampler uniforms → texture units(setInt on the shader).
//    Per - frame / before each draw that uses the program :
//
//Bind the actual textures to those units(glActiveTexture + glBindTexture) right before drawing with that shader(or at least before the first draw in the frame that relies on them).
//Why ? Because :
//
//    The sampler uniform is a property of the program(you set it once after glLinkProgram).
//    The texture binding is global state per texture unit and can be changed by other passes / shaders.To be safe, bind what you need before you draw.
//    Also, your snippet is missing glActiveTexture(...) before each glBindTexture(...).Without it, you’ll bind to whichever unit happens to be active(usually GL_TEXTURE0), which will cause the “program texture usage” error if the sampler expects a different unit.
//
//    What to put in setup() (once)
//
//
//
//    C++
//    // 1) After creating GL context and making it current
//    GLint maxFragUnits = 0;
//glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxFragUnits);
//// Choose safe units (don’t assume 20/21 exist)
//const GLint U_LTC1 = 10;
//const GLint U_LTC2 = 11;
//assert(U_LTC2 < maxFragUnits);
//
//// 2) Load textures
//LTC1Map = Texture::loadMTexture();     // must be a 2D texture
//LTC2Map = Texture::loadLUTTexture();   // must be a 2D texture
//
//// 3) After you link pbrShader:
//pbrShader.use();
//pbrShader.setInt("LTC1", U_LTC1);  // sampler uniform -> unit index, not texture ID
//pbrShader.setInt("LTC2", U_LTC2);
//
//// (Do the same "setInt" for all other samplers, e.g., IBL, BRDF LUT, etc.)
//
//Afficher plus de lignes
//Reminder : “after linking” means after you call glLinkProgram() for pbrShader.Uniform locations and existence are only valid then.
//
//What to do in the render loop(every frame, before draw)
//
//
//
//C++
//pbrShader.use();
//
//// Bind LTC textures to the units you told the shader to use
//glActiveTexture(GL_TEXTURE0 + U_LTC1);
//glBindTexture(GL_TEXTURE_2D, LTC1Map);
//
//glActiveTexture(GL_TEXTURE0 + U_LTC2);
//glBindTexture(GL_TEXTURE_2D, LTC2Map);
//
//// ...bind your other textures too (irradiance, prefilter, BRDF, material maps, etc.)
//
//// Set per-frame uniforms (projection/view can change on resize/camera move)
//pbrShader.setMat4("projection", projection);
//pbrShader.setMat4("view", view);
//pbrShader.setVec3("viewPos", camera->position);
//
//// draw...
//
//Afficher plus de lignes
//If you never change those texture units anywhere else in your engine, you could bind once and leave them—but in practice other passes often rebind units(postprocess, UI, other materials), so the robust pattern is to(re)bind right before drawing the objects that need them.
//
//Important details specific to your snippet
//You wrote :
//
//
//
//
//C++
//pbrShader.setInt("LTC1", 20);
//glBindTexture(GL_TEXTURE_2D, LTC1Map);
//Two issues :
//
//Missing glActiveTexture(GL_TEXTURE0 + 20) — without it, you bind on whichever unit is active(usually unit 0), not 20.
//This belongs in the loop(before draw), not only in setup.Do setInt in setup; do glActiveTexture / glBindTexture before drawing with pbrShader.
//Quick checklist
//Check GL_MAX_TEXTURE_IMAGE_UNITS and pick units < max.
//    Load / create textures in setup().
//    After linking, call setInt("LTC1", U_LTC1) / setInt("LTC2", U_LTC2) in setup().
//    In the loop, before drawing with pbrShader :
//glActiveTexture(GL_TEXTURE0 + U_LTC1); glBindTexture(GL_TEXTURE_2D, LTC1Map);
//glActiveTexture(GL_TEXTURE0 + U_LTC2); glBindTexture(GL_TEXTURE_2D, LTC2Map);
//Ensure GLSL declares uniform sampler2D LTC1; and uniform sampler2D LTC2;
//If you use shadow samplers, bind depth textures with COMPARE_MODE.
//If you want, paste your LTC uniform declarations(GLSL) and the place where you bind IBL / BRDF textures in the loop; I can give you a “known - good” block you can drop in as your per - frame bindings.
//
//
//
//
//
//
//
//
//You said :
//Should i do glActiveTexture(GL_TEXTURE0); after drawing ?
//Copilot said :
//Copilot
//Short answer : No, you do not need to call glActiveTexture(GL_TEXTURE0) after drawing.
//And in most engines, you should NOT do it.
//
//Let me explain clearly why.
//
//✅ Why you don’t need to reset active texture state
//glActiveTexture(...) just sets which texture unit subsequent glBindTexture(...) calls will affect.
//It is not a piece of state that must be restored after drawing(unlike depth func or blending).
//
//OpenGL does not require(and does not recommend) “resetting” the active texture unit.
//
//From a driver point of view :
//
//Leaving the active unit at whatever you last used is harmless.
//Every shader draw should explicitly rebind the textures it needs anyway.
//Resetting it is useless, does not improve clarity, and wastes CPU draw time.
//🔥 What CAN cause problems ?
//Not resetting the active texture unit is fine…
//
//…BUT only if you always do this before binding a texture :
//
//
//
//
//C++
//glActiveTexture(GL_TEXTURE0 + UNIT);
//glBindTexture(GL_TEXTURE_2D, texture);
//If you bind a texture without explicitly calling glActiveTexture, THEN you will bind to whatever unit was last used… and that can break things.
//
//So the rule is :
//
//Before every texture bind(glBindTexture), explicitly choose the texture unit.
//
//If you follow that rule, you never have to “reset” anything.
//
//🧠 Example : Correct approach(no reset needed)
//Setup(once) :
//
//
//
//
//    C++
//    pbrShader.use();
//pbrShader.setInt("LTC1", 20);
//pbrShader.setInt("LTC2", 21);
//Loop(every frame) :
//
//
//
//
//    C++
//    pbrShader.use();
//
//// Bind LTC1
//glActiveTexture(GL_TEXTURE0 + 20);
//glBindTexture(GL_TEXTURE_2D, LTC1Map);
//
//// Bind LTC2
//glActiveTexture(GL_TEXTURE0 + 21);
//glBindTexture(GL_TEXTURE_2D, LTC2Map);
//
//// Bind IBL textures
//glActiveTexture(GL_TEXTURE0 + 7);
//glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
//
//glActiveTexture(GL_TEXTURE0 + 8);
//glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
//
//glActiveTexture(GL_TEXTURE0 + 9);
//glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
//
//// ...draw...
//renderScene();
//
//Afficher plus de lignes
//No need for:
//
//
//
//
//C++
//// This is useless and not needed
//glActiveTexture(GL_TEXTURE0);
//🚫 Wrong idea : Resetting active unit
//This is not required and can hide bugs rather than prevent them :
//
//
//
//
//C++
//glActiveTexture(GL_TEXTURE31);
//glBindTexture(...);
//
//// ❌ No benefit at all
//glActiveTexture(GL_TEXTURE0);
//If another part of your engine forgets to call glActiveTexture, it will silently break regardless of what you “reset”.
//
//So the correct pattern is :
//
//👉 Bind textures responsibly before drawing
//NOT
//👉 “Reset state after drawing”
//
//💥 When should you explicitly set GL_TEXTURE0 ?
//There is only one legitimate reason :
//
//✔ If your UI system, text renderer, or another library assumes unit 0 must be active when it starts binding textures
//Some libraries do this (Dear ImGui sometimes does).
//In that case, they usually call :
//
//
//
//
//C++
//glActiveTexture(GL_TEXTURE0);
//themselves before binding.
//
//You shouldn’t have to.
//
//🧪 Easy test to confirm
//If you want to confirm behavior, you can print the current active texture unit :
//
//
//
//
//C++
//GLint active;
//glGetIntegerv(GL_ACTIVE_TEXTURE, &active);
//printf("Current unit = %d\n", active - GL_TEXTURE0);
//You’ll see it stays at the last one you activated.
//
//Again: this is harmless.
//
//🎯 Final Answer
//❌ No, you do NOT need to call glActiveTexture(GL_TEXTURE0) after drawing.
//✔ You DO need to call glActiveTexture(GL_TEXTURE0 + UNIT) before every texture bind.
//✔ You SHOULD bind textures every frame before using the shader that samples them.
//✔ Resetting texture unit state is not required by OpenGL and provides no benefit.
//If you want, I can show you a clean, bulletproof texture - binding block for your PBR renderer that avoids 100 % of “program texture usage” errors.
//
//
//
//
//
//
//
//
//You said :
//Is this correct ? bool engine::Material::bind(engine::Shader & shader) const
//{
//    unsigned int textureUnit = 0;
//    bool success = true;
//
//    for (const auto& texture : textures)
//    {
//        const std::string uniformName = std::format("material.{}", texture.type);
//        const std::string hasMapName = std::format("material.has_{}_map", texture.type);
//
//        shader.use();
//
//        if (texture.id > 0)
//        {
//            glActiveTexture(GL_TEXTURE0 + textureUnit);
//            glBindTexture(GL_TEXTURE_2D, texture.id);
//
//            GLenum error = glGetError();
//            if (error != GL_NO_ERROR)
//            {
//                std::cerr << "OpenGL error while binding texture '" << texture.type
//                    << "' to unit " << textureUnit << ": " << std::hex << error << std::endl;
//                shader.setBool(hasMapName, false);
//                success = false;
//                continue;
//            }
//
//            shader.setInt(uniformName, textureUnit);
//            shader.setBool(hasMapName, true);
//            textureUnit++;
//        }
//        else
//        {
//            //std::cerr << "Warning: Texture ID is 0 for '" << texture.type << "'. Texture might not be loaded correctly." << std::endl;
//            shader.setBool(hasMapName, false);
//            //success = false;
//            success = true;
//        }
//    }
//
//    glActiveTexture(GL_TEXTURE0); // Reset active texture
//    return success;
//}void engine::Material::unbind() const
//{
//    for (size_t i = 0; i < textures.size(); ++i)
//    {
//        if (textures[i].id > 0)
//        {
//            glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(i));
//            glBindTexture(GL_TEXTURE_2D, 0);
//
//            GLenum error = glGetError();
//            if (error != GL_NO_ERROR)
//            {
//                std::cerr << "OpenGL error while unbinding texture unit " << i << ": " << std::hex << error << std::endl;
//            }
//        }
//    }
//
//    glActiveTexture(GL_TEXTURE0); // Reset to default
//}
//Copilot said :
//Copilot
//
//Raisonné pour 1s
//
//Evaluating shader handling
//
//Refining shader parameters
//Short answer : mostly OK, but there are a few important issues that can bite you :
//
//You call shader.use() inside the loop for every texture—wasteful; do it once before the loop.
//You don’t check device limits(GL_MAX_TEXTURE_IMAGE_UNITS) before incrementing textureUnit.You can easily overflow on iGPUs(common limit = 16).
//You always bind as GL_TEXTURE_2D; if some materials use cube / array / buffer textures later, this will be wrong.
//You reset the active unit to 0 at the end; not required and can hide bugs.
//unbind() assumes the same units as bind() (index - based), which may not match if you skip textures or if you add a starting offset.
//You’re mixing material units with global renderer units(IBL, BRDF, LTC).If your material begins at unit 0, you can collide with your global binds unless you reserve a base offset for materials.
//Per - call glGetError() is expensive; if you keep it, at least compile it out in release.
//success is overwritten to true in the else branch, negating earlier failures.
//Below is a refined version that fixes these and scales better.
//
//What I recommend changing
//A) Decide a base unit offset for materials
//Reserve low(or high) units for engine - wide resources(skybox, IBL, BRDF, LTC, shadow maps).Then start material textures at baseUnit, and return the next free unit.
//
//Example(renderer - wide) :
//
//
//
//
//    C++
//    // Renderer-reserved units (example, but validate against GL_MAX_TEXTURE_IMAGE_UNITS):
//    constexpr int U_BG_ENV = 0;
//constexpr int U_IRR = 7;
//constexpr int U_PREF = 8;
//constexpr int U_BRDF = 9;
//constexpr int U_LTC1 = 10;
//constexpr int U_LTC2 = 11;
//
//// Materials begin here:
//constexpr int MATERIAL_BASE_UNIT = 12;
//Make sure all of these indices are < GL_MAX_TEXTURE_IMAGE_UNITS.
//
//    B) Bind once per material call; don’t spam use()
//    Call shader.use() once per bind() call.Also avoid resetting the active unit at the end—it’s unnecessary.
//
//    C) Validate limits before binding
//    If textureUnit >= GL_MAX_TEXTURE_IMAGE_UNITS, stop and report an error.
//
//    D) Keep track of which target each texture uses
//    If your Texture struct only has id and type(albedo, normal, …), add a GL target(e.g., GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_2D_ARRAY, …).Then bind the proper target.For now, if your project only uses 2D textures for materials, your binding is fine—but be aware of the limitation.
//
//    E) Don’t try to “unbind” units
//    You rarely need to unbind textures after a draw.OpenGL binds are cheap to overwrite, not to clear.Removing unbind reduces state churn and avoids accidental unit mismatch.If you keep unbind(), it must use the same unit mapping as bind() (including the base offset and skipping missing textures).
//
//    F) Fix the success flag logic
//    If any texture bind fails, keep success = false (don’t reset it to true in later branches).
//
//    Improved bind() implementation
//
//
//
//    C++
//    bool engine::Material::bind(engine::Shader & shader, int baseUnit /*= MATERIAL_BASE_UNIT*/) const
//{
//    // Query once per program startup and cache these caps, not every bind.
//    GLint maxFragUnits = 16;
//    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxFragUnits);
//
//    // Use shader once for the whole material
//    shader.use();
//
//    unsigned int unit = static_cast<unsigned int>(baseUnit);
//    bool success = true;
//
//    for (const auto& tex : textures)
//    {
//        // Example: tex.type could be "albedo", "normal", "metallic", ...
//        const std::string uniformName = std::format("material.{}", tex.type);
//        const std::string hasMapName = std::format("material.has_{}_map", tex.type);
//
//        // If no texture, mark false and continue (don’t change unit).
//        if (tex.id == 0)
//        {
//            shader.setBool(hasMapName, false);
//            continue;
//        }
//
//        // Ensure we have room on the device
//        if (static_cast<int>(unit) >= maxFragUnits)
//        {
//            std::cerr << "[Material::bind] ERROR: Texture unit " << unit
//                << " exceeds GL_MAX_TEXTURE_IMAGE_UNITS=" << maxFragUnits
//                << " for uniform '" << uniformName << "'\n";
//            shader.setBool(hasMapName, false);
//            success = false;
//            break; // or return false;
//        }
//
//        // Bind using the right target. If you only use 2D textures for materials, keep GL_TEXTURE_2D.
//        GLenum target = GL_TEXTURE_2D; // TODO: extend your Texture to store its target.
//        glActiveTexture(GL_TEXTURE0 + unit);
//        glBindTexture(target, tex.id);
//
//#ifndef NDEBUG
//        // Keep this in debug only; glGetError is expensive
//        if (GLenum error = glGetError(); error != GL_NO_ERROR)
//        {
//            std::cerr << std::hex
//                << "[Material::bind] GL error 0x" << error
//                << " binding '" << tex.type << "' to unit " << unit
//                << " (target=" << target << ")\n" << std::dec;
//            shader.setBool(hasMapName, false);
//            success = false;
//            // Continue to bind others, or break if you prefer fail-fast.
//            ++unit;
//            continue;
//        }
//#endif
//
//        // Assign the sampler to the unit and flag presence
//        shader.setInt(uniformName, static_cast<int>(unit));
//        shader.setBool(hasMapName, true);
//
//        ++unit;
//    }
//
//    // No need to reset glActiveTexture here.
//    return success;
//}
//
//Afficher plus de lignes
//Optional : track bound units if you insist on unbind()
//If you really need an unbind(e.g., for a debug layer), capture which units you touched during bind() and only unbind those.Otherwise, I recommend removing unbind() entirely.
//
//
//
//
//C++
//void engine::Material::unbind(int baseUnit /*= MATERIAL_BASE_UNIT*/) const
//{
//    unsigned int unit = static_cast<unsigned int>(baseUnit);
//    for (const auto& tex : textures)
//    {
//        if (tex.id == 0)
//            continue;
//
//        glActiveTexture(GL_TEXTURE0 + unit);
//        glBindTexture(GL_TEXTURE_2D /* or tex.target */, 0);
//        ++unit;
//    }
//    // No need to set back to GL_TEXTURE0
//}
//
//Afficher plus de lignes
//Avoid collisions with engine - reserved units
//Because your renderer already uses units for IBL / LTC / BRDF / skybox, make sure you don’t let materials start at 0. Pass a baseUnit from the renderer :
//
//
//
//
//C++
//// In your draw code:
//int nextUnit = MATERIAL_BASE_UNIT; // 12 in our earlier example
//nextUnit = material.bind(pbrShader, nextUnit) ? nextUnit : nextUnit; // bind uses and advances
//// If your `bind` returns the next free unit, you can handle variable sizes gracefully.
//You can also refactor bind to return the next unit index instead of a bool, and use a separate out - parameter or error flag.
//
//Sanity checklist
//shader.use() called once per draw / material, not per texture.
//Material textures start at a reserved base unit to avoid clashing with IBL / BRDF / LTC.
//Check GL_MAX_TEXTURE_IMAGE_UNITS at startup; assert / log when exceeding.
//Don’t reset glActiveTexture to 0.
//Prefer not to “unbind”; just overwrite in the next pass.
//(If / when needed) support non - 2D targets by storing target in your Texture struct.
//Keep glGetError() behind #ifndef NDEBUG.
//If you paste your Texture struct (fields) and the material GLSL block(struct Material { ... }), I can tailor the binding code to your exact uniform names, handle normals / metallic / roughness packing, and ensure you never collide with your engine’s global texture units.
//
//
//
//
//
//
//
//
//You said :
//Is the order of the Sampler2D uniforms important in the frag shader ?
//Copilot said :
//Copilot
//Short answer : No — the order of sampler2D uniforms in your fragment shader does not matter at all.
//
//Long answer : here’s the full explanation so you understand why.
//
//✅ 1. The order of sampler uniforms in GLSL does not matter
//In GLSL, sampler uniforms are simply variables in the shader.
//Their position in :
//
//the struct Material { ... }
//the top of the shader
//any block
//or the order they appear in code
//does not affect how OpenGL assigns texture units.
//
//Example — both of these are 100 % valid:
//
//
//
//
//GLSL
//struct Material {
//    sampler2D albedo;
//    sampler2D normal;
//    sampler2D roughness;
//};
//and
//
//
//
//
//GLSL
//struct Material {
//    sampler2D roughness;
//    sampler2D normal;
//    sampler2D albedo;
//};
//No difference.
//
//Because GLSL does not map sampler → texture unit based on order.
//You assign that manually with glUniform1i() (or via setInt()).
//
//✅ 2. What does matter ?
//✔ Correct sampler → correct texture unit
//
//
//
//C++
//glActiveTexture(GL_TEXTURE0 + 3);
//glBindTexture(GL_TEXTURE_2D, roughnessTex);
//pbrShader.setInt("material.roughness", 3);
//This association defines the mapping.
//
//✔ Sampler type must match the texture target
//GLSL sampler	Must bind texture target
//sampler2D	GL_TEXTURE_2D
//samplerCube	GL_TEXTURE_CUBE_MAP
//sampler2DShadow	depth texture + compare mode
//isampler2D	integer internal format
//usampler2D	unsigned integer internal format
//Mismatch → GL_INVALID_OPERATION(502)
//(“program texture usage”, exactly what you got).
//
//✔ No two different sampler types may share the same texture unit in one program
//This will also produce the OpenGL error you saw.
//
//Example of illegal setup :
//
//
//
//
//GLSL
//sampler2D albedo;
//samplerCube environment;
//both assigned to unit 4:
//
//albedo = 4
//environment = 4
//Result → GL_INVALID_OPERATION.
//
//❌ What does NOT matter
//✘ The textual order in the shader
//✘ How you arrange uniforms in a struct
//✘ How you declare them in your C++ code
//✘ The numbering or ordering of binding calls as long as each sampler receives a valid texture unit
//None of that matters.
//
//🎯 Example : perfectly valid shader with random order
//
//
//
//GLSL
//struct Material {
//    sampler2D metallic;
//    sampler2D albedo;
//    sampler2D ao;
//    sampler2D roughness;
//    sampler2D normal;
//};
//In C++:
//
//
//
//
//C++
//shader.setInt("material.albedo", 0);
//shader.setInt("material.normal", 1);
//shader.setInt("material.metallic", 2);
//shader.setInt("material.roughness", 3);
//shader.setInt("material.ao", 4);
//Works fine.
//
//🧠 Important note for structs
//The name does matter :
//
//
//
//
//GLSL
//uniform Material material;
//Meaning in C++ the uniform names become :
//
//material.albedo
//material.normal
//material.metallic
//...
//If you change material to mat, the names change :
//
//mat.albedo
//mat.normal
//mat.metallic
//...
//But again, order inside the struct still does not matter.
//
//🚀 Summary
//✔ The order of sampler2D uniforms in your fragment shader does not matter.
//What does matter :
//
//Correct texture unit assignment(setInt)
//Correct texture binding(glActiveTexture + glBindTexture)
//No sampler type conflicts on the same unit
//Texture units must exist(< GL_MAX_TEXTURE_IMAGE_UNITS)
//    Texture target must match sampler type
//    If these conditions are met, your sampler order is irrelevant.
//
//    If you want, you can paste your Material GLSL struct, and I’ll verify :
//
//correct names
//correct mapping to C++
//no hidden sampler issues(the most common cause of the 502 error).
//
//
//
//
//
//
//
//You said :
//Here the setInt uniform should be done at first or at last ?
//lActiveTexture(GL_TEXTURE0 + 3);
//glBindTexture(GL_TEXTURE_2D, roughnessTex);
//pbrShader.setInt("material.roughness", 3);
//Copilot said :
//Copilot
//Great question — and a very important detail in OpenGL texture binding.
//
//✅ Correct order : Uniform first OR bind first — both work — BUT there is a recommended best practice
//Technically :
//
//✔ You can call setInt("material.roughness", 3) before binding
//✔ You can call it after binding
//It makes zero difference to OpenGL, because :
//
//    setInt("sampler", unitIndex) only stores an integer in the shader program.
//    The shader does not check bindings until draw time(glDrawArrays / glDrawElements).
//    Texture binding is global OpenGL state, not tied to a program.
//    So this is valid :
//
//
//
//
//C++
//pbrShader.setInt("material.roughness", 3);
//glActiveTexture(GL_TEXTURE0 + 3);
//glBindTexture(GL_TEXTURE_2D, roughnessTex);
//And this is valid :
//
//
//
//
//C++
//glActiveTexture(GL_TEXTURE0 + 3);
//glBindTexture(GL_TEXTURE_2D, roughnessTex);
//pbrShader.setInt("material.roughness", 3);
//🚀 Recommended best practice(important)
//Do all sampler setInt() calls once in setup() (after linking).
//Do all glActiveTexture / glBindTexture in the render loop, before draw.
//
//So inside your draw loop, the correct idiom is :
//
//
//
//
//C++
//glActiveTexture(GL_TEXTURE0 + 3);
//glBindTexture(GL_TEXTURE_2D, roughnessTex);
//// NO need to call setInt here normally
//❗ Why this is the best practice ?
//1. setInt should be done only once
//Samplers rarely change.Setting them every frame costs CPU time and risks mistakes.
//
//2. Binding textures changes every frame
//Material textures differ per object, so texture binding belongs in the render loop.
//
//3. Uniform location depends on shader linking
//You must call setInt after linking, so setup() is the safe place :
//
//
//
//
//C++
//pbrShader.use();
//pbrShader.setInt("material.roughness", 3);
//pbrShader.setInt("material.normal", 4);
//pbrShader.setInt("material.metallic", 5);
//pbrShader.setInt("material.albedo", 6);
//🔥 The only case where order matters
//If you mistakenly do:
//
//
//
//
//    C++
//        glBindTexture(GL_TEXTURE_2D, roughnessTex);  // WRONG: no glActiveTexture first!
//    pbrShader.setInt("material.roughness", 3);
//    This is wrong because glBindTexture binds to whichever texture unit was active(often unit 0).
//        So always do:
//
//
//
//
//    C++
//        glActiveTexture(GL_TEXTURE0 + unit);
//    glBindTexture(...);
//    🎯 Final answer
//        ✔ Correct : both orders work
//
//
//
//        C++
//        glActiveTexture(GL_TEXTURE0 + 3);
//    glBindTexture(GL_TEXTURE_2D, roughnessTex);
//    pbrShader.setInt("material.roughness", 3);
//    or
//
//
//
//
//        C++
//        pbrShader.setInt("material.roughness", 3);
//    glActiveTexture(GL_TEXTURE0 + 3);
//    glBindTexture(GL_TEXTURE_2D, roughnessTex);
//    ⭐ Recommended pattern :
//    In setup(once) :
//
//
//
//
//        C++
//        pbrShader.use();
//    pbrShader.setInt("material.roughness", 3);
//    In loop(per - frame) :
//
//
//
//
//        C++
//        glActiveTexture(GL_TEXTURE0 + 3);
//    glBindTexture(GL_TEXTURE_2D, roughnessTex);
//    If you want, I can help you reorganize your PBR binding code into a safe, minimal, error‑proof structure that completely prevents the GL_INVALID_OPERATION 502 errors you were seeing.
//
//
//
//
//
//
//
//
//        Message Copilot
//
//
//        Outils
//
//
//        Il est possible que le contenu généré par l’IA soit incorrect
//
