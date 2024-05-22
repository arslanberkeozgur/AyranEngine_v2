#pragma once

#include <string>
#include <glm/glm.hpp>

class Entity;
struct Action;

class Scene
{
public:
	Scene();
	explicit Scene(const std::string& Name);

public:
	void OnStartScene();
	void BindActions();
	void OnUpdate();
	void DefineActions(Entity& e, Action& action);

public:
	std::string name;

private:
	glm::vec3 addedVelocity{ 0.0f, 0.0f, 0.0f };
};

