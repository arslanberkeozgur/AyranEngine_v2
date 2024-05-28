#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <string>
#include "Texture2D.h"

class Shader;

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TextureCoords;
};

class Mesh
{
public:
	std::vector<Vertex>			vertices;
	std::vector<unsigned int>	indices;
	std::vector<Texture2D>		textures;

	Mesh(std::vector<Vertex> Vertices, std::vector<unsigned int> Indices, std::vector<Texture2D> Textures);
	
	void Draw(Shader& shader);

	unsigned int& getVAO();
	unsigned int& getVBO();
	unsigned int& getEBO();

private:
	unsigned int VAO, VBO, EBO;

	void setupMesh();
};

