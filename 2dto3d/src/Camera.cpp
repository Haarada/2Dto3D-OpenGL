#include "Camera.h"
#include <iostream>
Camera::Camera(GLFWwindow* window)
	: m_WindowHandle(window), m_Position(glm::vec3(0,0,2)), m_HorizontalAngle(3.14f), m_VeritcalAngle(0.0f),
m_MovementSpeed(3.0f), m_RotationSpeed(3.0f), m_LastTime(0.0f), m_CurrentTime(0.0f), m_DeltaTime(0.0f)
{
}


//int camera::comparekey(const int pressedkey)
//{
//	if (glfwgetkey(m_windowhandle, pressedkey) == glfw_press)
//		return pressedkey;
//	return -1;
//}

void Camera::CalculateMovement()
{

	m_direction = glm::vec3(
		cos(m_VeritcalAngle) * sin(m_HorizontalAngle),
		sin(m_VeritcalAngle),
		cos(m_VeritcalAngle) * cos(m_HorizontalAngle)
	);

	m_right = glm::vec3(
		sin(m_HorizontalAngle - 3.14f / 2.0f),
		0,
		cos(m_HorizontalAngle - 3.14f / 2.0f)
	);

	m_up = glm::cross(m_right, m_direction);


}



void Camera::CheckFuncKeys()
{
	m_DeltaTime = m_CurrentTime - m_LastTime;
	m_FrameCount++;
	if (m_FrameCount * m_DeltaTime > 1)
	{
		std::cout << "FPS: " << 1.0f / m_DeltaTime << "	| Frame time: " << m_DeltaTime * 1000 << " ms\n";
		m_FrameCount = 0;
	}
	//std::cout << "Frametime:" << m_DeltaTime*1000 << "\n";
	//std::cout << "FPS:" << 1.0f / m_DeltaTime << "\n";
	// angles
	if (glfwGetKey(m_WindowHandle, GLFW_KEY_UP) == GLFW_PRESS)
		m_VeritcalAngle += m_RotationSpeed * m_DeltaTime;
	if (glfwGetKey(m_WindowHandle, GLFW_KEY_DOWN) == GLFW_PRESS)
		m_VeritcalAngle -= m_RotationSpeed * m_DeltaTime;
	if (glfwGetKey(m_WindowHandle, GLFW_KEY_LEFT) == GLFW_PRESS)
		m_HorizontalAngle += m_RotationSpeed * m_DeltaTime;
	if (glfwGetKey(m_WindowHandle, GLFW_KEY_RIGHT) == GLFW_PRESS)
		m_HorizontalAngle -= m_RotationSpeed * m_DeltaTime;

	CalculateMovement();

	// Movement
	if (glfwGetKey(m_WindowHandle, GLFW_KEY_W) == GLFW_PRESS)
		m_Position += m_direction * m_DeltaTime * m_MovementSpeed;
	if (glfwGetKey(m_WindowHandle, GLFW_KEY_S) == GLFW_PRESS)
		m_Position -= m_direction * m_DeltaTime * m_MovementSpeed;
	if (glfwGetKey(m_WindowHandle, GLFW_KEY_A) == GLFW_PRESS)
		m_Position -= m_right * m_DeltaTime * m_MovementSpeed;
	if (glfwGetKey(m_WindowHandle, GLFW_KEY_D) == GLFW_PRESS)
		m_Position += m_right * m_DeltaTime * m_MovementSpeed;

	

}

glm::mat4 Camera::ProjectionMatrix()
{

	return glm::perspective(glm::radians(m_FieldOfView), float(m_Width) / float(m_Height), 0.1f, 100.0f);
}

glm::mat4 Camera::ViewMatrix()
{
	return glm::lookAt(
		m_Position,
		m_Position + m_direction,
		m_up
	);
}

void Camera::CheckTime()
{
	m_LastTime = m_CurrentTime;
	m_CurrentTime = glfwGetTime();
}


