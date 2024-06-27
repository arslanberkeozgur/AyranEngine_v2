#include "Texture2D.h"
#include "stb_image.h"
#include "Shader.h"

#include <iostream>
#include <sstream>


Texture2D::Texture2D()
	: horizontalWrapMode{GL_REPEAT}, verticalWrapMode{ GL_REPEAT },
	minFilter{GL_LINEAR_MIPMAP_LINEAR}, magFilter{GL_LINEAR}
{}

bool Texture2D::load(const std::string& path, TextureInfo& info)
{
	this->path = path;

	// Generate and bind texture.
	glGenTextures(1, &ID);
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

	GLenum format;

	switch (nrChannels)
	{
	case 1:
		format = GL_RED;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		info.alphaChannel = true;
		break;
	default:
		std::cout << "Texture of unknown format at path : " << path << std::endl;
		stbi_image_free(data);
		return false;
	}
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	// Generate mipmap.
	glGenerateMipmap(GL_TEXTURE_2D);

	std::cout << "Texture at path: '" << path << "' has been succesfully loaded." << std::endl;

	// Free resources.
	stbi_image_free(data);

	return true;
}

void Texture2D::use() const
{
	glActiveTexture(GL_TEXTURE0 + textureID);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture2D::setHorizontalWrapMode(GLenum mode)
{
	horizontalWrapMode = mode;
	use();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, horizontalWrapMode);
}

void Texture2D::setVerticalWrapMode(GLenum mode)
{
	verticalWrapMode = mode;
	use();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, verticalWrapMode);
}

void Texture2D::setMinFilter(GLenum filter)
{
	minFilter = filter;
	use();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
}

void Texture2D::setMagFilter(GLenum filter)
{
	magFilter = filter;
	use();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture2D::CreateAsBuffer(GLenum bufferType, unsigned int width, unsigned int height)
{
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(bufferType, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID, 0);
}