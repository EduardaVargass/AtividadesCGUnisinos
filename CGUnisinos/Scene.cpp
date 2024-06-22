#pragma once
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Common/include/stb_image.h"
#include "SceneObj.cpp"

using namespace std;
using json = nlohmann::json;

struct SceneObjAux {
	float x, y, z;
	string objFilePath;
};

class Scene
{
public:
	vector<SceneObj> sceneObject;
	float lightPositionX, lightPositionY, lightPositionZ, lightColorR, lightColorG, lightColorB;

	Scene(string jsonFilePath, Shader* shader)
		: jsonFilePath(jsonFilePath), shader(shader)
    {
		loadSceneFromJSON(jsonFilePath);
		loadObjects();
    }

private:
	Shader* shader;
	string jsonFilePath;
	std::vector<SceneObjAux> sceneObjectsAux;

	void loadSceneFromJSON(const std::string& jsonFilePath) {
		std::ifstream file(jsonFilePath);
		if (!file.is_open()) {
			std::cerr << "Não foi possível abrir o arquivo JSON: " << jsonFilePath << std::endl;
			return;
		}

		json j;
		file >> j;

		if (j.contains("objects")) {
			for (const auto& obj : j["objects"]) {
				SceneObjAux objAux;
				objAux.objFilePath = obj["objFilePath"];
				objAux.x = obj["positionX"];
				objAux.y = obj["positionY"];
				objAux.z = obj["positionZ"];
				sceneObjectsAux.push_back(objAux);
			}
		} 
		else {
			std::cerr << "Estrutura JSON inválida: 'objects' não encontrado." << std::endl;
		}

		if (j.contains("light")) {
			const auto& light = j["light"];
			if (light.contains("lightPositionX")) {
				lightPositionX = light["lightPositionX"];
			}
			if (light.contains("lightPositionY")) {
				lightPositionY = light["lightPositionY"];
			}
			if (light.contains("lightPositionZ")) {
				lightPositionZ = light["lightPositionZ"];
			}
			if (light.contains("lightColorR")) {
				lightColorR = light["lightColorR"];
			}
			if (light.contains("lightColorG")) {
				lightColorG = light["lightColorG"];
			}
			if (light.contains("lightColorB")) {
				lightColorB = light["lightColorB"];
			}
		}
		else {
			std::cerr << "Estrutura JSON inválida: 'lights' não encontrado." << std::endl;
		}
	}

	void loadObjects() {
		for (const auto& obj : sceneObjectsAux)
		{
			SceneObj sceneObj = SceneObj(obj.x, obj.y, obj.z, obj.objFilePath, shader);
			sceneObject.push_back(sceneObj);
		}
	}
};