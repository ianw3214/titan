#pragma once

#include <string>
#include <vector>

#include <gl/glew.h>
#include <glm/glm.hpp>

class Shader;

// =====================================================
struct Vertex {
    glm::vec3 mPosition;
    glm::vec3 mNormal;
    glm::vec2 mTextureCoords;
};

// =====================================================
struct Texture {
    GLuint mId;
    std::string mType;
    std::string mPath;
};

// =====================================================
class Mesh {
public:
    std::vector<Vertex> mVertices;
    std::vector<unsigned int> mIndices;
    std::vector<Texture> mTextures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(Shader& shader);

private:
    void SetupMesh();

private:
    GLuint mVAO;
    GLuint mVBO;
    GLuint mEBO;
};