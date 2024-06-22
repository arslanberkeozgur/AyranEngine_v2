#pragma once

#include <string>
#include <vector>
#include <assimp/scene.h>

#include "Mesh.h"

class Shader;
struct TextureRenderOption;

class Model
{
public:

	Model(const std::string& path, const std::string& Name) : name{ Name } { loadModel(path); }
	void Draw(Shader& shader);
	~Model();

	void ApplyOptionToAllTextures(TextureRenderOption option);

	bool isTransparent = false;
	std::string name;

private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture2D> texturesLoaded;

	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture2D> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType engineType);
};

struct TextureRenderOption
{
	GLenum horizontalWrapMode = GL_REPEAT;
	GLenum verticalWrapMode	  = GL_REPEAT;
	GLenum minFilter		  = GL_LINEAR_MIPMAP_LINEAR;
	GLenum magFilter		  = GL_LINEAR;
};
