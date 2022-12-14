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
	ObjModel model;
	Transform transform;
	bool isSelected;
	glm::vec3 stencilColor;
	int index; //instance index --- really want a static for keys

	GameObject(ObjModel model, glm::vec3 position)
	{
		this->model = model;
		this->transform = {
			position,
			glm::vec3(0),
			glm::vec3(0)
		};
		this->isSelected = false;
	}

	void draw(Shader shader)
	{
		glm::mat4 model_mat = glm::mat4(1.0f);
		model_mat = glm::translate(model_mat, transform.location); //can add rotation and scale stuff later
		shader.setMat4("model", model_mat);
		shader.setBool("isHighlighted", isSelected);

		//our asset model
		model.Draw(shader);
	}

	GameObject(){}
};