#include "Scene.h"
#include "Engine.h"
#include "Entity.h"
#include "EntityManager.h"

#include <cstdlib>
#include <string>

Scene::Scene() {}

Scene::Scene(const std::string& Name): name{Name} {}

void Scene::OnStartScene()
{
	Entity player = Engine::Instance().AddEntity("player");
	player.addComponent<cTransform>(glm::vec3(0.0f, 0.0f, 4.0f));
	player.getComponent<cTransform>().front = glm::vec3(0.0f, 0.0f, -1.0f);
	player.addComponent<cInput>();
	player.addComponent<cCamera>();
	Engine::Instance().SetMainCamera(&player.getComponent<cCamera>());

	cInput& playerInput = player.getComponent<cInput>();
	playerInput.BindAction(ActionType::MOVE_FORWARD);
	playerInput.BindAction(ActionType::MOVE_BACKWARD);
	playerInput.BindAction(ActionType::STRAFE_LEFT);
	playerInput.BindAction(ActionType::STRAFE_RIGHT);
	playerInput.BindAction(ActionType::MOVE_UP);
	playerInput.BindAction(ActionType::MOVE_DOWN);
	playerInput.BindAction(ActionType::TOGGLE_FLASHLIGHT);

	Entity light = Engine::Instance().AddEntity("lightSource");
	light.addComponent<cPointLight>();
	light.addComponent<cTransform>(glm::vec3(3.0f, 4.0f, -3.0f));
	light.getComponent<cTransform>().scale = glm::vec3(0.2f);

	cPointLight& pointLight = light.getComponent <cPointLight>();

	light.addComponent<cInput>();
	cInput& lightInput = light.getComponent<cInput>();
	lightInput.BindAction(ActionType::MOVE_FORWARD1);
	lightInput.BindAction(ActionType::MOVE_BACKWARD1);
	lightInput.BindAction(ActionType::STRAFE_LEFT1);
	lightInput.BindAction(ActionType::STRAFE_RIGHT1);
	lightInput.BindAction(ActionType::MOVE_DOWN1);
	lightInput.BindAction(ActionType::MOVE_UP1);
	
	for (int i = 0; i < 10; ++i)
	{
		Entity house = Engine::Instance().AddEntity("house" + std::to_string(i));
		house.addComponent<cModel>(Engine::Instance().models[0]);
		house.addComponent<cTransform>(glm::vec3(0.0f + i * 35.0f, 0.0f, 0.0f));
	}

	for (int i = 0; i < 10; ++i)
	{
		Entity house = Engine::Instance().AddEntity("house" + std::to_string(10 + i));
		house.addComponent<cModel>(Engine::Instance().models[0]);
		house.addComponent<cTransform>(glm::vec3(0.0f + i * 35.0f, 0.0f, 35.0f));
		house.getComponent<cTransform>().eulerY = 180.0f;
	}

	for (int i = 0; i < 10; ++i)
	{
		Entity spotlight = Engine::Instance().AddEntity("spotlight" + std::to_string(i));
		spotlight.addComponent<cTransform>(glm::vec3(10.0f + i * 35.0f, 10.0f, 10.0f));
		spotlight.getComponent<cTransform>().front = glm::vec3(0.0f, -1.0f, 0.0f);
		spotlight.addComponent<cSpotLight>();

		Entity spotlight1 = Engine::Instance().AddEntity("spotlight" + std::to_string(10 + i));
		spotlight1.addComponent<cTransform>(glm::vec3(10.0f + i * 35.0f, 10.0f, 20.0f));
		spotlight1.getComponent<cTransform>().front = glm::vec3(0.0f, -1.0f, 0.0f);
		spotlight1.addComponent<cSpotLight>();
	}

	Engine::Instance().AddGlobalLight();

	return;
}

