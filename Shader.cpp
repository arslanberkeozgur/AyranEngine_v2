
#include "Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader()
{}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
	load(vertexPath, fragmentPath);
}

void Shader::load(const std::string& vertexPath, const std::string& fragmentPath)
{
	std::ifstream vertexIFS;
	vertexIFS.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	std::ostringstream vertexOSS;

	try {
		vertexIFS.open(vertexPath);
		vertexOSS << vertexIFS.rdbuf();
		vertexIFS.close();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::OPENING_VERTEX_SHADER_SOURCE_CODE" << std::endl;
	}

	std::ifstream fragmentIFS;
	fragmentIFS.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	std::ostringstream fragmentOSS;

	try {
		fragmentIFS.open(fragmentPath);
		fragmentOSS << fragmentIFS.rdbuf();
		fragmentIFS.close();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::OPENING_FRAGMENT_SHADER_SOURCE_CODE" << std::endl;
	}

	generateShaderProgram(vertexOSS.str().c_str(), fragmentOSS.str().c_str());
}

void Shader::generateShaderProgram(const char* vertexSource, const char* fragmentSource)
{
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::COMPILING_VERTEX_SHADER : " << std::endl << infoLog << std::endl;
	}

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::COMPILING_FRAGMENT_SHADER : " << std::endl << infoLog << std::endl;
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::LINKING_VERTEX_AND_FRAGMENT_SHADERS : " << std::endl << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	std::cout << "Shader with ID " << ID << " was succesfully loaded." << std::endl;
}

void Shader::use() const
{
	glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), int(value));
}

void Shader::setuInt(const std::string& name, unsigned int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFVector(const std::string& name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setFVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setFMat4(const std::string& name, glm::mat4& mat4)
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat4));
}

void Shader::setFVec3(const std::string& name, const glm::vec3& vec)
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y, vec.z);
}

void Shader::setFMat3(const std::string& name, glm::mat3& mat3)
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat3));
}