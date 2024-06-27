#include "Framebuffer.h"
#include <iostream>

Framebuffer::Framebuffer(unsigned int Width, unsigned int Height)
	: width{Width}, height{Height}
{}

void Framebuffer::Generate()
{
	// Generate buffer.
	glGenFramebuffers(1, &ID);
	// Bind buffer.
	glBindFramebuffer(bufferType, ID);
	// Generate, bind, allocate and attach color buffer.
	colorBuffer.CreateAsBuffer(bufferType, width, height);
	// Generate, bind allocate and attach render buffer.
	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(bufferType, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
	// Check status.
	status = glCheckFramebufferStatus(bufferType);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER::Framebuffer with ID: " << ID << " is not complete." << std::endl;
	// Unbind buffer.
	glBindFramebuffer(bufferType, 0);
}

void Framebuffer::SetBufferType(GLenum type)
{
	bufferType = type;
}

GLenum Framebuffer::GetBufferType() const
{
	return bufferType;
}

GLenum Framebuffer::CheckStatus() const
{
	return status;
}

void Framebuffer::Bind()
{
	glBindFramebuffer(bufferType, ID);
}

void Framebuffer::Unbind()
{
	glBindFramebuffer(bufferType, 0);
}

Texture2D Framebuffer::GetColorBuffer() const
{
	return colorBuffer;
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &ID);
	glDeleteRenderbuffers(1, &renderBuffer);
	glDeleteTextures(1, &colorBuffer.ID);
}