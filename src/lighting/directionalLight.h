#pragma once

#include <glm/glm.hpp>

class Shader;

// =====================================================
class DirectionalLight {
public:
    DirectionalLight(glm::vec3 direction);

    // Sets 4 corresponding uniforms in the shader:
    //  - mDirection
    //  - mAmbient
    //  - mDiffuse
    //  - mSpecular
    void SetShaderUniforms(Shader& shader) const;

private:
    glm::vec3 mDirection;
    glm::vec3 mAmbient = glm::vec3(0.2f);
    glm::vec3 mDiffuse = glm::vec3(0.5f);
    glm::vec3 mSpecular = glm::vec3(1.f);
};