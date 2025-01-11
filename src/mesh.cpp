#include "mesh.h"

#include "shader.h"

// =====================================================
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    : mVertices(vertices)
    , mIndices(indices)
    , mTextures(textures)
{
    SetupMesh();
}

// =====================================================
void Mesh::Draw(Shader& shader)
{
    unsigned int numDiffuse = 1;
    unsigned int numSpecular = 1;
    for (unsigned int i = 0; i < mTextures.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string number;
        std::string name = mTextures[i].mType;
        if (name == "texture_diffuse")
        {
            number = std::to_string(numDiffuse++);
        }
        else
        {
            number = std::to_string(numSpecular++);
        }
        shader.SetInt(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, mTextures[i].mId);
    }
    glActiveTexture(GL_TEXTURE0);

    // Draw mesh
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// =====================================================
void Mesh::SetupMesh()
{
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), mIndices.data(), GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mNormal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mTextureCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}