void Scene::BindActions()
{
	Engine::Instance().BindInputKey(GLFW_KEY_W, ActionType::MOVE_FORWARD);
	Engine::Instance().BindInputKey(GLFW_KEY_S, ActionType::MOVE_BACKWARD);
	Engine::Instance().BindInputKey(GLFW_KEY_A, ActionType::STRAFE_LEFT);
	Engine::Instance().BindInputKey(GLFW_KEY_D, ActionType::STRAFE_RIGHT);
	Engine::Instance().BindInputKey(GLFW_KEY_E, ActionType::MOVE_UP);
	Engine::Instance().BindInputKey(GLFW_KEY_Q, ActionType::MOVE_DOWN);
	Engine::Instance().BindInputKey(GLFW_KEY_UP, ActionType::MOVE_FORWARD1);
	Engine::Instance().BindInputKey(GLFW_KEY_DOWN, ActionType::MOVE_BACKWARD1);
	Engine::Instance().BindInputKey(GLFW_KEY_LEFT, ActionType::STRAFE_LEFT1);
	Engine::Instance().BindInputKey(GLFW_KEY_RIGHT, ActionType::STRAFE_RIGHT1);
	Engine::Instance().BindInputKey(GLFW_KEY_P, ActionType::MOVE_UP1);
	Engine::Instance().BindInputKey(GLFW_KEY_O, ActionType::MOVE_DOWN1);
	Engine::Instance().BindInputKey(GLFW_KEY_F, ActionType::TOGGLE_FLASHLIGHT);
	
	return;
}

void Scene::DefineActions(Entity& e, Action& action)
{
	if (e.hasComponent<cTransform>())
	{
		float deltaTime = float(Engine::Instance().deltaTime);
		float inputMoveSpeed = Engine::Instance().inputMoveSpeed;
		cTransform& entityTransform = e.getComponent<cTransform>();

		switch (action.type)
		{
		case ActionType::MOVE_FORWARD:
			if (action.eventType == ActionEventType::CONTINUE)
				addedVelocity += float(deltaTime) * inputMoveSpeed * entityTransform.front;
			break;
		case ActionType::MOVE_BACKWARD:
			if (action.eventType == ActionEventType::CONTINUE)
				addedVelocity -= float(deltaTime) * inputMoveSpeed * entityTransform.front;
			break;
		case ActionType::STRAFE_LEFT:
			if (action.eventType == ActionEventType::CONTINUE)
				addedVelocity -= float(deltaTime) * inputMoveSpeed * entityTransform.right;
			break;
		case ActionType::STRAFE_RIGHT:
			if (action.eventType == ActionEventType::CONTINUE)
				addedVelocity += float(deltaTime) * inputMoveSpeed * entityTransform.right;
			break;
		case ActionType::MOVE_UP:
			if (action.eventType == ActionEventType::CONTINUE)
				addedVelocity += float(deltaTime) * inputMoveSpeed * entityTransform.up;
			break;
		case ActionType::MOVE_DOWN:
			if (action.eventType == ActionEventType::CONTINUE)
				addedVelocity -= float(deltaTime) * inputMoveSpeed * entityTransform.up;
			break;
		case ActionType::MOVE_FORWARD1:
			if (action.eventType == ActionEventType::CONTINUE)
				addedVelocity += float(deltaTime) * inputMoveSpeed * Engine::Instance().GetMainCameraOwner()->getComponent<cTransform>().front;
			break;
		case ActionType::MOVE_BACKWARD1:
			if (action.eventType == ActionEventType::CONTINUE)
				addedVelocity -= float(deltaTime) * inputMoveSpeed * Engine::Instance().GetMainCameraOwner()->getComponent<cTransform>().front;
			break;
		case ActionType::STRAFE_LEFT1:
			if (action.eventType == ActionEventType::CONTINUE)
				addedVelocity -= float(deltaTime) * inputMoveSpeed * Engine::Instance().GetMainCameraOwner()->getComponent<cTransform>().right;
			break;
		case ActionType::STRAFE_RIGHT1:
			if (action.eventType == ActionEventType::CONTINUE)
				addedVelocity += float(deltaTime) * inputMoveSpeed * Engine::Instance().GetMainCameraOwner()->getComponent<cTransform>().right;
			break;
		case ActionType::MOVE_UP1:
			if (action.eventType == ActionEventType::CONTINUE)
				addedVelocity += float(deltaTime) * inputMoveSpeed * Engine::Instance().GetMainCameraOwner()->getComponent<cTransform>().up;
			break;
		case ActionType::MOVE_DOWN1:
			if (action.eventType == ActionEventType::CONTINUE)
				addedVelocity -= float(deltaTime) * inputMoveSpeed * Engine::Instance().GetMainCameraOwner()->getComponent<cTransform>().up;
			break;
		}
		entityTransform.velocity = addedVelocity;
	}

	if (action.type == ActionType::TOGGLE_FLASHLIGHT && action.eventType == ActionEventType::BEGIN)
	{
		if (e.hasComponent<cSpotLight>())
		{
			e.removeComponent<cSpotLight>();
		}
		else
		{
			e.addComponent<cSpotLight>();
		}
	}
}

void Scene::OnUpdate()
{
	addedVelocity = glm::vec3(0.0f);
}