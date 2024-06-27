#pragma once

#include "Texture2D.h"

class Framebuffer
{
public:
	Framebuffer(unsigned int Width, unsigned int Height);

	void      Generate();
	void      SetBufferType(GLenum type);
	GLenum    GetBufferType() const;
	void      Bind();
	void      Unbind();
	GLenum    CheckStatus() const;
	Texture2D GetColorBuffer() const;

	~Framebuffer();
private:
	unsigned int ID;
	Texture2D    colorBuffer;
	unsigned int renderBuffer;
	GLenum       bufferType = GL_FRAMEBUFFER;
	unsigned int width;
	unsigned int height;
	GLenum		 status;
};

