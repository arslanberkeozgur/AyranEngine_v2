#pragma once


#include <string>
#include <memory>

#include "Shader.h"
#include "Input.h"
#include "Model.h"

enum class Component_Type
{
	NULL_TYPE,
	TRANSFORM,
	INPUT,
	CAMERA,
	SHADER,
	VAO,
	POINT_LIGHT,
	SPOT_LIGHT,
	MODEL
};

struct Component
{
	bool active = false;
	size_t ownerID = -1;

	// hack! for now.
	Component_Type type = Component_Type::NULL_TYPE;

	Component() {}
};

struct cTransform : Component
{
	bool		allowRotation	= true;
	glm::vec3	position		= { 0.0f, 0.0f, 0.0f };
	glm::vec3	velocity		= { 0.0f, 0.0f, 0.0f };
	glm::vec3	scale			= { 1.0f, 1.0f, 1.0f };
	glm::vec3	front			= { 0.0f, 0.0f, 1.0f };
	glm::vec3	up				= { 0.0f, 1.0f, 0.0f };
	glm::vec3	right;
	float		yaw				= -90.0f;
	float		pitch			= 0.0f;

	cTransform()
	{
		right = glm::normalize(glm::cross(front, up));
		type = Component_Type::TRANSFORM;
	}
	cTransform(glm::vec3 pos, glm::vec3 front, glm::vec3 up, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f))
		: position{ pos }, front{ front }, up{ up }, scale{ scale }
	{
		right = glm::normalize(glm::cross(front, up));
		type = Component_Type::TRANSFORM;
	}
	explicit cTransform(glm::vec3 pos)
		: position{ pos }
	{
		right = glm::normalize(glm::cross(front, up));
		type = Component_Type::TRANSFORM;
	}
};

struct cInput : Component
{
	std::vector<Action_Type> actions;
	cInput() 
	{
		type = Component_Type::INPUT;
	}

	void BindAction(Action_Type type)
	{
		actions.push_back(type);
	}
	void UnbindAction(Action_Type type)
	{
		auto result = std::remove_if(actions.begin(), actions.end(), [type](Action_Type& elem) {return elem == type; });
		actions.erase(result, actions.end());
	}
};

struct cCamera : Component
{
	bool		isMainCamera = true;
	glm::vec3	relativePosition = glm::vec3(0.0f, 0.0f, 0.0f);

	cCamera() 
	{
		type = Component_Type::CAMERA;
	}
};

struct cShader : Component
{
	std::string name;

	cShader() 
	{
		type = Component_Type::SHADER;
	}
	cShader(const std::string& Name) : name{ Name } 
	{
		type = Component_Type::SHADER;
	}
};

struct cVAO : Component
{
	unsigned int VAO = -1;
	cVAO() 
	{
		type = Component_Type::VAO;
	}
	cVAO(unsigned int vao) : VAO{ vao } 
	{
		type = Component_Type::VAO;
	}
};

struct cPointLight : Component
{
	float		constant	= 1.0f;
	float		linear		= 0.09f;
	float		quadratic	= 0.032f;

	glm::vec3	ambient		{ 0.3f, 0.3f, 0.3f };
	glm::vec3	diffuse		{ 1.0f, 1.0f, 1.0f };
	glm::vec3	specular	{ 1.0f, 1.0f, 1.0f };

	cPointLight() 
	{ 
		type = Component_Type::POINT_LIGHT;
	}
	cPointLight(glm::vec3& Ambient, glm::vec3& Diffuse, glm::vec3& Specular, float Constant = 1.0f, float Linear = 0.09f, float Quadratic = 0.032f)
		: ambient{ Ambient }, diffuse{ Diffuse }, specular{ Specular }, constant{ Constant }, linear{ Linear }, quadratic{ Quadratic } 
	{
		type = Component_Type::POINT_LIGHT;
	}
};

struct cSpotLight : Component
{
	float		cutoff			= glm::cos(glm::radians(12.5f));
	float		outerCutoff		= glm::cos(glm::radians(20.0f));
	float		constant		= 1.0f;
	float		linear			= 0.09f;
	float		quadratic		= 0.032f;

	glm::vec3	ambient			{ 0.2f, 0.2f, 0.2f };
	glm::vec3	diffuse			{ 1.0f, 1.0f, 1.0f };
	glm::vec3	specular		{ 1.0f, 1.0f, 1.0f };

	cSpotLight()
	{
		type = Component_Type::SPOT_LIGHT;
	}
	cSpotLight
	(
		glm::vec3& Ambient, glm::vec3& Diffuse, glm::vec3& Specular, 
		float Cutoff = glm::cos(glm::radians(12.5f)), float OuterCutoff = glm::cos(glm::radians(20.0f)),
		float Constant = 1.0f, float Linear = 0.09f, float Quadratic = 0.032f
	)
		: ambient{ Ambient }, diffuse{ Diffuse }, specular{ Specular }, cutoff{ Cutoff }, outerCutoff{ OuterCutoff },
		  constant{Constant}, linear{Linear}, quadratic{Quadratic}
	{
		type = Component_Type::SPOT_LIGHT;
	}
};

struct cModel : Component
{
	std::shared_ptr<Model> model = nullptr;
	
	cModel() {}
	cModel(std::shared_ptr<Model> Model) : model{ Model } {}
};