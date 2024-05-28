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

#include "../Common/include/Shader.h"
#include "SceneObject.cpp"
//#include "Mesh.h"

// Dimensões da janela
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Variáveis de controle de rotação
bool rotateX = false, rotateY = false, rotateZ = false;

// Variáveis de controle de translação
bool translateX = false, translateY = false, translateZ = false;
int translateDirection = 0;

// Variável de controle de escala
float scale = 1.0;

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
	case(GLFW_KEY_W):
		translateX = false;
		translateY = false;
		translateZ = true;
		translateDirection = 1;
		break;
	case(GLFW_KEY_S):
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
}

// Reseta variáveis de controle de translação
void resetTranslationVariables() {
	translateX = false;
	translateY = false;
	translateZ = false;
	translateDirection = 0;
}

// Cria e retorna um vetor de objetos da cena, representando cubos, distribuídos horizontalmente, com base no número fornecido (numCubes)
std::vector<SceneObject> generateCubes(int numCubes, GLuint vertexArrayObject, int numVertices, Shader* shader) {
	std::vector<SceneObject> cubes;

	const float horizontalSpacing = 2.75f;

	for (int i = 0; i < numCubes; ++i)
	{
		float xPosition = 0.0f;

		if (i % 2 == 0)
			xPosition = (-horizontalSpacing) * (i / 2);
		else
			xPosition = (horizontalSpacing) * ((i / 2) + 1);

		cubes.push_back(SceneObject(vertexArrayObject, numVertices, shader, glm::vec3(xPosition, 0.0, 0.0)));
	}

	return cubes;
}

// Função para ler o arquivo OBJ e extrair os dados de vértices e índices
bool readOBJFile(const std::string& filepath, std::vector<glm::vec3>& vertices, std::vector<GLuint>& indices, std::vector<GLfloat>& vbuffer) {
	// Vetor de cores para mapeamento de cores normais
	std::vector<glm::vec3> colors = {
		glm::vec3(1.0f, 0.0f, 0.0f),   // Vermelho
		glm::vec3(0.0f, 1.0f, 0.0f),   // Verde
		glm::vec3(0.0f, 0.0f, 1.0f),   // Azul
		glm::vec3(1.0f, 1.0f, 0.0f),   // Amarelo
		glm::vec3(1.0f, 0.0f, 1.0f),   // Magenta
		glm::vec3(0.0f, 1.0f, 1.0f)    // Ciano
	};

	// Abrindo o arquivo OBJ
	std::ifstream inputFile(filepath);
	if (!inputFile.is_open()) {
		std::cerr << "Erro ao abrir o arquivo OBJ: " << filepath << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(inputFile, line)) {
		std::istringstream ssline(line);
		std::string word;
		ssline >> word;

		if (word == "v") {
			glm::vec3 v;
			ssline >> v.x >> v.y >> v.z;
			vertices.push_back(v);
		}
		else if (word == "f") {
			std::string tokens[3];
			ssline >> tokens[0] >> tokens[1] >> tokens[2];

			for (int i = 0; i < 3; ++i) {
				int posLastValue = tokens[i].find_last_of('/');
				std::string lastValue = tokens[i].substr(posLastValue + 1);
				int normal = std::stoi(lastValue);
				glm::vec3 normalColors = colors[normal - 1];

				int pos = tokens[i].find("/");
				std::string token = tokens[i].substr(0, pos);
				int index = std::atoi(token.c_str()) - 1;
				indices.push_back(index);

				vbuffer.push_back(vertices[index].x);
				vbuffer.push_back(vertices[index].y);
				vbuffer.push_back(vertices[index].z);

				vbuffer.push_back(normalColors.r);
				vbuffer.push_back(normalColors.g);
				vbuffer.push_back(normalColors.b);
			}
		}
	}

	inputFile.close();
	return true;
}

// Função para inicializar os buffers de vértices e arrays de vértices (VAO e VBO)
bool initializeBuffers(GLuint& VBO, GLuint& VAO, const std::vector<GLfloat>& vbuffer) {
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(GLfloat), vbuffer.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Especificando os atributos do vértice
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

// Função principal para carregar um arquivo OBJ e inicializar os buffers de vértices e arrays de vértices (VAO e VBO)
int loadSimpleOBJ(const std::string& filepath, int& numVertices) {
	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;
	std::vector<GLfloat> vbuffer;

	if (!readOBJFile(filepath, vertices, indices, vbuffer)) {
		std::cerr << "Erro ao ler o arquivo OBJ: " << filepath << std::endl;
		return -1;
	}

	numVertices = vbuffer.size() / 6;

	GLuint VBO, VAO;
	if (!initializeBuffers(VBO, VAO, vbuffer)) {
		std::cerr << "Erro ao inicializar os buffers de vértices e arrays de vértices." << std::endl;
		return -1;
	}

	return VAO;
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

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Compilando e buildando o programa de shader
	Shader shader("VShader.vs", "FShader.fs");
	glUseProgram(shader.ID);

	//Matriz de view
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	shader.setMat4("view", value_ptr(view));

	//Matriz de projeção perspectiva - definindo o volume de visualização (frustum)
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	shader.setMat4("projection", glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);

	int numVertices;
	GLuint VAO = loadSimpleOBJ("../3D_models/Cube/cube.obj", numVertices);

	int numCubes = 7;
	std::vector<SceneObject> cubes = generateCubes(numCubes, VAO, numVertices, &shader);

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

		for (int i = 0; i < cubes.size(); ++i)
		{
			if (rotateX)
				cubes[i].rotateX();
			else if (rotateY)
				cubes[i].rotateY();
			else if (rotateZ)
				cubes[i].rotateZ();

			if (translateX)
				cubes[i].translateX(translateDirection);
			else if (translateY)
				cubes[i].translateY(translateDirection);
			else if (translateZ)
				cubes[i].translateZ(translateDirection);

			cubes[i].setScale(glm::vec3(scale, scale, scale));
			cubes[i].updateModelMatrix();
			cubes[i].renderObject();
		}

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}