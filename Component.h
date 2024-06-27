#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <string>
#include <memory>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Enums.h"
#include "Shader.h"
#include "Input.h"
#include "Model.h"

enum class ComponentType
{
	NULL_TYPE,
	TRANSFORM,
	INPUT,
	CAMERA,
	SHADER,
	POINT_LIGHT,
	SPOT_LIGHT,
	MODEL
};

struct Component
{
	bool active = false;
	size_t ownerID = -1;

	// hack! for now.
	ComponentType type = ComponentType::NULL_TYPE;

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
	glm::quat   orientation		= { glm::vec3(0.0, 0.0, 0.0) };

	cTransform()
	{
		right = glm::normalize(glm::cross(front, up));
		type = ComponentType::TRANSFORM;
	}
	cTransform(glm::vec3 pos, glm::vec3 front, glm::vec3 up, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f))
		: position{ pos }, front{ front }, up{ up }, scale{ scale }
	{
		right = glm::normalize(glm::cross(front, up));
		type = ComponentType::TRANSFORM;
	}
	explicit cTransform(glm::vec3 pos)
		: position{ pos }
	{
		right = glm::normalize(glm::cross(front, up));
		type = ComponentType::TRANSFORM;
	}
};

struct cInput : Component
{
	std::vector<ActionType> actions;
	cInput() 
	{
		type = ComponentType::INPUT;
	}

	void BindAction(ActionType type)
	{
		actions.push_back(type);
	}
	void UnbindAction(ActionType type)
	{
		auto result = std::remove_if(actions.begin(), actions.end(), [type](ActionType& elem) {return elem == type; });
		actions.erase(result, actions.end());
	}
};

struct cCamera : Component
{
	float		yaw				 = -90.0f;
	float		pitch			 = 0.0f;
	glm::vec3	relativePosition = glm::vec3(0.0f, 0.0f, 0.0f);

	cCamera() 
	{
		type = ComponentType::CAMERA;
	}
};

struct cShader : Component
{
	Shader shader;

	cShader() 
	{
		type = ComponentType::SHADER;
	}
	cShader(Shader Shader) : shader{ Shader } 
	{
		type = ComponentType::SHADER;
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
		type = ComponentType::POINT_LIGHT;
	}
	cPointLight(glm::vec3& Ambient, glm::vec3& Diffuse, glm::vec3& Specular, float Constant = 1.0f, float Linear = 0.09f, float Quadratic = 0.032f)
		: ambient{ Ambient }, diffuse{ Diffuse }, specular{ Specular }, constant{ Constant }, linear{ Linear }, quadratic{ Quadratic } 
	{
		type = ComponentType::POINT_LIGHT;
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
		type = ComponentType::SPOT_LIGHT;
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
		type = ComponentType::SPOT_LIGHT;
	}
};

struct cModel : Component
{
	std::shared_ptr<Model> model		= nullptr;
	bool				   isOutlined   = false;
	glm::vec3			   outlineColor = { 0.0f, 0.0f, 0.0f };
	
	cModel() 
	{
		type = ComponentType::MODEL;
	}
	cModel(std::shared_ptr<Model> Model) : model{ Model } 
	{
		type = ComponentType::MODEL;
	}
};