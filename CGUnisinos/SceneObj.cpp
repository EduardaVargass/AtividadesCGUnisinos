#pragma once
#include <string>
#include "SceneObjInfo.cpp"
#include <GLFW/glfw3.h>
#include "../Common/include/Shader.h"

using namespace std; 

class SceneObj {
public:
	SceneObjInfo sceneObjInfo;

	SceneObj(float x, float y, float z, string objFilePath, Shader* shader, glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0), 
		float rotationAngle = 0.0, glm::vec3 rotationAxis = glm::vec3(0.0, 0.0, 1.0), float translationSpeed = 0.05)
		: x(x), y(y), z(z), objFilePath(objFilePath), sceneObjInfo(objFilePath), shader(shader), scale(scale), rotationAngle(rotationAngle),
		rotationAxis(rotationAxis), translationSpeed(translationSpeed)
	{
		this->position = glm::vec3(x, y, z);
	}

	void updateScale(const glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0))
	{
		this->scale = scale;
	}

	void updateModelMatrix()
	{
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, position);
		model = glm::rotate(model, glm::radians(rotationAngle), rotationAxis);
		model = glm::scale(model, scale);
		shader->setMat4("model", glm::value_ptr(model));
	}

	void renderObject() const
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sceneObjInfo.textureId);
		glBindVertexArray(sceneObjInfo.VAO);
		glDrawArrays(GL_TRIANGLES, 0, sceneObjInfo.numVertices);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void rotateX()
	{
		this->rotationAngle = (GLfloat)glfwGetTime() * 10;
		this->rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
	}

	void rotateY()
	{
		this->rotationAngle = (GLfloat)glfwGetTime() * 10;
		this->rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	}

	void rotateZ()
	{
		this->rotationAngle = (GLfloat)glfwGetTime() * 10;
		this->rotationAxis = glm::vec3(0.0f, 0.0f, 1.0f);
	}

	void translateX(int direction = 1)
	{
		this->position += glm::vec3((direction * translationSpeed), 0.0f, 0.0f);
	}

	void translateY(int direction = 1)
	{
		this->position += glm::vec3(0.0f, (direction * translationSpeed), 0.0f);
	}

	void translateZ(int direction = 1)
	{
		this->position += glm::vec3(0.0f, 0.0f, (direction * translationSpeed));
	}

private:
	float x, y, z;
	string objFilePath;

	glm::vec3 position;
	glm::vec3 scale;
	float rotationAngle;
	glm::vec3 rotationAxis;
	float translationSpeed;
	Shader* shader;
};