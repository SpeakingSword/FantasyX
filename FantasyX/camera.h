#ifndef CAMERA_H
#define CAMERA_H

#include <glad\glad.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <vector>

// 摄像机可能的移动方式
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// 默认的摄像机常量
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.05f;
const float FOV = 60.0f;
const float MAX_FOV = 80.0f;
const float MIN_FOV = 0.1f;
const float ZOOM_SPEED = 8.0f;
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 1000.0f;

class Camera {
public:
	// 摄像机属性
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// 欧拉角
	float Yaw;
	float Pitch;
	// 摄像机可配置选项
	float MovementSpeed;
	float MouseSensitivity;
	float Fov;
	float ZoomSpeed;
	float Near_Plane;
	float Far_Plane;

	// 使用向量构建摄像机
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
		float yaw = YAW, float pitch = PITCH): Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), 
		MouseSensitivity(SENSITIVITY), Fov(FOV), ZoomSpeed(ZOOM_SPEED), Near_Plane(NEAR_PLANE), Far_Plane(FAR_PLANE)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// 使用标量构建摄像机
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
		Front(glm::vec3(0.0f, 0.0f, -0.1f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Fov(FOV), ZoomSpeed(ZOOM_SPEED),
		Near_Plane(NEAR_PLANE), Far_Plane(FAR_PLANE)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// 处理摄像机收到的可用的键盘输入
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
	}

	// 处理摄像机收到的鼠标输入
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		updateCameraVectors();
	}

	// 处理摄像机收到的滚轮输入
	void ProcessMouseScroll(float yoffset, float deltaTime)
	{
		Position += Front * yoffset * ZoomSpeed * deltaTime;
	}

	// 计算并返回相机的观察矩阵
	glm::mat4 lookAt(glm::vec3 cameraPos, glm::vec3 cameraTargetPos, glm::vec3 up)
	{
		glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTargetPos);
		glm::vec3 cameraRight = glm::normalize(glm::cross(glm::normalize(up), cameraDirection));
		glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
		glm::mat4 translation;
		translation[3][0] = -cameraPos.x;
		translation[3][1] = -cameraPos.y;
		translation[3][2] = -cameraPos.z;
		glm::mat4 rotation;
		rotation[0][0] = cameraRight.x;
		rotation[1][0] = cameraRight.y;
		rotation[2][0] = cameraRight.z;
		rotation[0][1] = cameraUp.x;
		rotation[1][1] = cameraUp.y;
		rotation[2][1] = cameraUp.z;
		rotation[0][2] = cameraDirection.x;
		rotation[1][2] = cameraDirection.y;
		rotation[2][2] = cameraDirection.z;
		return rotation * translation;

	}

private:
	// 计算并更新摄像机的方向
	void updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
		front.y = sin(glm::radians(Pitch));
		front.z = cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));
		Front = glm::normalize(front);
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};

#endif // !CAMERA_H

