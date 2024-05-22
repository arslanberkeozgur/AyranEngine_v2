#include "Scene.h"
#include "Engine.h"
#include "Entity.h"
#include "EntityManager.h"

#include <cstdlib>

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
	//player.addComponent<cPointLight>();

	cInput& playerInput = player.getComponent<cInput>();
	playerInput.BindAction(Action_Type::MOVE_FORWARD);
	playerInput.BindAction(Action_Type::MOVE_BACKWARD);
	playerInput.BindAction(Action_Type::STRAFE_LEFT);
	playerInput.BindAction(Action_Type::STRAFE_RIGHT);
	playerInput.BindAction(Action_Type::MOVE_UP);
	playerInput.BindAction(Action_Type::MOVE_DOWN);
	playerInput.BindAction(Action_Type::TOGGLE_FLASHLIGHT);

	//Entity e1 = Engine::Instance().AddEntity("entity1");
	//Entity e2 = Engine::Instance().AddEntity("entity2");
	//Entity e3 = Engine::Instance().AddEntity("entity3");
	//Entity e4 = Engine::Instance().AddEntity("entity4");
	//Entity e5 = Engine::Instance().AddEntity("entity5");
	//e1.addComponent<cTransform>(glm::vec3(-2.0f, 1.0f, -1.0f));
	//e1.addComponent<cVAO>(Engine::Instance().containerVAO);
	//e2.addComponent<cTransform>(glm::vec3(2.0f, -1.0f, -1.0f));
	//e2.addComponent<cVAO>(Engine::Instance().containerVAO);
	//e3.addComponent<cTransform>(glm::vec3(0.0f, 2.0f, -3.0f));
	//e3.addComponent<cVAO>(Engine::Instance().containerVAO);
	//e4.addComponent<cTransform>(glm::vec3(-5.0f, 4.0f, 2.0f));
	//e4.addComponent<cVAO>(Engine::Instance().containerVAO);
	//e5.addComponent<cTransform>(glm::vec3(1.0f, -4.0f, -1.0f));
	//e5.addComponent<cVAO>(Engine::Instance().containerVAO);

	Entity light = Engine::Instance().AddEntity("lightSource");
	light.addComponent<cPointLight>();
	light.addComponent<cTransform>(glm::vec3(3.0f, 4.0f, -3.0f));
	light.getComponent<cTransform>().scale = glm::vec3(0.2f);
	light.addComponent<cVAO>(Engine::Instance().containerVAO);

	cPointLight& pointLight = light.getComponent <cPointLight>();

	//Entity light1 = Engine::Instance().AddEntity("lightSource1");
	//light1.addComponent<cPointLight>();
	//light1.addComponent<cTransform>(glm::vec3(-3.0f, -2.0f, 3.0f));
	//light1.getComponent<cTransform>().scale = glm::vec3(0.2f);
	//light1.addComponent<cVAO>(Engine::Instance().lightVAO);

	light.addComponent<cInput>();
	cInput& lightInput = light.getComponent<cInput>();
	lightInput.BindAction(Action_Type::MOVE_FORWARD1);
	lightInput.BindAction(Action_Type::MOVE_BACKWARD1);
	lightInput.BindAction(Action_Type::STRAFE_LEFT1);
	lightInput.BindAction(Action_Type::STRAFE_RIGHT1);
	lightInput.BindAction(Action_Type::MOVE_DOWN1);
	lightInput.BindAction(Action_Type::MOVE_UP1);
	
	Entity guitar = Engine::Instance().AddEntity("guitar");
	guitar.addComponent<cModel>(Engine::Instance().models[0]);
	guitar.addComponent<cTransform>(glm::vec3(0.0f, 0.0f, 0.0f));
	guitar.getComponent<cTransform>().scale = glm::vec3(0.1f);

	//Engine::Instance().AddGlobalLight();

	return;
}

