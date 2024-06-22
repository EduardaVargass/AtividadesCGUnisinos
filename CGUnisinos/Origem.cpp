#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Common/include/stb_image.h"
#include "../Common/include/Shader.h"
#include "Camera.cpp"
#include "Scene.cpp"
#include "SceneObj.cpp"

// Dimensões da janela
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Variáveis de controle de rotação
bool rotateX = false, rotateY = false, rotateZ = false;

// Variáveis de controle de translação
bool translateX = false, translateY = false, translateZ = false;
int translateDirection = 0;

// Variável de controle de escala
float scale = 1.0;

Camera* gCamera = nullptr;

// Ajusta a escala com base na tecla pressionada.
void adjustScale(int key)
{
	float scaleFactor = 0.05;

	if (key == GLFW_KEY_KP_ADD)
		scale += scale * scaleFactor;
	else if (key == GLFW_KEY_KP_SUBTRACT)
		scale -= scale * scaleFactor;
}

// Ajusta a rotação com base na tecla pressionada.
void adjustRotation(int key)
{
	switch (key)
	{
	case(GLFW_KEY_X):
		rotateX = true;
		rotateY = false;
		rotateZ = false;
		break;
	case(GLFW_KEY_Y):
		rotateX = false;
		rotateY = true;
		rotateZ = false;
		break;
	case(GLFW_KEY_Z):
		rotateX = false;
		rotateY = false;
		rotateZ = true;
		break;
	default:
		break;
	}
	
}

// Ajusta a translação com base na tecla pressionada.
void adjustTranslation(int key)
{
	switch (key)
	{
	case(GLFW_KEY_RIGHT):
		translateX = true;
		translateY = false;
		translateZ = false;
		translateDirection = 1;
		break;
	case(GLFW_KEY_LEFT):
		translateX = true;
		translateY = false;
		translateZ = false;
		translateDirection = -1;
		break;
	case(GLFW_KEY_UP):
		translateX = false;
		translateY = true;
		translateZ = false;
		translateDirection = 1;
		break;
	case(GLFW_KEY_DOWN):
		translateX = false;
		translateY = true;
		translateZ = false;
		translateDirection = -1;
		break;
	case(GLFW_KEY_I):
		translateX = false;
		translateY = false;
		translateZ = true;
		translateDirection = 1;
		break;
	case(GLFW_KEY_K):
		translateX = false;
		translateY = false;
		translateZ = true;
		translateDirection = -1;
		break;
	default:
		break;
	}
}

// Função callback acionada quando há interação com o teclado
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	adjustScale(key);
	adjustRotation(key);
	adjustTranslation(key);

	if (gCamera)
		gCamera->moveCamera(key);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (gCamera)
		gCamera->updateCameraDirection(xpos, ypos);
}

void scrollCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (gCamera)
		gCamera->scrollCamera(ypos);
}

// Reseta variáveis de controle de translação
void resetTranslationVariables() {
	translateX = false;
	translateY = false;
	translateZ = false;
	translateDirection = 0;
}

int main()
{
	// Inicialização da GLFW
	glfwInit();

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola 3D -- Eduarda!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, keyCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Falha ao inicializar o GLAD" << std::endl;
	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderizador: " << renderer << endl;
	cout << "Versão OpenGL suportada" << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Compilando e buildando o programa de shader
	Shader shader("VShader.vs", "FShader.fs");
	glUseProgram(shader.ID);

	Camera camera(&shader, width, height);
	gCamera = &camera;

	Scene scene = Scene("Scene.json", &shader);

	// Iluminação: Define a posição da fonte de luz
	shader.setVec3("light_pos", scene.lightPositionX, scene.lightPositionY, scene.lightPositionZ);
	// Iluminação: Define a cor da luz
	shader.setVec3("light_color", scene.lightColorR, scene.lightColorG, scene.lightColorB);

	glEnable(GL_DEPTH_TEST);


	// Loop da aplicação
	while (!glfwWindowShouldClose(window))
	{
		resetTranslationVariables();

		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		gCamera->updateCamera();

		for (int i = 0; i < scene.sceneObject.size(); ++i)
		{
			// Iluminação: Coeficiente de material para a luz ambiente
			shader.setFloat("ka", scene.sceneObject[i].sceneObjInfo.ka);
			// Iluminação: Coeficiente de material para a luz difusa
			shader.setFloat("kd", scene.sceneObject[i].sceneObjInfo.kd);
			// Iluminação: Coeficiente de material para a luz especular
			shader.setFloat("ks", scene.sceneObject[i].sceneObjInfo.ks);
			// Iluminação: Expoente de brilho do material
			shader.setFloat("q", scene.sceneObject[i].sceneObjInfo.ns);

			if (rotateX)
				scene.sceneObject[i].rotateX();
			else if (rotateY)
				scene.sceneObject[i].rotateY();
			else if (rotateZ)
				scene.sceneObject[i].rotateZ();

			if (translateX)
				scene.sceneObject[i].translateX(translateDirection);
			else if (translateY)
				scene.sceneObject[i].translateY(translateDirection);
			else if (translateZ)
				scene.sceneObject[i].translateZ(translateDirection);

			scene.sceneObject[i].updateScale(glm::vec3(scale, scale, scale));
			scene.sceneObject[i].updateModelMatrix();
			scene.sceneObject[i].renderObject();
		}
		
		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	for (int i = 0; i < scene.sceneObject.size(); ++i) {
		glDeleteVertexArrays(1, &scene.sceneObject[i].sceneObjInfo.VAO);
	}
	
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}