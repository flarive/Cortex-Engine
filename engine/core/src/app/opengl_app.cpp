#include "../../include/app/opengl_app.h"

engine::OpenGLApp::OpenGLApp(const std::string& _title, unsigned int _width, unsigned int _height, bool _fullscreen, AppSettings _settings)
    : App(_title, _width, _height, _fullscreen, _settings)
{
    logger.trace("OpenGLApp {} constructor called", _title);

    setup();
}

void engine::OpenGLApp::start()
{

}

void engine::OpenGLApp::setup()
{
    initGLFW();

    const char* glsl_version = initBackend();

    initWindow();

    setApp(window);

    // boilerplate stuff (ie. basic window setup, initialize OpenGL) occurs in abstract class
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    enableVerticalSync(true);

    enableMouseCapture(true);

    initGLAD();

    initImGUI(glsl_version);

    registerWindowInputCallbacks();

#if DEBUG
    // check opengl context is valid
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        // enable openGL debut ouput
        RenderState::enableDebugOutput();
        glDebugMessageCallback(openglDebugCallback, nullptr);
    }
    else {
        logger.warn("OpenGL debug context not available.");
    }
#endif
}

void engine::OpenGLApp::initGLFW()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        logger.error("GLFW init failed");
        std::exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // Enable 4x MSAA

#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // enable OpenGL debug output
#else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);
#endif
}

const char* engine::OpenGLApp::initBackend()
{
    // GL 3.3 + GLSL 130
    const char* glsl_version = "#version 130";

    // opengl 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only and macOS

    return glsl_version;
}

void engine::OpenGLApp::initWindow()
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
        toggleFullscreen([this]() {});

    glfwMakeContextCurrent(window);
}

void engine::OpenGLApp::initGLAD()
{
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        logger.error("Failed to initialize GLAD");
        std::exit(EXIT_FAILURE);
    }
}

void engine::OpenGLApp::initImGUI(const char* glsl_version)
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

    //ImGuiIO& io = ImGui::GetIO();

    //ImFont* fontSmall = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Regular.ttf", 14.0f);
    //ImFont* fontMedium = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Regular.ttf", 20.0f);
    //ImFont* fontLarge = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Bold.ttf", 32.0f);

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

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void engine::OpenGLApp::enableVerticalSync(bool enable)
{
    // This enables V-Sync, capping the frame rate to the monitor's refresh rate (usually 60Hz or 144Hz).
    glfwSwapInterval(enable ? 1 : 0);
}

void engine::OpenGLApp::enableMouseCapture(bool enable)
{
    // tell GLFW to capture our mouse
    if (!enable)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void engine::OpenGLApp::glfw_error_callback(int error, const char* description)
{
    logger.error("GLFW Error {}: {}", error, description);
    std::exit(EXIT_FAILURE);
}

void GLAPIENTRY engine::OpenGLApp::openglDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    // Ignore informational messages
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)// || severity == GL_DEBUG_SEVERITY_LOW || severity == GL_DEBUG_SEVERITY_MEDIUM)
        return;

    OpenGLDebug::debugMessage(source, type, id, severity, length, message, userParam);
}

void engine::OpenGLApp::exit()
{
    /*logger.info("Exiting app {}", m_title);

    m_sceneManager.clean();*/

    // imGui Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    //unregisterWindowInputCallbacks();

    glfwDestroyWindow(window);

    glfwTerminate();
}

engine::OpenGLApp::~OpenGLApp()
{
    logger.trace("OpenGLApp {} destructor called", m_title);
}