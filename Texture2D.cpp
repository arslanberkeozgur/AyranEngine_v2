#include "Texture2D.h"
#include "stb_image.h"
#include "Shader.h"

#include <iostream>
#include <sstream>

Texture2D::Texture2D(std::string nname)
	: textureUnit{GL_TEXTURE0}, name{nname}, horizontalWrapMode{GL_REPEAT}, verticalWrapMode{GL_REPEAT}, 
	minFilter{GL_LINEAR_MIPMAP_LINEAR}, magFilter{GL_LINEAR}
{
	stbi_set_flip_vertically_on_load(true);
}

bool Texture2D::load(const std::string& path)
{
	// Generate and bind texture.
	glGenTextures(1, &ID);
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, ID);

	// Set texture parameters.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, horizontalWrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, verticalWrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

	// Load data.
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if (!data)
	{
		std::cerr << "Failed to load texture from path '" << path << "'" << std::endl;
		return false;
	}
	if (nrChannels == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else if (nrChannels == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	else
		std::cout << "Extension not recognized." << std::endl;

	// Generate mipmap.
	glGenerateMipmap(GL_TEXTURE_2D);

	std::cout << "Texture '" << name << "' has been succesfully loaded." << std::endl;

	// Free resources.
	stbi_image_free(data);

	return true;
}

void Texture2D::use() const
{
	GLenum texUnit = GL_TEXTURE0 + textureUnit;
	glActiveTexture(texUnit);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture2D::setSpecularUnit(const Shader& shader, unsigned int unit)
{
	// Naming convention in the shader: "Texture"(unit), e.g. if unit == 0, name the sampler as Texture0. 
	textureUnit = unit;
	shader.use();
	shader.setuInt("material.specular", unit);
}

void Texture2D::setDiffuseUnit(const Shader& shader, unsigned int unit)
{
	textureUnit = unit;
	shader.use();
	shader.setuInt("material.diffuse", unit);
}

void Texture2D::setEmissionUnit(const Shader& shader, unsigned int unit)
{
	textureUnit = unit;
	shader.use();
	shader.setuInt("material.emission", unit);
}

void Texture2D::setHorizontalWrapMode(GLenum mode)
{
	horizontalWrapMode = mode;
}

void Texture2D::setVerticalWrapMode(GLenum mode)
{
	verticalWrapMode = mode;
}

void Texture2D::setMinFilter(GLenum filter)
{
	minFilter = filter;
}

void Texture2D::setMagFilter(GLenum filter)
{
	magFilter = filter;
}

unsigned int Texture2D::getTextureUnit() const
{
	return textureUnit;
}