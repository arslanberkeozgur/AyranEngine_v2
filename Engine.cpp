#define GLM_ENABLE_EXPERIMENTAL

#include "Engine.h"
#include "Shader.h"
#include "Texture2D.h"
#include "Component.h"
#include "EntityManager.h"
#include "Scene.h"
#include "Model.h"
#include "stb_image.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <typeinfo>

unsigned int Engine::SCREEN_WIDTH  = 1600;
unsigned int Engine::SCREEN_HEIGHT = 900;
bool		 Engine::FULLSCREEN	   = false;

Engine::Engine()
{
	lastMouseX = float(SCREEN_WIDTH) / 2;
	lastMouseY = float(SCREEN_HEIGHT) / 2;

	entityManager = std::make_shared<EntityManager>();

	CreateWindow();
}

Engine& Engine::Instance()
{
	static Engine engine;
	return engine;
}

void Engine::Run()
{
	startTime = glfwGetTime();
	SetupShaders();
	OnStartEngine();

	while (!glfwWindowShouldClose(window))
	{
		CalculateDeltaTime();
		entityManager->update();
		ProcessInput();
		if (activeScene) activeScene->OnUpdate();
		ExecuteActions();
		TransformEntities();
		DefaultShaderUpdate();
		Render();
	}
}

// MAIN SYSTEMS
// -------------------------------------------------------------------------------------------

void Engine::CreateWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Recap", FULLSCREEN ? glfwGetPrimaryMonitor() : NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}

	// Make window the current context
	glfwMakeContextCurrent(window);

	// Disable the cursor.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLAD.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
	}
}

void Engine::SetupShaders()
{
	shaderMap[ShaderType::DEFAULT].load("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
	shaderMap[ShaderType::LIGHT_SOURCE].load("shaders/lightVertexShader.vert", "shaders/simpleColorFragmentShader.frag");
	shaderMap[ShaderType::OUTLINE].load("shaders/lightVertexShader.vert", "shaders/simpleColorFragmentShader.frag");

	activeShader = shaderMap[ShaderType::DEFAULT];

	activeShader.use();
	activeShader.setFloat("material.shininess", 64.0f);
	activeShader.setFloat("camInfo.near", nearFrustum);
	activeShader.setFloat("camInfo.far", farFrustum);
}

void Engine::OnStartEngine()
{
	defaultTexture.load("textures/white.jpg");
	defaultTexture.type = TextureType::DIFFUSE;

	models.push_back(std::make_shared<Model>("models/house/house.obj"));
	models.push_back(std::make_shared<Model>("models/ashtray/ashtray.obj"));

	if (WIREFRAME)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	// Add default scene.
	sceneMap["defaultScene"] = std::make_shared<Scene>("defaultScene");
	activeScene = sceneMap["defaultScene"];

	activeScene->BindActions();
	activeScene->OnStartScene();

	entityManager->update();
}

void Engine::DefaultShaderUpdate()
{
	activeShader = shaderMap[ShaderType::DEFAULT];
	activeShader.use();

	CalculateViewMatrix(activeShader);
	CalculateProjectionMatrix(activeShader);
	CalculateLighting(activeShader);
}

void Engine::CalculateViewMatrix(Shader& shader)
{
	if (mainCamera)
	{
		cTransform cameraTransform = entityManager->getEntityWithID(mainCamera->ownerID)->getComponent<cTransform>();
		glm::vec3 cameraPosition = cameraTransform.position + mainCamera->relativePosition;
		view = glm::lookAt(cameraPosition, cameraPosition + cameraTransform.front, cameraTransform.up);
	}
	else
	{
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, 5.0f));
	}

	shader.setFMat4("view", view);
}

void Engine::CalculateProjectionMatrix(Shader& shader)
{
	projection = glm::perspective(glm::radians(FOV), (float)SCREEN_WIDTH / SCREEN_HEIGHT, nearFrustum, farFrustum);
	shader.setFMat4("projection", projection);
}

void Engine::CalculateLighting(Shader& shader)
{
	// Global light
	shader.setFVec3("directionalLight.direction", TransformDirectionalVectorToViewSpace(globalLightDirection));
	shader.setFVec3("directionalLight.ambient", globalLightAmbient);
	shader.setFVec3("directionalLight.diffuse", globalLightDiffuse);
	shader.setFVec3("directionalLight.specular", globalLightSpecular);

	// Bookkeep point & spotlights
	shader.setInt("numOfPointLights", sceneNumOfPointLights);
	shader.setInt("numOfSpotLights", sceneNumOfSpotLights);

	CalculateSpotlights(shader);
	CalculatePointLights(shader);
}

