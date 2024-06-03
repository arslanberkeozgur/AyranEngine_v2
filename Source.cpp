#include "Engine.h"
#include "EntityManager.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);

int main()
{
	Engine::Instance().BindFramebufferSizeCallback(framebuffer_size_callback);
	Engine::Instance().BindCursorPositionCallback(mouse_callback);

	Engine::Instance().Run();

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	if (Engine::Instance().firstMouse)
	{
		Engine::Instance().lastMouseX = xPos;
		Engine::Instance().lastMouseY = yPos;
		Engine::Instance().firstMouse = false;
	}

	double xOffset = Engine::Instance().inputMouseSensitivity * (xPos - Engine::Instance().lastMouseX);
	double yOffset = Engine::Instance().inputMouseSensitivity * (yPos - Engine::Instance().lastMouseY);
	Engine::Instance().lastMouseX = xPos;
	Engine::Instance().lastMouseY = yPos;

	if (Engine::Instance().mainCamera)
	{
		cCamera* camera = Engine::Instance().mainCamera;
		cTransform& camTransform = Engine::Instance().GetMainCameraOwner()->getComponent<cTransform>();
		camera->yaw += float(xOffset);
		camera->pitch -= float(yOffset);
		if (Engine::Instance().inputCameraConstrainPitch)
		{
			if (camera->pitch > 89.0f)
				camera->pitch = 89.0f;
			if (camera->pitch < -89.0f)
				camera->pitch = -89.0f;
		}

		glm::quat qPitch = glm::angleAxis(glm::radians(-camera->pitch), glm::vec3(1, 0, 0));
		glm::quat qYaw = glm::angleAxis(glm::radians(camera->yaw), glm::vec3(0, 1, 0));
		// omit roll
		camTransform.orientation = glm::normalize(qPitch * qYaw);
	}
}
