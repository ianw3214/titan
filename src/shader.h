#pragma once

#include <string>

#include <gl/glew.h>

// =====================================================
class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    
    void Use() const;
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec3(const std::string& name, float x, float y, float z);
public:
    GLuint mShaderID;
};