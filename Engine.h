#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <memory>
#include <map>
#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "Input.h"
#include "Texture2D.h"
#include "Shader.h"


struct cCamera;
class Scene;
class EntityManager;
class Entity;
struct Component;
class Model;

typedef std::map<ShaderType, Shader> ShaderMap;
typedef std::map<unsigned int, ActionType> ActionMap;
typedef std::map<std::string, std::shared_ptr<Scene>> SceneMap;

class Engine
{
private:
	Engine();
public:
	static Engine& Instance();
	~Engine();
	Engine(Engine const&) = delete;
	void operator=(Engine const&) = delete;

public:
	// General configuration options
	static unsigned int				SCREEN_WIDTH;
	static unsigned int				SCREEN_HEIGHT;
	static bool						FULLSCREEN;
	bool							WIREFRAME						= false;

	float							inputMouseSensitivity			= 0.1f;
	bool							inputCameraConstrainPitch		= true;
	
	glm::vec3						worldUp							{ 0.0f, 1.0f, 0.0f };

	double							deltaTime						= 0.0f;

	bool							firstMouse						= true;
	double							lastMouseX;
	double							lastMouseY;

	Texture2D						defaultTexture;

	cCamera*						mainCamera						= nullptr;
	ShaderMap						shaderMap;
	Shader							activeShader;

	// An action map is a mapping from keys to Actions.
	ActionMap						actionMap;
	// Registered actions is a vector of actions that are currently active.
	std::vector<Action>				registeredActions;

	SceneMap						sceneMap;
	std::shared_ptr<Scene>			activeScene						= nullptr;

	std::vector<std::shared_ptr<Model>> models;

private:

	std::shared_ptr<EntityManager>	entityManager;

	double					currentTime						= 0.0f;
	double					startTime						= 0.0f;

	// Perspective data.
	float					FOV								= 45.0f;
	float					nearFrustum						= 0.1f;
	float					farFrustum						= 100.0f;
	
	// Data related to lighting.
	unsigned int			sceneNumOfPointLights			= 0;
	unsigned int			sceneNumOfSpotLights			= 0;

	glm::vec3				globalLightDirection			= { 0.0f, 0.0f, 0.0f };
	glm::vec3				globalLightAmbient				= { 0.0f, 0.0f, 0.0f };
	glm::vec3				globalLightDiffuse				= { 0.0f, 0.0f, 0.0f };
	glm::vec3				globalLightSpecular				= { 0.0f, 0.0f, 0.0f };

public:
	void Run();

public:
	Entity AddEntity(const std::string& name);
	void SetMainCamera(cCamera* camera);
	Entity* GetMainCameraOwner();

public:
	void BindFramebufferSizeCallback(GLFWframebuffersizefun frameBufferSizeCallback);
	void BindCursorPositionCallback(GLFWcursorposfun MouseCallback);

public:
	GLFWwindow* GetWindow() const;
private:
	GLFWwindow* window = nullptr;

private:
	void CreateWindow();
	void SetupShaders();
	void DefaultShaderUpdate();
	void OnStartEngine();
	void ProcessInput();
	void ExecuteActions(Entity& e);

public:
	void ApplyVelocity(Entity e, glm::vec3 vel);
	void AddLocalRotation(Entity e, glm::vec3 axis, float angle);
	void AddGlobalRotation(Entity e, glm::vec3 axis, float angle);

private:
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	void CalculateViewMatrix(Shader& shader);
	void CalculateProjectionMatrix(Shader& shader);
	void CalculateLighting(Shader& shader);
	void CalculateSpotlights(Shader& shader);
	void CalculatePointLights(Shader& shader);

	glm::vec3 TransformPositionVectorToViewSpace(const glm::vec3& v);
	glm::vec3 TransformDirectionalVectorToViewSpace(const glm::vec3& v);

private:
	void CalculateDeltaTime();
	void ClearScreen(float r, float g, float b, float a);
	void Render();

public:
	void BindInputKey(unsigned int key, ActionType action);

public:
	void OnEntityDestroy(Entity& e);
	void OnAddComponent(const Component& e);
	void OnRemoveComponent(const Component& e);

public:
	void AddGlobalLight(const glm::vec3& direction = glm::vec3(0.3f, -1.0f, 0.3f), const glm::vec3& ambient = glm::vec3(0.1f), const glm::vec3& diffuse = glm::vec3(1.0f), const glm::vec3& specular = glm::vec3(1.0f));

private:
	void TransformEntities();
};