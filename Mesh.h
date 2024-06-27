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

	const unsigned int& getVAO() const;
	const unsigned int& getVBO() const;
	const unsigned int& getEBO() const;

private:
	unsigned int VAO, VBO, EBO;

	void setupMesh();
};

