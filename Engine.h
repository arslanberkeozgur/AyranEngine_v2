#pragma once

#include "Input.h"
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <memory>
#include <map>
#include <vector>
#include <string>
#include <glm/glm.hpp>


struct cCamera;
class Scene;
class EntityManager;
class Entity;
class Shader;
struct Component;
class Model;

typedef std::map<std::string, std::shared_ptr<Shader>> ShaderMap;
typedef std::map<unsigned int, Action_Type> ActionMap;
typedef std::map<std::string, std::shared_ptr<Scene>> SceneMap;

class Engine
{
private:
	Engine(unsigned int SW = 1600, unsigned int SH = 900, bool wireframe = false);
public:
	static Engine& Instance();
	~Engine();

public:
	unsigned int					SCREEN_WIDTH				    = 1600;
	unsigned int					SCREEN_HEIGHT					= 900;
	bool							WIREFRAME						= false;

	float							inputMoveSpeed					= 2.0f;
	float							inputMouseSensitivity			= 0.1f;
	bool							inputCameraConstrainPitch		= true;
	
	glm::vec3						worldUp							{ 0.0f, 1.0f, 0.0f };

	double							deltaTime						= 0.0f;

	unsigned int					containerVAO					= 0; 
	unsigned int					VBO  							= 0;
	unsigned int					lightVAO						= 0;
	std::vector<float>				vertices;

	bool							firstMouse						= true;
	double							lastMouseX;
	double							lastMouseY;

	cCamera*						mainCamera						= nullptr;
	ShaderMap						shaderMap;
	std::shared_ptr<Shader>			activeShader					= nullptr;

	ActionMap						actionMap;
	std::vector<Action>				registeredActions;

	SceneMap						sceneMap;
	std::shared_ptr<Scene>			activeScene						= nullptr;

	std::vector<std::shared_ptr<Model>> models;

private:

	std::shared_ptr<EntityManager>	entityManager;

	double					lastFrame						= 0.0f;
	double					startTime						= 0.0f;

	unsigned int			sceneNumOfPointLights			= 0;
	unsigned int			sceneNumOfSpotLights			= 0;

	glm::vec3				globalLightDirection			= { 0.0f, 0.0f, 0.0f };
	glm::vec3				globalLightAmbient				= { 0.0f, 0.0f, 0.0f };
	glm::vec3				globalLightDiffuse				= { 0.0f, 0.0f, 0.0f };
	glm::vec3				globalLightSpecular				= { 0.0f, 0.0f, 0.0f };

private:
	glm::mat4				model							= glm::mat4(1.0f);
	glm::mat4				view                            = glm::mat4(1.0f);
	glm::mat4				projection                      = glm::mat4(1.0f);

	void CalculateViewMatrix();
	void CalculateProjectionMatrix();

	glm::vec3 TransformPositionVectorToViewSpace(const glm::vec3& v);
	glm::vec3 TransformDirectionalVectorToViewSpace(const glm::vec3& v);

public:
	void Run();

public:
	Entity AddEntity(const std::string& name);
	void SetMainCamera(cCamera* camera);
	Entity* GetMainCameraOwner();

public:
	void BindFramebufferSizeCallback(GLFWframebuffersizefun frameBufferSizeCallback);
	void BindCursorPositionCallback(GLFWcursorposfun MouseCallback);
	void BindVertexData(float* VertexData, size_t size);

public:
	GLFWwindow* GetWindow() const;
private:
	GLFWwindow* window = nullptr;

private:
	void CreateWindow();
	void SetupRenderObjects();
	void SetupShaders();
	void OnStartEngine();
	void ProcessInput();
	void ExecuteActions(Entity& e);

	void TransformEntities();
	void CalculateLighting();
	void CalculateSpotlights();
	void CalculatePointLights();
	void Render();

private:
	void CalculateDeltaTime();
	void ClearScreen(float r, float g, float b, float a);

public:
	void BindInputKey(unsigned int key, Action_Type action);

public:
	void OnEntityDestroy(Entity& e);
	void OnAddComponent(const Component& e);
	void OnRemoveComponent(const Component& e);

public:
	void AddGlobalLight(const glm::vec3& direction = glm::vec3(0.3f, -1.0f, 0.3f), const glm::vec3& ambient = glm::vec3(0.1f), const glm::vec3& diffuse = glm::vec3(1.0f), const glm::vec3& specular = glm::vec3(1.0f));
};