void Engine::CalculateSpotlights(Shader& shader)
{
	unsigned int currentSpotLight = 0;
	for (Entity& e : entityManager->getEntities())
	{
		if (e.hasComponent<cSpotLight>())
		{
			cSpotLight& light = e.getComponent<cSpotLight>();
			shader.setFVec3("spotLights[" + std::to_string(currentSpotLight) + "].ambient", light.ambient);
			shader.setFVec3("spotLights[" + std::to_string(currentSpotLight) + "].diffuse", light.diffuse);
			shader.setFVec3("spotLights[" + std::to_string(currentSpotLight) + "].specular", light.specular);
			shader.setFloat("spotLights[" + std::to_string(currentSpotLight) + "].constant", light.constant);
			shader.setFloat("spotLights[" + std::to_string(currentSpotLight) + "].linear", light.linear);
			shader.setFloat("spotLights[" + std::to_string(currentSpotLight) + "].quadratic", light.quadratic);
			shader.setFloat("spotLights[" + std::to_string(currentSpotLight) + "].cutOff", light.cutoff);
			shader.setFloat("spotLights[" + std::to_string(currentSpotLight) + "].outerCutoff", light.outerCutoff);
			shader.setFVec3("spotLights[" + std::to_string(currentSpotLight) + "].position", TransformPositionVectorToViewSpace(e.getComponent<cTransform>().position));
			shader.setFVec3("spotLights[" + std::to_string(currentSpotLight) + "].direction", TransformDirectionalVectorToViewSpace(e.getComponent<cTransform>().front));
			++currentSpotLight;
		}
	}
}

void Engine::CalculatePointLights(Shader& shader)
{
	unsigned int currentPointLight = 0;
	for (Entity& e : entityManager->getEntities())
	{
		if (e.hasComponent<cPointLight>())
		{
			cPointLight& light = e.getComponent<cPointLight>();
			shader.setFVec3("pointLights[" + std::to_string(currentPointLight) + "].ambient", light.ambient);
			shader.setFVec3("pointLights[" + std::to_string(currentPointLight) + "].diffuse", light.diffuse);
			shader.setFVec3("pointLights[" + std::to_string(currentPointLight) + "].specular", light.specular);
			shader.setFloat("pointLights[" + std::to_string(currentPointLight) + "].constant", light.constant);
			shader.setFloat("pointLights[" + std::to_string(currentPointLight) + "].linear", light.linear);
			shader.setFloat("pointLights[" + std::to_string(currentPointLight) + "].quadratic", light.quadratic);
			shader.setFVec3("pointLights[" + std::to_string(currentPointLight) + "].position", TransformPositionVectorToViewSpace(e.getComponent<cTransform>().position));
			++currentPointLight;
		}
	}
}

void Engine::Render()
{
	ClearScreen(0.1f, 0.1f, 0.1f, 0.1f);

	for (Entity& e : entityManager->getEntities())
	{
		if (e.hasComponent<cModel>() && !e.getComponent<cModel>().isOutlined && e.hasComponent<cTransform>() && !e.hasComponent<cCamera>())
		{
			DrawEntity(e);
		}
	}

	for (Entity& e : outlinedObjects)
	{
		if (e.hasComponent<cTransform>() && !e.hasComponent<cCamera>())
		{
			glStencilMask(0xFF);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			DrawEntity(e);
			DrawOutlinedModel(e, e.getComponent<cModel>());
		}
	}

	glfwPollEvents();
	glfwSwapBuffers(window);
}

void Engine::DrawEntity(Entity e)
{
	if (!(e.hasComponent<cShader>()))
	{
		activeShader = shaderMap[ShaderType::DEFAULT];
		activeShader.use();
	}
	else
	{
		activeShader = e.getComponent<cShader>().shader;
		activeShader.use();
		activeShader.setFMat4("view", view);
		activeShader.setFMat4("projection", projection);
	}

	glm::mat4 translationMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(translationMatrix, e.getComponent<cTransform>().position);
	glm::mat4 scaleMatrix = glm::mat4(1.0f);
	scaleMatrix = glm::scale(scaleMatrix, e.getComponent<cTransform>().scale);
	glm::mat4 rotationMatrix = glm::toMat4(e.getComponent<cTransform>().orientation);
	glm::mat4 model = translationMatrix * rotationMatrix * scaleMatrix;
	activeShader.setFMat4("model", model);

	// Calculate the normal matrix
	glm::mat3 normal = glm::transpose(glm::inverse(view * model));
	activeShader.setFMat3("normalMatrix", normal);

	cModel& entityModel = e.getComponent<cModel>();
	entityModel.model->Draw(activeShader);
}

