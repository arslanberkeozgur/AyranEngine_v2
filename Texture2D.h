#pragma once

#include <string>
#include <glad/glad.h>
#include <glfw/glfw3.h>

class Shader;
struct TextureInfo;

enum class TextureType
{
	DIFFUSE,
	SPECULAR
};

class Texture2D
{
public:
	Texture2D();
	bool load(const std::string& path, TextureInfo& info);

	unsigned int ID;
	unsigned int textureID = 0;
	TextureType  type;
	std::string  path;

	void use() const;

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

struct TextureInfo
{
	bool alphaChannel = false;
};