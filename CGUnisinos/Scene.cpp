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
	std::vector<SceneObj> sceneObject;

	Scene(string jsonFilePath, Shader* shader)
		: jsonFilePath(jsonFilePath), shader(shader)
    {
		loadObjectsFromJSON(jsonFilePath);
    }

private:
	Shader* shader;
	string jsonFilePath;

	void loadObjectsFromJSON(const std::string& jsonFilePath) {
		std::vector<SceneObjAux> sceneObjectsAux;

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

		for (const auto& obj : sceneObjectsAux)
		{
			SceneObj sceneObj = SceneObj(obj.x, obj.y, obj.z, obj.objFilePath, shader);
			sceneObject.push_back(sceneObj);
		}
	}
};