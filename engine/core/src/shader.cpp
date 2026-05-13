#include "../include/shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "../include/common_defines.h"
#include "../include/managers/log_manager.h"

/// <summary>
/// OpenGL simple pipeline
/// VS => FS
/// </summary>
void engine::Shader::init(const char* shaderName, const char* vertexPath, const char* fragmentPath)
{
    name = shaderName;

    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode{};
    std::string fragmentCode{};
    std::ifstream vShaderFile{};
    std::ifstream fShaderFile{};
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        const char* error = e.what();
        logger.error("Shader {} FILE_NOT_SUCCESSFULLY_READ: {}", shaderName, error);
        throw std::runtime_error("Shader file read failed");
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    m_initialized = true;

	logger.info("Shader program compiled and linked: {} (ID: {})", name, ID);

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

/// <summary>
/// OpenGL pipeline without tessellation
/// VS => (optional GS) => FS
/// </summary>
void engine::Shader::init(const char* shaderName, const char* vertexPath, const char* geometryPath, const char* fragmentPath)
{
    name = shaderName;

    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode{};
    std::string fragmentCode{};
    std::string geometryCode{};
    std::ifstream vShaderFile{};
    std::ifstream fShaderFile{};
    std::ifstream gShaderFile{};
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        gShaderFile.open(geometryPath);
        std::stringstream vShaderStream, fShaderStream, gShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        gShaderStream << gShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        gShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        geometryCode = gShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        const char* error = e.what();
        logger.error("Shader {} FILE_NOT_SUCCESSFULLY_READ: {}", shaderName, error);
        throw std::runtime_error("Shader file read failed");
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    const char* gShaderCode = geometryCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment, geometry;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // geometry Shader
    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &gShaderCode, NULL);
    glCompileShader(geometry);
    checkCompileErrors(geometry, "GEOMETRY");
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glAttachShader(ID, geometry);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    m_initialized = true;

    logger.info("Shader program compiled and linked: {} (ID: {})", name, ID);

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glDeleteShader(geometry);
}

