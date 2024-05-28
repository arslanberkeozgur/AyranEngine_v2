#pragma once

#include <string>
#include <glad/glad.h>
#include <glfw/glfw3.h>

class Shader;

enum class TextureType
{
	DIFFUSE,
	SPECULAR
};

class Texture2D
{
public:
	Texture2D();
	bool load(const std::string& path);

	unsigned int ID;
	TextureType type;
	std::string path;

	void use(unsigned int i) const;

	void setHorizontalWrapMode(GLenum mode);
	void setVerticalWrapMode(GLenum mode);
	void setMinFilter(GLenum filter);
	void setMagFilter(GLenum filter);

private:
	GLenum horizontalWrapMode;
	GLenum verticalWrapMode;
	GLenum minFilter;
	GLenum magFilter;
};