#include "Mesh.h"
#include "Shader.h"
#include "glad/glad.h"
#include "Engine.h"

#include <iostream>

Mesh::Mesh(std::vector<Vertex> Vertices,std::vector<unsigned int> Indices, std::vector<Texture2D> Textures)
	: vertices{ Vertices }, indices{ Indices }, textures{ Textures }
{
	setupMesh();

	for (int i = 0; i < textures.size(); ++i)
	{
		textures[i].textureID = i;
	}
}

void Mesh::setupMesh()
{
	// Generate VAO, VBO and EBO.
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// Load VBO data.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	// Load EBO data.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Normal));

	// Vertex Texture Coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::TextureCoords));

	// Unbind VAO.
	glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader)
{
	unsigned int diffuseNumber = 1;
	unsigned int specularNumber = 1;
	for (unsigned int i = 0; i < textures.size(); ++i)
	{
		std::string name, number;

		switch (textures[i].type)
		{
		case TextureType::DIFFUSE:
			name = "texture_diffuse";
			number = diffuseNumber++;
			break;
		case TextureType::SPECULAR:
			name = "texture_specular";
			number = specularNumber++;
			break;
		}

		shader.setuInt(("material." + name + number).c_str(), i);
		textures[i].use();
	}

	if (textures.size() == 0)
	{
		// Use default texture if the mesh contains no textures.
		shader.setuInt("material.diffuse1", 0);
		Engine::Instance().defaultTexture.use();
	}

	// This sets the active texture to default so that in future we get nothing unexpected.
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, GLint(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

unsigned int& Mesh::getVAO()
{
	return VAO;
}

unsigned int& Mesh::getVBO()
{
	return VBO;
}

unsigned int& Mesh::getEBO()
{
	return EBO;
}