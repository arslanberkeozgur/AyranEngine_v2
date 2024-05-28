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
#include <typeinfo>

Engine::Engine(unsigned int SW, unsigned int SH, bool wireframe)
	: SCREEN_WIDTH{SW}, SCREEN_HEIGHT{SH}, WIREFRAME{wireframe}
{
	lastMouseX = float(SCREEN_WIDTH) / 2;
	lastMouseY = float(SCREEN_HEIGHT) / 2;

	entityManager = std::make_shared<EntityManager>();

	CreateWindow();
}

Engine& Engine::Instance()
{
	static Engine engine(1600, 900, false);
	return engine;
}

void Engine::Run()
{
	startTime = glfwGetTime();
	SetupShaders();
	OnStartEngine();

	entityManager->getEntitiesWithTag("house1")[0].addComponent<cShader>(shaderMap[ShaderType::LIGHT_SOURCE]);

	while (!glfwWindowShouldClose(window))
	{
		CalculateDeltaTime();
		entityManager->update();
		ProcessInput();
		if (activeScene) activeScene->OnUpdate();
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

	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Recap", NULL, NULL);
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
	shaderMap[ShaderType::LIGHT_SOURCE].load("shaders/lightVertexShader.vert", "shaders/lightFragmentShader.frag");

	activeShader = shaderMap[ShaderType::DEFAULT];

	activeShader.use();
	// TODO:
	// This will need to change later once we add customizable materials.
	activeShader.setFloat("material.shininess", 64.0f);
}

void Engine::OnStartEngine()
{
	defaultTexture.load("textures/white.jpg");
	defaultTexture.type = TextureType::DIFFUSE;

	models.push_back(std::make_shared<Model>("models/house/house.obj"));

	if (WIREFRAME)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	glEnable(GL_DEPTH_TEST);

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
	projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
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
			glm::vec4 lightPos = view * glm::vec4(e.getComponent<cTransform>().position, 1.0f);
			shader.setFVec3("pointLights[" + std::to_string(currentPointLight) + "].position", glm::vec3(lightPos.x, lightPos.y, lightPos.z));
			++currentPointLight;
		}
	}
}

void Engine::Render()
{
	ClearScreen(0.1f, 0.1f, 0.1f, 0.1f);
	for (Entity& e : entityManager->getEntities())
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
			activeShader.setFVec3("color", glm::vec3(glm::abs(glm::sin(currentTime)), 0.0f, 0.0f));
		}

		ExecuteActions(e);

		if (e.hasComponent<cTransform>())
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, e.getComponent<cTransform>().position);
			model = glm::scale(model, e.getComponent<cTransform>().scale);
			model = glm::rotate(model, glm::radians(e.getComponent<cTransform>().eulerX), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(e.getComponent<cTransform>().eulerY), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(e.getComponent<cTransform>().eulerZ), glm::vec3(0.0f, 0.0f, 1.0f));
			activeShader.setFMat4("model", model);

			// Calculate the normal matrix
			glm::mat3 normal = glm::transpose(glm::inverse(view * model));
			activeShader.setFMat3("normalMatrix", normal);

			if (e.hasComponent<cModel>())
			{
				e.getComponent<cModel>().model->Draw(activeShader);
			}
		}
	}

	glfwPollEvents();
	glfwSwapBuffers(window);
}

void Engine::ProcessInput()
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Remove all actions that have ended in the previous frame.
	auto result = std::remove_if(registeredActions.begin(), registeredActions.end(),
		[](Action& action) {return action.eventType == ActionEventType::END; });
	registeredActions.erase(result, registeredActions.end());

	for (auto& element : actionMap)
	{
		auto it = std::find_if(registeredActions.begin(), registeredActions.end(), [element](Action& a) {return a.type == element.second; });
		bool found = (it == registeredActions.end()) ? false : true;
		if (glfwGetKey(window, element.first) == GLFW_PRESS && !found)
		{
			registeredActions.push_back(Action(element.second, ActionEventType::BEGIN));
		}
		if (glfwGetKey(window, element.first) == GLFW_PRESS && found)
		{
			it->eventType = ActionEventType::CONTINUE;
		}
		if (glfwGetKey(window, element.first) == GLFW_RELEASE && found)
		{
			it->eventType = ActionEventType::END;
		}
	}
}

void Engine::ExecuteActions(Entity& e)
{
	if (e.hasComponent<cInput>())
	{
		for (ActionType actionType : e.getComponent<cInput>().actions)
		{
			auto it = std::find_if(registeredActions.begin(), registeredActions.end(), [actionType](Action& a) {return a.type == actionType; });
			bool found = (it == registeredActions.end()) ? false : true;
			if (found) activeScene->DefineActions(e, (*it));
		}
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
		}
	}
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

