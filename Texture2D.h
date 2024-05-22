#pragma once

#include <string>
#include <glad/glad.h>
#include <glfw/glfw3.h>

class Shader;

class Texture2D
{
public:
	Texture2D(std::string nname);
	bool load(const std::string& path);

	std::string name;
	unsigned int ID;

	void use() const;

	void setSpecularUnit(const Shader& shader, unsigned int unit);
	void setDiffuseUnit(const Shader& shader, unsigned int unit);
	void setEmissionUnit(const Shader& shader, unsigned int unit);
	void setHorizontalWrapMode(GLenum mode);
	void setVerticalWrapMode(GLenum mode);
	void setMinFilter(GLenum filter);
	void setMagFilter(GLenum filter);

	unsigned int getTextureUnit() const;

private:
	unsigned int textureUnit;
	GLenum horizontalWrapMode;
	GLenum verticalWrapMode;
	GLenum minFilter;
	GLenum magFilter;
};