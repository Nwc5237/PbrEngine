#pragma once
#include <glm/glm.hpp>
#include "ObjModel.hpp"

typedef struct Transform
{
	glm::vec3 location;
	glm::vec3 rotation;
	glm::vec3 scale;
} Transform;

class GameObject
{
public:
	glm::vec3 position;
	ObjModel model;
	Transform transform;

	GameObject(ObjModel model, glm::vec3 position)
	{
		this->model = model;
		this->position = position;
		this->transform = {
			glm::vec3(0),
			glm::vec3(0),
			glm::vec3(0)
		};
	}

	void draw(Shader shader)
	{
		glm::mat4 model_mat = glm::mat4(1.0f);
		model_mat = glm::translate(model_mat, transform.location); //can add rotation and scale stuff later
		shader.setMat4("model", model_mat);

		//our asset model
		model.Draw(shader);
	}

	GameObject(){}
};