void Scene::BindActions()
{
	Engine::Instance().BindInputKey(GLFW_KEY_W, Action_Type::MOVE_FORWARD);
	Engine::Instance().BindInputKey(GLFW_KEY_S, Action_Type::MOVE_BACKWARD);
	Engine::Instance().BindInputKey(GLFW_KEY_A, Action_Type::STRAFE_LEFT);
	Engine::Instance().BindInputKey(GLFW_KEY_D, Action_Type::STRAFE_RIGHT);
	Engine::Instance().BindInputKey(GLFW_KEY_E, Action_Type::MOVE_UP);
	Engine::Instance().BindInputKey(GLFW_KEY_Q, Action_Type::MOVE_DOWN);
	Engine::Instance().BindInputKey(GLFW_KEY_UP, Action_Type::MOVE_FORWARD1);
	Engine::Instance().BindInputKey(GLFW_KEY_DOWN, Action_Type::MOVE_BACKWARD1);
	Engine::Instance().BindInputKey(GLFW_KEY_LEFT, Action_Type::STRAFE_LEFT1);
	Engine::Instance().BindInputKey(GLFW_KEY_RIGHT, Action_Type::STRAFE_RIGHT1);
	Engine::Instance().BindInputKey(GLFW_KEY_P, Action_Type::MOVE_UP1);
	Engine::Instance().BindInputKey(GLFW_KEY_O, Action_Type::MOVE_DOWN1);
	Engine::Instance().BindInputKey(GLFW_KEY_F, Action_Type::TOGGLE_FLASHLIGHT);
	
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
		case Action_Type::MOVE_FORWARD:
			if (action.eventType == Action_Event_Type::CONTINUE)
				addedVelocity += float(deltaTime) * inputMoveSpeed * entityTransform.front;
			break;
		case Action_Type::MOVE_BACKWARD:
			if (action.eventType == Action_Event_Type::CONTINUE)
				addedVelocity -= float(deltaTime) * inputMoveSpeed * entityTransform.front;
			break;
		case Action_Type::STRAFE_LEFT:
			if (action.eventType == Action_Event_Type::CONTINUE)
				addedVelocity -= float(deltaTime) * inputMoveSpeed * entityTransform.right;
			break;
		case Action_Type::STRAFE_RIGHT:
			if (action.eventType == Action_Event_Type::CONTINUE)
				addedVelocity += float(deltaTime) * inputMoveSpeed * entityTransform.right;
			break;
		case Action_Type::MOVE_UP:
			if (action.eventType == Action_Event_Type::CONTINUE)
				addedVelocity += float(deltaTime) * inputMoveSpeed * entityTransform.up;
			break;
		case Action_Type::MOVE_DOWN:
			if (action.eventType == Action_Event_Type::CONTINUE)
				addedVelocity -= float(deltaTime) * inputMoveSpeed * entityTransform.up;
			break;
		case Action_Type::MOVE_FORWARD1:
			if (action.eventType == Action_Event_Type::CONTINUE)
				addedVelocity += float(deltaTime) * inputMoveSpeed * Engine::Instance().GetMainCameraOwner()->getComponent<cTransform>().front;
			break;
		case Action_Type::MOVE_BACKWARD1:
			if (action.eventType == Action_Event_Type::CONTINUE)
				addedVelocity -= float(deltaTime) * inputMoveSpeed * Engine::Instance().GetMainCameraOwner()->getComponent<cTransform>().front;
			break;
		case Action_Type::STRAFE_LEFT1:
			if (action.eventType == Action_Event_Type::CONTINUE)
				addedVelocity -= float(deltaTime) * inputMoveSpeed * Engine::Instance().GetMainCameraOwner()->getComponent<cTransform>().right;
			break;
		case Action_Type::STRAFE_RIGHT1:
			if (action.eventType == Action_Event_Type::CONTINUE)
				addedVelocity += float(deltaTime) * inputMoveSpeed * Engine::Instance().GetMainCameraOwner()->getComponent<cTransform>().right;
			break;
		case Action_Type::MOVE_UP1:
			if (action.eventType == Action_Event_Type::CONTINUE)
				addedVelocity += float(deltaTime) * inputMoveSpeed * Engine::Instance().GetMainCameraOwner()->getComponent<cTransform>().up;
			break;
		case Action_Type::MOVE_DOWN1:
			if (action.eventType == Action_Event_Type::CONTINUE)
				addedVelocity -= float(deltaTime) * inputMoveSpeed * Engine::Instance().GetMainCameraOwner()->getComponent<cTransform>().up;
			break;
		}
		entityTransform.velocity = addedVelocity;
	}

	if (action.type == Action_Type::TOGGLE_FLASHLIGHT && action.eventType == Action_Event_Type::BEGIN)
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