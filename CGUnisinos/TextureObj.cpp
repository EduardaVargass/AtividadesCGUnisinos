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

using namespace std;

class TextureObj {
public:
	string materialFileName, materialName;
	GLuint textureId;
private:
};