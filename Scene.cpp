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
	player.addComponent<cTransform>(glm::vec3(10.0f, 2.0f, 4.0f));
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
	playerInput.BindAction(ActionType::RUN);
	playerInput.BindAction(ActionType::SELECT_0);
	playerInput.BindAction(ActionType::SELECT_1);
	playerInput.BindAction(ActionType::SELECT_2);
	playerInput.BindAction(ActionType::SELECT_3);
	playerInput.BindAction(ActionType::SELECT_4);
	playerInput.BindAction(ActionType::SELECT_5);
	playerInput.BindAction(ActionType::SELECT_6);

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
		house.addComponent<cModel>(Engine::Instance().models["house"]);
		house.addComponent<cTransform>(glm::vec3(0.0f + i * 35.0f, 0.0f, 0.0f));
	}

	Entity ashtray = Engine::Instance().AddEntity("ashtray");
	ashtray.addComponent<cModel>(Engine::Instance().models["ashtray"]);
	ashtray.addComponent<cTransform>(glm::vec3(1.0f, 1.4f, 2.0f));

	Entity glass = Engine::Instance().AddEntity("glass");
	glass.addComponent<cModel>(Engine::Instance().models["transparent_window"]);
	glass.addComponent<cTransform>(glm::vec3(10.0f, 2.0f, 10.0f));

	Engine::Instance().OutlineEntity(ashtray, glm::vec3(0.0f, 0.5f, 0.8f));

	Entity temple = Engine::Instance().AddEntity("temple");
	temple.addComponent<cModel>(Engine::Instance().models["commodore"]);
	temple.addComponent<cTransform>(glm::vec3(0.0f, 0.0f, 30.0f));
	Engine::Instance().AddLocalRotation(temple, glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);

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
	Engine::Instance().BindInputKey(GLFW_KEY_LEFT_SHIFT, ActionType::RUN);
	Engine::Instance().BindInputKey(GLFW_KEY_KP_0, ActionType::SELECT_0);
	Engine::Instance().BindInputKey(GLFW_KEY_KP_1, ActionType::SELECT_1);
	Engine::Instance().BindInputKey(GLFW_KEY_KP_2, ActionType::SELECT_2);
	Engine::Instance().BindInputKey(GLFW_KEY_KP_3, ActionType::SELECT_3);
	Engine::Instance().BindInputKey(GLFW_KEY_KP_4, ActionType::SELECT_4);
	Engine::Instance().BindInputKey(GLFW_KEY_KP_5, ActionType::SELECT_5);
	Engine::Instance().BindInputKey(GLFW_KEY_KP_6, ActionType::SELECT_6);

	return;
}

