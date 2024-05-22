#pragma once

#include <string>
#include <vector>
#include <assimp/scene.h>

#include "Mesh.h"

class Shader;

class Model
{
public:

	Model(const std::string& path) { loadModel(path); }
	void Draw(Shader& shader);

private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> texturesLoaded;

	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType engineType);
	unsigned int textureFromFile(const char* path, const std::string& directory, bool gamma = true);
};
