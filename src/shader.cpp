#include "shader.h"

#include <fstream>
#include <sstream>

// =====================================================
Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
    std::string vertexCode, fragmentCode;
    std::ifstream vertexFile(vertexPath);
    if (vertexFile.is_open())
    {
        std::stringstream vertexFileStream;
        vertexFileStream << vertexFile.rdbuf();
        vertexCode = vertexFileStream.str();
    }

    std::ifstream fragmentFile(fragmentPath);
    if (fragmentFile.is_open())
    {
        std::stringstream fragmentFileStream;
        fragmentFileStream << fragmentFile.rdbuf();
        fragmentCode = fragmentFileStream.str();
    }
    const char* vertexCodeStr = vertexCode.c_str();
    const char* fragmentCodeStr = fragmentCode.c_str();

    int success;
    char infoLog[512];

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexCodeStr, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) 
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        // TODO: Log error info contained in infoLog
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentCodeStr, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        // TODO: Log error info contained in infoLog
    }

    mShaderID = glCreateProgram();
    glAttachShader(mShaderID, vertexShader);
    glAttachShader(mShaderID, fragmentShader);
    glLinkProgram(mShaderID);
    glGetProgramiv(mShaderID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(mShaderID, 512, NULL, infoLog);
        // TODO: Log error info contained in infoLog
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

// =====================================================
void Shader::Use()
{
    glUseProgram(mShaderID);
}

// =====================================================
void Shader::SetBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(mShaderID, name.c_str()), static_cast<int>(value));
}

// =====================================================
void Shader::SetInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(mShaderID, name.c_str()), value);
}

// =====================================================
void Shader::SetFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(mShaderID, name.c_str()), value);
}