/// <summary>
/// OpenGL pipeline with tessellation
/// VS => TCS => TES => (optional GS) => FS
/// </summary>
void engine::Shader::init(const char* shaderName, const char* vertexPath, const char* tessControlPath, const char* tessEvalPath, const char* geometryPath, const char* fragmentPath)
{
    name = shaderName;

    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::string tessControlCode;
    std::string tessEvalCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    std::ifstream tcShaderFile;
    std::ifstream teShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    tcShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    teShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // if geometry shader path is present, also load a geometry shader
        if (geometryPath != nullptr)
        {
            gShaderFile.open(geometryPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
        if (tessControlPath != nullptr) {
            tcShaderFile.open(tessControlPath);
            std::stringstream tcShaderStream;
            tcShaderStream << tcShaderFile.rdbuf();
            tcShaderFile.close();
            tessControlCode = tcShaderStream.str();
        }
        if (tessEvalPath != nullptr) {
            teShaderFile.open(tessEvalPath);
            std::stringstream teShaderStream;
            teShaderStream << teShaderFile.rdbuf();
            teShaderFile.close();
            tessEvalCode = teShaderStream.str();
        }
    }
    catch (std::ifstream::failure& e)
    {
        const char* error = e.what();
        logger.error("Shader {} FILE_NOT_SUCCESSFULLY_READ: {}", shaderName, error);
        throw std::runtime_error("Shader file read failed");
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // if geometry shader is given, compile geometry shader
    unsigned int geometry;
    if (geometryPath != nullptr)
    {
        const char* gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
    }
    // if tessellation shader is given, compile tessellation shader
    unsigned int tessControl;
    if (tessControlPath != nullptr)
    {
        const char* tcShaderCode = tessControlCode.c_str();
        tessControl = glCreateShader(GL_TESS_CONTROL_SHADER);
        glShaderSource(tessControl, 1, &tcShaderCode, NULL);
        glCompileShader(tessControl);
        checkCompileErrors(tessControl, "TESS_CONTROL");
    }
    unsigned int tessEval;
    if (tessEvalPath != nullptr)
    {
        const char* teShaderCode = tessEvalCode.c_str();
        tessEval = glCreateShader(GL_TESS_EVALUATION_SHADER);
        glShaderSource(tessEval, 1, &teShaderCode, NULL);
        glCompileShader(tessEval);
        checkCompileErrors(tessEval, "TESS_EVALUATION");
    }
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragmentSource = fragmentCode;
    if (tessEvalPath != nullptr) {
        fragmentSource = std::string("#define IS_TESSELLATED\n") + fragmentCode;
    }
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    if (tessControlPath != nullptr)
        glAttachShader(ID, tessControl);
    if (tessEvalPath != nullptr)
        glAttachShader(ID, tessEval);
    if (geometryPath != nullptr)
        glAttachShader(ID, geometry);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    
    m_initialized = true;

    logger.info("Shader program compiled and linked: {} (ID: {})", name, ID);
    
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    if (tessControlPath != nullptr)
        glDeleteShader(tessControl);
    if (tessEvalPath != nullptr)
        glDeleteShader(tessEval);
    if (geometryPath != nullptr)
        glDeleteShader(geometry);
    glDeleteShader(fragment);
}

bool engine::Shader::isValid() const
{
    if (ID == 0) {
		//logger.error("Shader program ID is 0. Shader not created.");
        return false;
    }

    GLint linkStatus = 0;
    glGetProgramiv(ID, GL_LINK_STATUS, &linkStatus);

    if (linkStatus == GL_FALSE) {
        GLint logLength = 0;
        glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &logLength);

        std::vector<char> log(logLength);
        glGetProgramInfoLog(ID, logLength, nullptr, log.data());

		logger.error("Shader program linking failed: {}", log.data());
        return false;
    }

    return true;
}

// activate the shader
// ------------------------------------------------------------------------
void engine::Shader::use() const
{
    if (isValid()) {
        glUseProgram(ID);
    }
    else {
        std::cerr << "Shader::use(): Shader is not valid !" << std::endl;
    }
}

// check uniform in shader
// -------------------------------------------------------------------------
bool engine::Shader::checkShaderUniformExists(unsigned int shaderID, const std::string& uniformName) const
{
    GLint uniformLoc = glGetUniformLocation(shaderID, uniformName.c_str());
    if (uniformLoc == -1) {
		logger.error("Uniform '{}' not found in shader {}", uniformName, shaderID);
        return false;
    }

    return true;
}

// utility uniform functions
// ------------------------------------------------------------------------
void engine::Shader::setBool(const std::string& name, bool value) const
{
    GLint uniformLoc = glGetUniformLocation(ID, name.c_str());
    if (uniformLoc != -1)
    {
        glUniform1i(uniformLoc, (int)value);
        //logger.info("Shader {} Set bool uniform '{}'", ID, name);
    }
    else
    {
        logger.error("Uniform '{}' not found in shader {}", name, ID);
    }
}
// ------------------------------------------------------------------------
void engine::Shader::setInt(const std::string& name, int value) const
{
    GLint uniformLoc = glGetUniformLocation(ID, name.c_str());
    if (uniformLoc != -1)
    {
        glUniform1i(uniformLoc, value);
        //logger.info("Shader {} Set int uniform '{}'", ID, name);
    }
    else
    {
        logger.error("Uniform '{}' not found in shader {}", name, ID);
    }
}
// ------------------------------------------------------------------------
void engine::Shader::setFloat(const std::string& name, float value) const
{
    GLint uniformLoc = glGetUniformLocation(ID, name.c_str());
    if (uniformLoc != -1)
    {
        glUniform1f(uniformLoc, value);
        //logger.info("Shader {} Set float uniform '{}'", ID, name);
    }
    else
    {
        logger.error("Uniform '{}' not found in shader {}", name, ID);
    }
}
// ------------------------------------------------------------------------
void engine::Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    GLint uniformLoc = glGetUniformLocation(ID, name.c_str());
    if (uniformLoc != -1)
    {
        glUniform2fv(uniformLoc, 1, &value[0]);
        //logger.info("Shader {} Set vec2 uniform '{}'", ID, name);
    }
    else
    {
        logger.error("Uniform '{}' not found in shader {}", name, ID);
    }
}
void engine::Shader::setVec2(const std::string& name, float x, float y) const
{
    GLint uniformLoc = glGetUniformLocation(ID, name.c_str());
    if (uniformLoc != -1)
    {
        glUniform2f(uniformLoc, x, y);
        //logger.info("Shader {} Set vec2 uniform '{}'", ID, name);
    }
    else
    {
        logger.error("Uniform '{}' not found in shader {}", name, ID);
    }
}
// ------------------------------------------------------------------------
void engine::Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    GLint uniformLoc = glGetUniformLocation(ID, name.c_str());
    if (uniformLoc != -1)
    {
        glUniform3fv(uniformLoc, 1, &value[0]);
        //logger.info("Shader {} Set vec3 uniform '{}'", ID, name);
    }
    else
    {
        logger.error("Uniform '{}' not found in shader {}", name, ID);
    }
}
void engine::Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    GLint uniformLoc = glGetUniformLocation(ID, name.c_str());
    if (uniformLoc != -1)
    {
        glUniform3f(uniformLoc, x, y, z);
        //logger.info("Shader {} Set vec3 uniform '{}'", ID, name);
    }
    else
    {
        logger.error("Uniform '{}' not found in shader {}", name, ID);
    }
}
// ------------------------------------------------------------------------
void engine::Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    GLint uniformLoc = glGetUniformLocation(ID, name.c_str());
    if (uniformLoc != -1)
    {
        glUniform4fv(uniformLoc, 1, &value[0]);
        //logger.info("Shader {} Set vec4 uniform '{}'", ID, name);
    }
    else
    {
        logger.error("Uniform '{}' not found in shader {}", name, ID);
    }
}
void engine::Shader::setVec4(const std::string& name, float x, float y, float z, float w)
{
    GLint uniformLoc = glGetUniformLocation(ID, name.c_str());
    if (uniformLoc != -1)
    {
        glUniform4f(uniformLoc, x, y, z, w);
        //logger.info("Shader {} Set vec4 uniform '{}'", ID, name);
    }
    else
    {
        logger.error("Uniform '{}' not found in shader {}", name, ID);
    }
}
// ------------------------------------------------------------------------
void engine::Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    GLint uniformLoc = glGetUniformLocation(ID, name.c_str());
    if (uniformLoc != -1)
    {
        glUniformMatrix2fv(uniformLoc, 1, GL_FALSE, &mat[0][0]);
        //logger.info("Shader {} Set mat2 uniform '{}'", ID, name);
    }
    else
    {
        logger.error("Uniform '{}' not found in shader {}", name, ID);
    }
}
// ------------------------------------------------------------------------
void engine::Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    GLint uniformLoc = glGetUniformLocation(ID, name.c_str());
    if (uniformLoc != -1)
    {
        glUniformMatrix3fv(uniformLoc, 1, GL_FALSE, &mat[0][0]);
        //logger.info("Shader {} Set mat3 uniform '{}'", ID, name);
    }
    else
    {
        logger.error("Uniform '{}' not found in shader {}", name, ID);
    }
}
// ------------------------------------------------------------------------
void engine::Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    GLint uniformLoc = glGetUniformLocation(ID, name.c_str());
    if (uniformLoc != -1)
    {
        glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, &mat[0][0]);
        //logger.info("Shader {} Set mat4 uniform '{}'", ID, name);
    }
    else
    {
        logger.error("Uniform '{}' not found in shader {}", name, ID);
    }
}