void Engine::ProcessInput()
{
	// TODO: Instead of polling, set an event-based input system.

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Remove all actions that have ended in the previous frame.
	auto result = std::remove_if(registeredActions.begin(), registeredActions.end(),
		[](Action& action) {return action.eventType == ActionEventType::END; });
	registeredActions.erase(result, registeredActions.end());

	for (auto& element : actionMap)
	{
		// For all elements of the action map, see if that action is registered currently by the user.
		auto it = std::find_if(registeredActions.begin(), registeredActions.end(), [element](Action& a) {return a.type == element.second; });
		bool found = (it == registeredActions.end()) ? false : true;
		// If the action is not currently registered but is pressed, then register the action.
		if (glfwGetKey(window, element.first) == GLFW_PRESS && !found)
		{
			registeredActions.push_back(Action(element.second, ActionEventType::BEGIN));
		}
		// If the action is registered and continually pressed, then set its type to CONTINUE.
		if (glfwGetKey(window, element.first) == GLFW_PRESS && found)
		{
			it->eventType = ActionEventType::CONTINUE;
		}
		// If the action is registered and it is just released, then set its type to END.
		if (glfwGetKey(window, element.first) == GLFW_RELEASE && found)
		{
			it->eventType = ActionEventType::END;
		}
	}
}

void Engine::ExecuteActions()
{
	for (Entity& e : entityManager->getEntities())
	{
		if (e.hasComponent<cInput>())
		{
			ExecuteActions(e);
		}
	}
}

void Engine::ExecuteActions(Entity& e)
{
	for (ActionType actionType : e.getComponent<cInput>().actions)
	{
		// Loop over all the defined actions of the entity. If any of the defined actions are currently registered, 
		// pass it to the active scene's definition of the action.
		auto it = std::find_if(registeredActions.begin(), registeredActions.end(), [actionType](Action& a) {return a.type == actionType; });
		bool found = (it == registeredActions.end()) ? false : true;
		if (found) activeScene->DefineActions(e, (*it));
	}
}

void Engine::TransformEntities()
{
	for (Entity& e : entityManager->getEntities())
	{
		if (e.hasComponent<cTransform>())
		{
			cTransform& transform = e.getComponent<cTransform>();
			transform.position += transform.velocity;
			transform.velocity = glm::vec3(0.0f);

			if (transform.allowRotation)
			{
				transform.front = glm::normalize(glm::rotate(glm::inverse(transform.orientation), glm::vec3(0.0, 0.0, -1.0)));
				transform.up = glm::normalize(glm::rotate(glm::inverse(transform.orientation), glm::vec3(0.0, 1.0, 0.0)));
				transform.right = glm::cross(transform.front, transform.up);
			}
		}
	}
}

void Engine::ApplyVelocity(Entity e, glm::vec3 vel)
{
	if (e.hasComponent<cTransform>())
		e.getComponent<cTransform>().velocity += vel;
	else
		std::cout << "WARNING::Applying velocity to an entity with no cTransform component::EntityTag=" << e.getTag() << std::endl;
}

void Engine::AddLocalRotation(Entity e, glm::vec3 axis, float angle)
// Axis need not be normalized, angle is in degrees.
{
	if (e.hasComponent<cTransform>())
	{
		e.getComponent<cTransform>().orientation = glm::rotate(e.getComponent<cTransform>().orientation, glm::radians(angle), glm::normalize(axis));
	}
	else 
		std::cout << "WARNING::Attempting rotation on an entity with no cTransform component::EntityTag=" << e.getTag() << std::endl;
}

void Engine::AddGlobalRotation(Entity e, glm::vec3 axis, float angle)
{
	if (e.hasComponent<cTransform>())
	{
		glm::quat rotation = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
		e.getComponent<cTransform>().orientation = rotation * e.getComponent<cTransform>().orientation;
	}
	else
		std::cout << "WARNING::Attempting rotation on an entity with no cTransform component::EntityTag=" << e.getTag() << std::endl;
}


// SECONDARY FUNCTIONS
// ---------------------------------------------------------------------------------------------------

Entity Engine::AddEntity(const std::string& name)
{
	return entityManager->addEntity(name);
}

Entity* Engine::GetMainCameraOwner()
{
	return entityManager->getEntityWithID(Engine::Instance().mainCamera->ownerID);
}

void Engine::SetMainCamera(cCamera* camera)
{
	mainCamera = camera;
}

void Engine::OnEntityDestroy(Entity& e)
{
	if (e.hasComponent<cPointLight>()) --sceneNumOfPointLights;
	if (e.hasComponent<cSpotLight>()) --sceneNumOfSpotLights;
}

