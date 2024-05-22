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

	//stbi_set_flip_vertically_on_load(true);
	models.push_back(std::make_shared<Model>(Model("models/commodore/commodore.obj")));

	SetupRenderObjects();
	SetupShaders();
	OnStartEngine();

	Texture2D texture{ "containerDiffuse" };
	texture.setDiffuseUnit(*activeShader, 0);
	texture.load("textures/container2.png");
	Texture2D specularTexture{ "containerSpecular" };
	specularTexture.setSpecularUnit(*activeShader, 1);
	specularTexture.load("textures/container2_specular.png");


	// Run the rendering loop.
	while (!glfwWindowShouldClose(window))
	{
		activeShader = shaderMap["defaultShader"];
		activeShader->use();

		CalculateDeltaTime();

		// 1. Manage Memory
		entityManager->update();

		// 2. Input Processing
		ProcessInput();

		ClearScreen(0.1f, 0.1f, 0.1f, 0.1f);

		if (activeScene) activeScene->OnUpdate();

		// !!!CalculateViewMatrix, CalculateProjectionMatrix & CalculateLighting assumes activeShader == defaultShader!!!
		CalculateViewMatrix();
		CalculateProjectionMatrix();

		// Movement System
		TransformEntities();
		// Lighting System
		CalculateLighting();

		// 4. Movement and render coupled for now.
		for (Entity& e : entityManager->getEntities())
		{
			activeShader = shaderMap["defaultShader"];
			activeShader->use();
			ExecuteActions(e);

			if (e.hasComponent<cTransform>())
			{
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, e.getComponent<cTransform>().position);
				model = glm::scale(model, e.getComponent<cTransform>().scale);
				activeShader->setFMat4("model", model);

				// Calculate the normal matrix
				glm::mat3 normal = glm::transpose(glm::inverse(view * model));
				activeShader->setFMat3("normalMatrix", normal);

				if (e.hasComponent<cPointLight>())
				{
					cPointLight& light = e.getComponent<cPointLight>();
					activeShader = shaderMap["lightShader"];
					activeShader->use();
					activeShader->setFMat4("projection", projection);
					activeShader->setFMat4("view", view);
					activeShader->setFMat4("model", model);
					activeShader->setFVec3("color", light.diffuse);
				}

				if (e.hasComponent<cModel>())
				{
					e.getComponent<cModel>().model->Draw((*activeShader));
					continue;
				}

				texture.use();
				specularTexture.use();

				if (e.hasComponent<cVAO>() && !e.hasComponent<cCamera>())
				{
					glBindVertexArray(e.getComponent<cVAO>().VAO);
					glDrawArrays(GL_TRIANGLES, 0, 36);
				}	
			}
		}
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
}

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
	shaderMap["defaultShader"] = std::make_shared<Shader>();
	shaderMap["defaultShader"]->load("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
	shaderMap["lightShader"] = std::make_shared<Shader>();
	shaderMap["lightShader"]->load("shaders/lightVertexShader.vert", "shaders/lightFragmentShader.frag");

	activeShader = shaderMap["defaultShader"];

	activeShader->use();
	// TODO:
	// This will need to change later once we add customizable materials.
	activeShader->setFloat("material.shininess", 64.0f);
}

void Engine::SetupRenderObjects()
{
	// Generate buffers.
	glGenVertexArrays(1, &containerVAO);
	glGenBuffers(1, &VBO);
	// Bind VAO.
	glBindVertexArray(containerVAO);
	// Bind VBO and pass data.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	// Attribute pointer for vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Attribute pointer for normal vectors
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Attribute pointer for texture coords.
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// Unbind buffers.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create a different VAO for the light so that it won't be affected from future changes.
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	if (WIREFRAME)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	glEnable(GL_DEPTH_TEST);
}

void Engine::OnStartEngine()
{
	// Add default scene.
	sceneMap["defaultScene"] = std::make_shared<Scene>("defaultScene");
	activeScene = sceneMap["defaultScene"];

	activeScene->BindActions();
	activeScene->OnStartScene();

	entityManager->update();
}

void Engine::ProcessInput()
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Remove all actions that have ended in the previous frame.
	auto result = std::remove_if(registeredActions.begin(), registeredActions.end(), 
		[](Action& action) {return action.eventType == Action_Event_Type::END; });
	registeredActions.erase(result, registeredActions.end());

	for (auto& element : actionMap)
	{
		auto it = std::find_if(registeredActions.begin(), registeredActions.end(), [element](Action& a) {return a.type == element.second; });
		bool found = (it == registeredActions.end()) ? false : true;
		if (glfwGetKey(window, element.first) == GLFW_PRESS && !found)
		{
			registeredActions.push_back(Action(element.second, Action_Event_Type::BEGIN));
		}
		if (glfwGetKey(window, element.first) == GLFW_PRESS && found)
		{
			it->eventType = Action_Event_Type::CONTINUE;
		}
		if (glfwGetKey(window, element.first) == GLFW_RELEASE && found)
		{
			it->eventType = Action_Event_Type::END;
		}
	}
}