void engine::Shader::clean()
{
    if (ID > 0)
    {
        if (isValid()) {
            glDeleteProgram(ID);
        }
        logger.info("Shader program {} (ID: {}) deleted", name, ID);
        ID = 0;
        name.clear();
        m_initialized = false;
    }
}

void engine::Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            logger.error("Shader {} SHADER_COMPILATION_ERROR of type {}: {}", shader, type, infoLog);
            throw std::runtime_error("Shader compilation error");
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            logger.error("Shader {} PROGRAM_LINKING_ERROR of type {}: {}", shader, type, infoLog);
            throw std::runtime_error("Shader linking error");
        }
    }
}

engine::ShaderType engine::Shader::getShaderType()
{
    if (name == "phong") return ShaderType::Phong;
    if (name == "blinnphong") return ShaderType::BlinnPhong;
    if (name == "blinnphongTessellation") return ShaderType::BlinnPhongTessellation;
    if (name == "pbr") return ShaderType::PBR;
    if (name == "pbrTessellation") return ShaderType::PBRTessellation;
    if (name == "simpleDepthBuffer1") return ShaderType::DepthBufferDirectionalLights;
    if (name == "simpleDepthBuffer2") return ShaderType::DepthBufferPointLights;
    return ShaderType::Unknown;
}

