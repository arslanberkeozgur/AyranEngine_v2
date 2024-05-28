#pragma once

#include <string>
#include <glm/glm.hpp>

class Shader
{
public:
	
	// Program ID.
	unsigned int ID = -1;
	
	Shader();
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	void load(const std::string& vertexPath, const std::string& fragmentPath);
	// Use shader.
	void use() const;
	// Utility uniform functions.
	void setBool(const std::string& name, bool value) const;
	void setuInt(const std::string& name, unsigned int value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setFVector(const std::string& name, float x, float y, float z, float w) const;
	void setFVec3(const std::string& name, float x, float y, float z) const;
	void setFVec3(const std::string& name, const glm::vec3& vec);
	void setFMat4(const std::string& name, glm::mat4& mat4);
	void setFMat3(const std::string& name, glm::mat3& mat3);

private:
	void generateShaderProgram(const char* vertexSource, const char* fragmentSource);

};

