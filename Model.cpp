#include "Model.h"
#include "Shader.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>
#include <glad/glad.h>
#include "stb_image.h"


void Model::Draw(Shader& shader)
{
	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		meshes[i].Draw(shader);
	}
}

void Model::loadModel(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// Process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// Then do the same for each of its children.
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture2D> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;

		// Process vertex positions, normals and texture coordinates.
		glm::vec3 position;
		position.x = mesh->mVertices[i].x;
		position.y = mesh->mVertices[i].y;
		position.z = mesh->mVertices[i].z;
		vertex.Position = position;
		
		glm::vec3 normal;
		normal.x = mesh->mNormals[i].x;
		normal.y = mesh->mNormals[i].y;
		normal.z = mesh->mNormals[i].z;
		vertex.Normal = normal;

		if (mesh->mTextureCoords[0])
		{
			glm::vec2 texture;
			texture.x = mesh->mTextureCoords[0][i].x;
			texture.y = mesh->mTextureCoords[0][i].y;
			vertex.TextureCoords = texture;
		}
		else
		{
			vertex.TextureCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}
	// Process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	// Process material
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture2D> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		
		std::vector<Texture2D> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture2D> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType engineType)
{
	std::vector<Texture2D> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
	{
		aiString path;
		mat->GetTexture(type, i, &path);

		bool skip = false;
		for (unsigned int j = 0; j < texturesLoaded.size(); ++j)
		{
			if (std::strcmp(texturesLoaded[j].path.data(), path.C_Str()) == 0)
			{
				textures.push_back(texturesLoaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			Texture2D texture;
			TextureInfo info;
			texture.load(directory + '/' + std::string(path.C_Str()), info);
			isTransparent = true;
			texture.type = engineType;
			texture.path = path.C_Str();
			textures.push_back(texture);
			texturesLoaded.push_back(texture);
		}
	}
	return textures;
}

void Model::ApplyOptionToAllTextures(TextureRenderOption option)
{
	for (Mesh& mesh : meshes)
	{
		for (Texture2D& texture : mesh.textures)
		{
			texture.setHorizontalWrapMode(option.horizontalWrapMode);
			texture.setVerticalWrapMode(option.verticalWrapMode);
			texture.setMinFilter(option.minFilter);
			texture.setMagFilter(option.magFilter);
		}
	}
}


Model::~Model()
{
	for (int i = 0; i < meshes.size(); ++i)
	{
		glDeleteVertexArrays(1, &(meshes[i].getVAO()));
		glDeleteBuffers(1, &(meshes[i].getVBO()));
		glDeleteBuffers(1, &(meshes[i].getEBO()));
	}
}