void engine::Shader::getActiveUniformsList(const std::string& uniformName)
{
    GLint count = 0;
    glGetProgramInterfaceiv(ID, GL_UNIFORM, GL_ACTIVE_RESOURCES, &count);

    // Query a few helpful properties.
    const GLenum props[] = {
        GL_NAME_LENGTH,
        GL_TYPE,
        GL_LOCATION,
        GL_BLOCK_INDEX,
        GL_ARRAY_SIZE,             // helpful for arrays
        GL_REFERENCED_BY_VERTEX_SHADER,
        GL_REFERENCED_BY_TESS_CONTROL_SHADER,
        GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
        GL_REFERENCED_BY_GEOMETRY_SHADER,
        GL_REFERENCED_BY_FRAGMENT_SHADER,
        GL_REFERENCED_BY_COMPUTE_SHADER
    };

    for (GLint i = 0; i < count; ++i)
    {
        GLint values[sizeof(props) / sizeof(props[0])] = {};
        glGetProgramResourceiv(ID, GL_UNIFORM, i,
            static_cast<GLsizei>(std::size(props)),
            props,
            static_cast<GLsizei>(std::size(values)),
            nullptr,
            values);

        const GLint nameLen = values[0];
        const GLenum type = static_cast<GLenum>(values[1]);
        const GLint location = values[2];
        const GLint blockIndex = values[3];
        const GLint arraySize = values[4];

        // Get the name
        std::string uname(nameLen, '\0');
        glGetProgramResourceName(ID, GL_UNIFORM, i, nameLen, nullptr, uname.data());
        if (!uname.empty() && uname.back() == '\0') uname.pop_back();

        if (uname != uniformName)
            continue;

        // Determine if it "survived"
        // - Default-block uniform: survives if listed; location != -1 means assignable via glUniform*
        // - Block member: survives if listed; location will be -1 by definition
        bool survived = true; // if it's in the interface, it's active

        const bool inBlock = (blockIndex >= 0);
        const bool hasLocation = (location != -1);

        // Optional: stage usage (can help you see where it is live)
        const bool refVS = values[5] != 0;
        const bool refTCS = values[6] != 0;
        const bool refTES = values[7] != 0;
        const bool refGS = values[8] != 0;
        const bool refFS = values[9] != 0;
        const bool refCS = values[10] != 0;

        logger.info(
            "Uniform '{}' | type=0x{:04X} | arraySize={} | location={} | blockIndex={} | used(VS/TCS/TES/GS/FS/CS)={}/{}/{}/{}/{}/{} | survived={}",
            uname, type, arraySize, location, blockIndex,
            refVS, refTCS, refTES, refGS, refFS, refCS,
            survived
        );

        // If you specifically want to know whether you can set it with glUniform*:
        if (!inBlock && hasLocation) {
            // Assignable via glUniform*
        }
        else if (inBlock) {
            // It's a UBO member; query the block/binding and set via buffer data, not glUniform*
        }
        else {
            // Not assignable (e.g., special/opaque, or driver reports location -1)
        }
    }
}