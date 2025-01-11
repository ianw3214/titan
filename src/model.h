#pragma once

#include <vector>

#include <assimp/scene.h>

#include "mesh.h"

class Shader;

// =====================================================
class Model {
public:
    Model(const std::string& path)
    {
        LoadModel(path);
    }
    void Draw(Shader& shader);

private:
    void LoadModel(std::string path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> LoadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);

private:
    std::vector<Mesh> mMeshes;
    std::vector<Texture> mLoadedTextures;
    std::string mDirectory;
};