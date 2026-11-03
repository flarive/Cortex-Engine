#include "../../include/app/app.h"

#include "../../include/debug/opengl_debug.h"
#include "../../include/singleton.h"

#include <format>

engine::App::App(const std::string& _title, unsigned int _width, unsigned int _height, bool _fullscreen, const AppSettings& _settings)
    : m_title(_title), width(static_cast<float>(_width)), height(static_cast<float>(_height)), fullscreen(_fullscreen), m_appSettings(_settings)
{
    logger.trace("App {} constructor called", m_title);

    logger.info("Engine startup");

    setup();
}

void engine::App::setup()
{
    // store SceneSettings in a singleton for easy access everywhere
    engine::Singleton::initialize(m_appSettings);
}

const int engine::App::getFrameDelay()
{
    return m_appSettings.targetFPS > 0 ? (1000 / m_appSettings.targetFPS) : 0; // in milliseconds
}

const bool engine::App::capFramerate()
{
    return m_appSettings.targetFPS > 0 ? true : false;
}

bool engine::App::isRunning()
{
    m_inputManager.beginFrame();

    // Poll and handle events (inputs, window resize, etc.)
    glfwPollEvents();

    // Handle keyboard/mouse/gamepad input (app level)
    onInputUpdate();

    return !glfwWindowShouldClose(window);
}

void engine::App::setApp(GLFWwindow* _window)
{
    // Store the 'this' pointer (App) in the window's user pointer
    glfwSetWindowUserPointer(_window, this);
}

void engine::App::initGLAD()
{
}

void engine::App::initImGUI(const char* backend_version)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags)
    {
        // Set rounding for ALL windows (including child windows)
        style.WindowRounding = 6.0f; // Rounding for regular windows
        style.ChildRounding = 6.0f;  // Rounding for child windows (docked windows)

        style.ItemSpacing.y = 8.0; // vertical padding between widgets
        style.FramePadding.x = 8.0; // better widget horizontal padding
        style.FramePadding.y = 4.0; // better widget vertical padding
    }

    // Apply Adobe Spectrum theme
    //https://github.com/adobe/imgui/blob/master/docs/Spectrum.md#imgui-spectrum
    ImGui::Spectrum::StyleColorsSpectrum();
    //ImGui::Spectrum::LoadFont(17.0f);
    ImGui::Spectrum::LoadFonts();
}

void engine::App::glfw_error_callback(int error, const char* description)
{
    logger.error("GLFW Error {}: {}", error, description);
    std::exit(EXIT_FAILURE);
}

void engine::App::setWindowTitle(bool appendFps)
{
    std::string title{};
    
    if (appendFps) {
        title = std::format("{} {} {:.0f} FPS {}", m_title_prefix, m_title, ImGui::GetIO().Framerate, m_title_suffix);
    }
    else {
        title = std::format("{} {} {}", m_title_prefix, m_title, m_title_suffix);
    }

    glfwSetWindowTitle(window, title.c_str());
}

void engine::App::setWindowTitle(const std::string& title)
{
    if (title != m_title)
    {
        m_title = title;
        setWindowTitle();
    }
}

void engine::App::setWindowTitlePrefix(const std::string& prefix)
{
    if (prefix != m_title_prefix)
    {
        m_title_prefix = prefix;
        setWindowTitle();
    }
}

void engine::App::resetWindowTitlePrefix()
{
    m_title_prefix.clear();
}

void engine::App::setWindowTitleSuffix(const std::string& suffix)
{
    if (suffix != m_title_suffix)
    {
        m_title_suffix = suffix;
        setWindowTitle();
    }
}

void engine::App::resetWindowTitleSuffix()
{
    m_title_suffix.clear();
}

void engine::App::exit()
{
    logger.info("Exiting app {}", m_title);

	m_sceneManager.clean();

    unregisterWindowInputCallbacks();
}

// Toggle Fullscreen
void engine::App::toggleFullscreen(std::function<void()> func)
{
    static bool isFullscreen = false;

    // remember window original position and size
    static int windowPosX, windowPosY;
    static int windowWidth, windowHeight;

    if (!isFullscreen)
    {
        logger.info("Toggle to fullscreen mode");

        // Save window position and size
        glfwGetWindowPos(window, &windowPosX, &windowPosY);
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        // Get primary monitor
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        // Switch to fullscreen
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

        // Cast to float
        width = static_cast<float>(mode->width);
        height = static_cast<float>(mode->height);
    }
    else
    {
        logger.info("Toggle to windowed mode");

        // Restore windowed mode
        glfwSetWindowMonitor(window, nullptr, windowPosX, windowPosY, windowWidth, windowHeight, 0);
        
        // Cast to float
        width = static_cast<float>(windowWidth);
        height = static_cast<float>(windowHeight);
    }

    // reinit framebuffers because width and height changed
    func();

    isFullscreen = !isFullscreen;
	fullscreen = isFullscreen;
}

void engine::App::registerWindowInputCallbacks()
{
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetWindowRefreshCallback(window, windowRefreshCallback);
    glfwSetJoystickCallback(joystickCallback);
}

void engine::App::unregisterWindowInputCallbacks()
{
    glfwSetFramebufferSizeCallback(window, nullptr);
    glfwSetKeyCallback(window, nullptr);
    glfwSetCursorPosCallback(window, nullptr);
    glfwSetMouseButtonCallback(window, nullptr);
    glfwSetScrollCallback(window, nullptr);
    glfwSetWindowRefreshCallback(window, nullptr);
    glfwSetJoystickCallback(nullptr);
}

void engine::App::onResize(int width, int height)
{
    (void)width;   //Do nothing
    (void)height;   //Do nothing
}

void engine::App::onRefresh()
{

}

void engine::App::onInputUpdate()
{
    if (m_inputManager.isKeyPressed(GLFW_KEY_ESCAPE))
    {
        // closing app
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

engine::App::~App()
{
    logger.trace("App {} destructor called", m_title);
}