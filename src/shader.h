#pragma once

#include <string>

#include <gl/glew.h>
#include <glm/glm.hpp>

// =====================================================
class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    
    void Use() const;
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec3(const std::string& name, float x, float y, float z);
    void SetVec3(const std::string& name, const glm::vec3& value);
public:
    GLuint mShaderID;
};