#pragma once
#include "Renderer.h"
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <math.h>


class Camera
{
private:

	GLFWwindow* m_WindowHandle;

	glm::vec3 m_Position;

	float m_HorizontalAngle;
	float m_VeritcalAngle;

	float m_MovementSpeed;
	float m_RotationSpeed;

	float m_FieldOfView = 45.0f;

	int m_Width = 1280;
	int m_Height = 720;

	double m_LastTime;
	double m_CurrentTime;
	float m_DeltaTime;

	glm::vec3 m_direction = glm::vec3(
		cos(m_VeritcalAngle) * sin(m_HorizontalAngle),
		sin(m_VeritcalAngle),
		cos(m_VeritcalAngle) * cos(m_HorizontalAngle)
	);

	glm::vec3 m_right = glm::vec3(
		sin(m_HorizontalAngle - 3.14f / 2.0f),
		0,
		cos(m_HorizontalAngle - 3.14f / 2.0f)
	);

	glm::vec3 m_up = glm::cross(m_right, m_direction);
	
	unsigned int m_FrameCount = 0;

	//int Comparekey(const int pressedKey);

public:
	Camera(GLFWwindow* window);
	//~Camera();

	void CheckTime();
	void CalculateMovement();
	void CheckFuncKeys();


	glm::mat4 ProjectionMatrix();
	glm::mat4 ViewMatrix();
	

	


};