void Scene::DefineActions(Entity& e, Action& action)
{
	if (e.hasComponent<cTransform>())
	{
		float deltaTime = float(Engine::Instance().deltaTime);
		cTransform& entityTransform = e.getComponent<cTransform>();

		switch (action.type)
		{
		case ActionType::MOVE_FORWARD:
			if (action.eventType == ActionEventType::CONTINUE)
				Engine::Instance().ApplyVelocity(e, float(deltaTime) * moveSpeed * entityTransform.front);
			break;
		case ActionType::MOVE_BACKWARD:
			if (action.eventType == ActionEventType::CONTINUE)
				Engine::Instance().ApplyVelocity(e, -float(deltaTime) * moveSpeed * entityTransform.front);
			break;
		case ActionType::STRAFE_LEFT:
			if (action.eventType == ActionEventType::CONTINUE)
				Engine::Instance().ApplyVelocity(e, -float(deltaTime) * moveSpeed * entityTransform.right);
			break;
		case ActionType::STRAFE_RIGHT:
			if (action.eventType == ActionEventType::CONTINUE)
				Engine::Instance().ApplyVelocity(e, float(deltaTime) * moveSpeed * entityTransform.right);
			break;
		case ActionType::MOVE_UP:
			if (action.eventType == ActionEventType::CONTINUE)
				Engine::Instance().ApplyVelocity(e, float(deltaTime) * moveSpeed * entityTransform.up);
			break;
		case ActionType::MOVE_DOWN:
			if (action.eventType == ActionEventType::CONTINUE)
				Engine::Instance().ApplyVelocity(e, -float(deltaTime) * moveSpeed * entityTransform.up);
			break;
		case ActionType::MOVE_FORWARD1:
			if (action.eventType == ActionEventType::CONTINUE)
				Engine::Instance().ApplyVelocity(e, float(deltaTime) * moveSpeed * Engine::Instance().GetMainCameraOwner()->getComponent<cTransform>().front);
			break;
		case ActionType::MOVE_BACKWARD1:
			if (action.eventType == ActionEventType::CONTINUE)
				Engine::Instance().ApplyVelocity(e, -float(deltaTime) * moveSpeed * Engine::Instance().GetMainCameraOwner()->getComponent<cTransform>().front);
			break;
		case ActionType::STRAFE_LEFT1:
			if (action.eventType == ActionEventType::CONTINUE)
				Engine::Instance().ApplyVelocity(e, -float(deltaTime) * moveSpeed * Engine::Instance().GetMainCameraOwner()->getComponent<cTransform>().right);
			break;
		case ActionType::STRAFE_RIGHT1:
			if (action.eventType == ActionEventType::CONTINUE)
				Engine::Instance().ApplyVelocity(e, float(deltaTime) * moveSpeed * Engine::Instance().GetMainCameraOwner()->getComponent<cTransform>().right);
			break;
		case ActionType::MOVE_UP1:
			if (action.eventType == ActionEventType::CONTINUE)
				Engine::Instance().ApplyVelocity(e, float(deltaTime) * moveSpeed * Engine::Instance().GetMainCameraOwner()->getComponent<cTransform>().up);
			break;
		case ActionType::MOVE_DOWN1:
			if (action.eventType == ActionEventType::CONTINUE)
				Engine::Instance().ApplyVelocity(e, -float(deltaTime) * moveSpeed * Engine::Instance().GetMainCameraOwner()->getComponent<cTransform>().up);
			break;
		case ActionType::RUN:
			if (action.eventType == ActionEventType::BEGIN)
				moveSpeed = 4;
			else if (action.eventType == ActionEventType::END)
				moveSpeed = 2;
			break;
		case ActionType::SELECT_0:
			if (action.eventType == ActionEventType::BEGIN)
			{
				Engine::Instance().postProcessingQuad->removeComponent<cShader>();
				Engine::Instance().postProcessingQuad->addComponent<cShader>(Engine::Instance().postProcessingShaders[ShaderType::POST_PROCESSING_DEFAULT]);
			}
			break;
		case ActionType::SELECT_1:
			if (action.eventType == ActionEventType::BEGIN)
			{
				Engine::Instance().postProcessingQuad->removeComponent<cShader>();
				Engine::Instance().postProcessingQuad->addComponent<cShader>(Engine::Instance().postProcessingShaders[ShaderType::COLOR_INVERSION]);
			}
			break;
		case ActionType::SELECT_2:
			if (action.eventType == ActionEventType::BEGIN)
			{
				Engine::Instance().postProcessingQuad->removeComponent<cShader>();
				Engine::Instance().postProcessingQuad->addComponent<cShader>(Engine::Instance().postProcessingShaders[ShaderType::GRAYSCALE]);
			}
			break;
		case ActionType::SELECT_3:
			if (action.eventType == ActionEventType::BEGIN)
			{
				Engine::Instance().postProcessingQuad->removeComponent<cShader>();
				Engine::Instance().postProcessingQuad->addComponent<cShader>(Engine::Instance().postProcessingShaders[ShaderType::SHARPEN]);
			}
			break;
		case ActionType::SELECT_4:
			if (action.eventType == ActionEventType::BEGIN)
			{
				Engine::Instance().postProcessingQuad->removeComponent<cShader>();
				Engine::Instance().postProcessingQuad->addComponent<cShader>(Engine::Instance().postProcessingShaders[ShaderType::BLUR]);
			}
			break;
		
		case ActionType::SELECT_5:
			if (action.eventType == ActionEventType::BEGIN)
			{
				Engine::Instance().postProcessingQuad->removeComponent<cShader>();
				Engine::Instance().postProcessingQuad->addComponent<cShader>(Engine::Instance().postProcessingShaders[ShaderType::EDGE_DETECTION]);
			}
			break;
		case ActionType::SELECT_6:
			if (action.eventType == ActionEventType::BEGIN)
			{
				Engine::Instance().postProcessingQuad->removeComponent<cShader>();
				Engine::Instance().postProcessingQuad->addComponent<cShader>(Engine::Instance().postProcessingShaders[ShaderType::CUSTOM_EFFECT]);
			}
			break;
		}
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

}