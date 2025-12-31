#include "../include/shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "../include/common_defines.h"
#include "../include/managers/log_manager.h"

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
        exit(EXIT_FAILURE);
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


void engine::Shader::init(const char* shaderName, const char* vertexPath, const char* fragmentPath, const char* geometryPath)
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
        exit(EXIT_FAILURE);
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

    //logger.info("Shader program compiled and linked: {} (ID: {})", name, ID);

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glDeleteShader(geometry);
}

bool engine::Shader::isValid() const
{
    if (ID == 0) {
		logger.error("Shader program ID is 0. Shader not created.");
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
        std::cerr << "Shader::use(): Shader is not valid!" << std::endl;
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
            logger.error("Shader {} SHADER_COMPILATION_ERROR of type: {}", shader, infoLog);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            logger.error("Shader {} PROGRAM_LINKING_ERROR of type {}: {}", shader, type, infoLog);
            exit(EXIT_FAILURE);
        }
    }
}

engine::ShaderType engine::Shader::getShaderType()
{
    if (name == "blinnphong") return ShaderType::BlinnPhong;
    if (name == "pbr") return ShaderType::PBR;
    return ShaderType::Unknown;
}