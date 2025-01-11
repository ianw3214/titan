#include "model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// =====================================================
unsigned int TextureFromFile(const char* file, const std::string& directory)
{
    std::string fileName = directory + '/' + file;
    GLuint textureID;
    glGenTextures(1, &textureID);

    // Texture loading
    stbi_set_flip_vertically_on_load(true);
    int width, height, numChannels;
    unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &numChannels, 0);
    if (data == nullptr)
    {
        // TODO: Error here
        return 0;
    }
    GLenum format;
    if (numChannels == 1) format = GL_RED;
    if (numChannels == 3) format = GL_RGB;
    if (numChannels == 4) format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return textureID;
}

// =====================================================
void Model::Draw(Shader& shader)
{
    for (Mesh& mesh : mMeshes)
    {
        mesh.Draw(shader);
    }
}

// =====================================================
void Model::LoadModel(std::string path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr)
    {
        // TODO: Error
        return;
    }

    mDirectory = path.substr(0, path.find_last_of('/'));
    ProcessNode(scene->mRootNode, scene);
}

// =====================================================
void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        mMeshes.push_back(ProcessMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

// =====================================================
Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;
        vertex.mPosition = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.mNormal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        if (mesh->mTextureCoords[0] != nullptr)
        {
            vertex.mTextureCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else
        {
            vertex.mTextureCoords = glm::vec2(0.f, 0.f);
        }
        vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

// =====================================================
std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < material->GetTextureCount(type); ++i)
    {
        aiString str;
        material->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < mLoadedTextures.size(); ++j)
        {
            if (std::strcmp(mLoadedTextures[j].mPath.data(), str.C_Str()) == 0)
            {
                textures.push_back(mLoadedTextures[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {
            Texture texture;
            texture.mId = TextureFromFile(str.C_Str(), mDirectory);
            texture.mType = typeName;
            texture.mPath = str.C_Str();
            textures.push_back(texture);
            mLoadedTextures.push_back(texture);
        }
    }
    return textures;
}