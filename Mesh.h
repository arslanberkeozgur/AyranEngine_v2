#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <string>

class Shader;

enum class TextureType
{
	DIFFUSE,
	SPECULAR
};

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TextureCoords;
};

struct Texture
{
	unsigned int id;
	TextureType type;
	std::string path;
};

class Mesh
{
public:
	std::vector<Vertex>			vertices;
	std::vector<unsigned int>	indices;
	std::vector<Texture>		textures;

	Mesh(std::vector<Vertex> Vertices, std::vector<unsigned int> Indices, std::vector<Texture> Textures);
	
	void Draw(Shader& shader);

private:
	unsigned int VAO, VBO, EBO;

	void setupMesh();
};