void Engine::ExecuteActions(Entity& e)
{
	if (e.hasComponent<cInput>())
	{
		for (Action_Type actionType : e.getComponent<cInput>().actions)
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

			if (transform.allowRotation)
			{

			}
		}
	}
}

void Engine::CalculatePointLights()
{
	unsigned int currentPointLight = 0;
	for (Entity& e : entityManager->getEntities())
	{
		if (e.hasComponent<cPointLight>())
		{
			cPointLight& light = e.getComponent<cPointLight>();
			activeShader->setFVec3("pointLights[" + std::to_string(currentPointLight) + "].ambient", light.ambient);
			activeShader->setFVec3("pointLights[" + std::to_string(currentPointLight) + "].diffuse", light.diffuse);
			activeShader->setFVec3("pointLights[" + std::to_string(currentPointLight) + "].specular", light.specular);
			activeShader->setFloat("pointLights[" + std::to_string(currentPointLight) + "].constant", light.constant);
			activeShader->setFloat("pointLights[" + std::to_string(currentPointLight) + "].linear", light.linear);
			activeShader->setFloat("pointLights[" + std::to_string(currentPointLight) + "].quadratic", light.quadratic);
			glm::vec4 lightPos = view * glm::vec4(e.getComponent<cTransform>().position, 1.0f);
			activeShader->setFVec3("pointLights[" + std::to_string(currentPointLight) + "].position", glm::vec3(lightPos.x, lightPos.y, lightPos.z));
			++currentPointLight;
		}
	}
}

void Engine::CalculateSpotlights()
{
	unsigned int currentSpotLight = 0;
	for (Entity& e : entityManager->getEntities())
	{
		if (e.hasComponent<cSpotLight>())
		{
			cSpotLight& light = e.getComponent<cSpotLight>();
			activeShader->setFVec3("spotLights[" + std::to_string(currentSpotLight) + "].ambient", light.ambient);
			activeShader->setFVec3("spotLights[" + std::to_string(currentSpotLight) + "].diffuse", light.diffuse);
			activeShader->setFVec3("spotLights[" + std::to_string(currentSpotLight) + "].specular", light.specular);
			activeShader->setFloat("spotLights[" + std::to_string(currentSpotLight) + "].constant", light.constant);
			activeShader->setFloat("spotLights[" + std::to_string(currentSpotLight) + "].linear", light.linear);
			activeShader->setFloat("spotLights[" + std::to_string(currentSpotLight) + "].quadratic", light.quadratic);
			activeShader->setFloat("spotLights[" + std::to_string(currentSpotLight) + "].cutOff", light.cutoff);
			activeShader->setFloat("spotLights[" + std::to_string(currentSpotLight) + "].outerCutoff", light.outerCutoff);
			activeShader->setFVec3("spotLights[" + std::to_string(currentSpotLight) + "].position", TransformPositionVectorToViewSpace(e.getComponent<cTransform>().position));
			activeShader->setFVec3("spotLights[" + std::to_string(currentSpotLight) + "].direction", TransformDirectionalVectorToViewSpace(e.getComponent<cTransform>().front));
			++currentSpotLight;
		}
	}
}

void Engine::CalculateLighting()
{
	// Global light
	activeShader->setFVec3("directionalLight.direction", TransformDirectionalVectorToViewSpace(globalLightDirection));
	activeShader->setFVec3("directionalLight.ambient", globalLightAmbient);
	activeShader->setFVec3("directionalLight.diffuse", globalLightDiffuse);
	activeShader->setFVec3("directionalLight.specular", globalLightSpecular);

	// Bookkeep point & spotlights
	activeShader->setInt("numOfPointLights", sceneNumOfPointLights);
	activeShader->setInt("numOfSpotLights", sceneNumOfSpotLights);

	CalculateSpotlights();
	CalculatePointLights();
}

void Engine::Render()
{

}

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
	if (component.type == Component_Type::POINT_LIGHT) ++sceneNumOfPointLights;
	if (component.type == Component_Type::SPOT_LIGHT) ++sceneNumOfSpotLights;
}

void Engine::OnRemoveComponent(const Component& component)
{
	if (component.type == Component_Type::POINT_LIGHT) --sceneNumOfPointLights;
	if (component.type == Component_Type::SPOT_LIGHT) --sceneNumOfSpotLights;
}

void Engine::AddGlobalLight(const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
{
	globalLightDirection = direction;
	globalLightAmbient = ambient;
	globalLightDiffuse = diffuse;
	globalLightSpecular = specular;
}


void Engine::BindInputKey(unsigned int key, Action_Type action)
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

void Engine::BindVertexData(float* VertexData, size_t size)
{
	vertices = std::vector<float>(VertexData, VertexData + size / sizeof(VertexData[0]));
}

GLFWwindow* Engine::GetWindow() const
{
	return window;
}

void Engine::CalculateDeltaTime()
{
	double currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void Engine::ClearScreen(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Engine::CalculateViewMatrix()
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

	activeShader->setFMat4("view", view);
}

void Engine::CalculateProjectionMatrix()
{
	projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
	activeShader->setFMat4("projection", projection);
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
	// Clean up.
	glDeleteVertexArrays(1, &containerVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	// Terminate.
	glfwTerminate();
}

