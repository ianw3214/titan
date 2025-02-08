#include "directionalLight.h"

#include "shader.h"

// =====================================================
DirectionalLight::DirectionalLight(glm::vec3 direction)
    : mDirection(direction)
{

}

// =====================================================
void DirectionalLight::SetShaderUniforms(Shader& shader) const
{
    shader.SetVec3("directionalLight.mDirection", mDirection);
    shader.SetVec3("directionalLight.mAmbient", mAmbient);
    shader.SetVec3("directionalLight.mDiffuse", mDiffuse);
    shader.SetVec3("directionalLight.mSpecular", mSpecular);
}