void Engine::OnAddComponent(const Component& component)
{
	if (component.type == ComponentType::POINT_LIGHT) ++sceneNumOfPointLights;
	if (component.type == ComponentType::SPOT_LIGHT) ++sceneNumOfSpotLights;
}

void Engine::OnRemoveComponent(const Component& component)
{
	if (component.type == ComponentType::POINT_LIGHT) --sceneNumOfPointLights;
	if (component.type == ComponentType::SPOT_LIGHT) --sceneNumOfSpotLights;
}

std::vector<Entity>& Engine::GetEntitiesWithTag(const std::string& tag)
{
	return entityManager->getEntitiesWithTag(tag);
}

void Engine::AddGlobalLight(const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
{
	globalLightDirection = direction;
	globalLightAmbient = ambient;
	globalLightDiffuse = diffuse;
	globalLightSpecular = specular;
}


void Engine::BindInputKey(unsigned int key, ActionType action)
{
	actionMap[key] = action;
}

void Engine::BindCursorPositionCallback(GLFWcursorposfun MouseCallback)
{
	glfwSetCursorPosCallback(window, *MouseCallback);
}

void Engine::BindFramebufferSizeCallback(GLFWframebuffersizefun FrameBufferSizeCallback)
{
	glfwSetFramebufferSizeCallback(window, *FrameBufferSizeCallback);
}

GLFWwindow* Engine::GetWindow() const
{
	return window;
}

void Engine::CalculateDeltaTime()
{
	double currentFrame = glfwGetTime();
	deltaTime = currentFrame - currentTime;
	currentTime = currentFrame;
}

void Engine::ClearScreen(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0x00);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
}

glm::vec3 Engine::TransformPositionVectorToViewSpace(const glm::vec3& v)
{
	glm::vec4 pos4 = view * glm::vec4(v, 1.0f);
	return glm::vec3(pos4.x, pos4.y, pos4.z);
}

glm::vec3 Engine::TransformDirectionalVectorToViewSpace(const glm::vec3& v)
{
	glm::vec4 dir4 = glm::transpose(glm::inverse(view)) * glm::vec4(v, 0.0f);
	return glm::vec3(dir4.x, dir4.y, dir4.z);
}

void Engine::DrawOutlinedModel(Entity e, cModel& model)
{
	Shader previousShader = activeShader;
	activeShader = shaderMap[ShaderType::OUTLINE];
	glm::mat4 translationMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(translationMatrix, e.getComponent<cTransform>().position);
	glm::mat4 scaleMatrix = glm::mat4(1.0f);
	scaleMatrix = glm::scale(scaleMatrix, 1.1f * e.getComponent<cTransform>().scale);
	glm::mat4 rotationMatrix = glm::toMat4(e.getComponent<cTransform>().orientation);
	glm::mat4 modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
	activeShader.use();
	activeShader.setFMat4("model", modelMatrix);
	activeShader.setFVec3("color", model.outlineColor);
	activeShader.setFMat4("view", view);
	activeShader.setFMat4("projection", projection);

	glStencilMask(0x00);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glDisable(GL_DEPTH_TEST);
	model.model->Draw(activeShader);
	activeShader = previousShader;
	activeShader.use();
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glEnable(GL_DEPTH_TEST);
}

void Engine::OutlineEntity(Entity e, glm::vec3 color)
{
	if (e.hasComponent<cTransform>() && e.hasComponent<cModel>())
	{
		cModel& model = e.getComponent<cModel>();
		model.isOutlined = true;
		model.outlineColor = color;
		outlinedObjects.push_back(e);
	}
	else
		std::cout << "WARNING::Attempting to outline an object with no cTransform and/or cModel component." << std::endl;
}

void Engine::RemoveOutline(Entity e)
{
	if (e.hasComponent<cTransform>() && e.hasComponent<cModel>())
	{
		e.getComponent<cModel>().isOutlined = false;
		auto result = std::remove_if(outlinedObjects.begin(), outlinedObjects.end(), [e](Entity& entity) {return entity.getID() == e.getID(); });
		outlinedObjects.erase(result, outlinedObjects.end());
	}
	else 
		std::cout << "WARNING::Attempting to outline an object with no cTransform and/or cModel component." << std::endl;
}


// Getter Functions 

float Engine::GetTimeSinceCreation() const
{
	return currentTime;
}

Engine::~Engine()
{
	for (auto& pair : shaderMap)
	{
		if (pair.second.ID >= 0)
		{
			std::cout << "Shader program destroyed with ID : " << pair.second.ID << std::endl;
			glDeleteProgram(pair.second.ID);
		}
	}

	// Terminate.
	glfwTerminate();
}

