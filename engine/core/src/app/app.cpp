#include "../../include/app/app.h"

#include "../../include/debug/opengl_debug.h"
#include "../../include/singleton.h"

#include <format>

engine::App::App(const std::string& _title, unsigned int _width, unsigned int _height, bool _fullscreen, AppSettings _settings)
    : m_title(_title), width(static_cast<float>(_width)), height(static_cast<float>(_height)), fullscreen(_fullscreen), settings(_settings)
{
    logger.trace("App {} constructor called", m_title);

    logger.info("Engine startup");

    setup();
}

engine::App::~App()
{
    logger.trace("App {} destructor called", m_title);
}

const int engine::App::getFrameDelay()
{
    return settings.targetFPS > 0 ? (1000 / settings.targetFPS) : 0; // in milliseconds
}

const bool engine::App::capFramerate()
{
    return settings.targetFPS > 0 ? true : false;
}

bool engine::App::isRunning()
{
    return !glfwWindowShouldClose(window);
}

void engine::App::setup()
{
    initGLFW();

    const char* glsl_version = initOpenGL();

    initWindow();

    // boilerplate stuff (ie. basic window setup, initialize OpenGL) occurs in abstract class
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    enableVerticalSync(true);

    enableMouseCapture(true);

    initGLAD();

    initImGUI(glsl_version);

    // check opengl context is valid
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        // enable openGL debut ouput
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openglDebugCallback, nullptr);
    }
    else {
        logger.warn("OpenGL debug context not available.");
    }
}

void engine::App::initGLFW()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        logger.error("GLFW init failed");
        std::exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // Enable 4x MSAA

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // enable OpenGL debug output
}

const char* engine::App::initOpenGL()
{
    // GL 3.3 + GLSL 130
    const char* glsl_version = "#version 130";
    
    // opengl 3.3
    /*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);*/

    // opengl 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only and macOS

    return glsl_version;
}

void GLAPIENTRY engine::App::openglDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    // Ignore informational messages
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)// || severity == GL_DEBUG_SEVERITY_LOW || severity == GL_DEBUG_SEVERITY_MEDIUM)
        return;
    
    OpenGLDebug::debugMessage(source, type, id, severity, length, message, userParam);
}

void engine::App::initWindow()
{
    GLFWmonitor* myMonitor = glfwGetPrimaryMonitor(); // The primary monitor

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // Request debug context
    
    const GLFWvidmode* mode = glfwGetVideoMode(myMonitor);

    // Create window with graphics context
    window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), "Cortex engine", NULL, nullptr);
    if (window == NULL)
    {
        logger.error("Failed to create GLFW window");
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    if (fullscreen)
        toggleFullscreen([this](){});

    glfwMakeContextCurrent(window);
}

void engine::App::initGLAD()
{
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        logger.error("Failed to initialize GLAD");
        std::exit(EXIT_FAILURE);
    }
}

void engine::App::initImGUI(const char* glsl_version)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    ////ImGuiIO& io = ImGui::GetIO();
    //ImFont* defaultFont = io.Fonts->AddFontDefault(); // Default
    //ImFont* largeFont = io.Fonts->AddFontFromFileTTF("fonts/Raleway-Medium.ttf", 20.0f); // Larger font

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

    //if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    //{
    //    style.WindowRounding = 6.0f;
    //    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    //}

    // Apply Adobe Spectrum theme
    //https://github.com/adobe/imgui/blob/master/docs/Spectrum.md#imgui-spectrum
    ImGui::Spectrum::StyleColorsSpectrum();
    ImGui::Spectrum::LoadFont(17.0f);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void engine::App::enableVerticalSync(bool enable)
{
    // This enables V-Sync, capping the frame rate to the monitor's refresh rate (usually 60Hz or 144Hz).
    glfwSwapInterval(enable ? 1 : 0);
}

void engine::App::enableMouseCapture(bool enable)
{
    // tell GLFW to capture our mouse
    if (!enable)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

    // imGui Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);

    glfwTerminate();
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
        //glfwGetWindowSize(window, &width, &height);

        // Cast to float
        width = static_cast<float>(mode->width);
        height = static_cast<float>(mode->height);
    }
    else
    {
        logger.info("Toggle to windowed mode");

        // Restore windowed mode
        glfwSetWindowMonitor(window, nullptr, windowPosX, windowPosY, windowWidth, windowHeight, 0);
        //glfwGetWindowSize(window, &width, &height);
        
        // Cast to float
        width = static_cast<float>(windowWidth);
        height = static_cast<float>(windowHeight);
    }

    // reinit framebuffers because width and height changed
    func();

    isFullscreen = !isFullscreen;
	fullscreen = isFullscreen;
}