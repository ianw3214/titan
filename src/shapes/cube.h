#pragma once

#include <string>

class Shader;

#include <gl/glew.h>
#include <glm/glm.hpp>

// =====================================================
class Cube {
public:
    Cube(glm::vec3 position = glm::vec3(0.f));
    void Draw(Shader& shader, float scale = 1.f);

    void SetTexture(const std::string& path);

private:
    glm::vec3 mPosition;
    GLuint mTextureId;

    static GLuint sVAO;
};