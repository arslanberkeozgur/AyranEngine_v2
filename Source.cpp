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
		Entity* cameraOwner = Engine::Instance().GetMainCameraOwner();
		cTransform& cameraTransform = cameraOwner->getComponent<cTransform>();
		cameraTransform.yaw += float(xOffset);
		cameraTransform.pitch -= float(yOffset);
		if (Engine::Instance().inputCameraConstrainPitch)
		{
			if (cameraTransform.pitch > 89.0f)
				cameraTransform.pitch = 89.0f;
			if (cameraTransform.pitch < -89.0f)
				cameraTransform.pitch = -89.0f;
		}
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(cameraTransform.yaw)) * cos(glm::radians(cameraTransform.pitch));
		front.y = sin(glm::radians(cameraTransform.pitch));
		front.z = sin(glm::radians(cameraTransform.yaw)) * cos(glm::radians(cameraTransform.pitch));
		cameraTransform.front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		cameraTransform.right = glm::normalize(glm::cross(cameraTransform.front, Engine::Instance().worldUp));
		cameraTransform.up = glm::normalize(glm::cross(cameraTransform.right, cameraTransform.front));
	}
}
