#include "Tank.h"
#include "camera.h"
#include "mesh.h"
#include "shaders.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wreturn-type"

#include "Template/PerlinNoise.hpp"

#pragma GCC diagnostic pop

Camera* Tmpl9::Tank::camera = nullptr;
float height;

Tmpl9::Tank::Tank()
{
}

btVector3 Tmpl9::Tank::RandomPosition() const
{
	std::vector<Vertex> vertices_ = terrain_->GetVertices();
	int random_vertex = rand() % terrain_->GetVertices().size();

	float x_ = vertices_[random_vertex].Position.x;
	float y_ = vertices_[random_vertex].Position.y;
	float z_ = vertices_[random_vertex].Position.z;

	return btVector3{x_, y_ - 9, z_};
}

void Tmpl9::Tank::Lighting(Shader2* shader, glm::vec3 pointLightPositions[4])
{
	glm::vec3 lightPos = {0.0f, 0.0f, 0.0f};

	glm::vec3 pointLightColors[] = {
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0, 0.0),
		glm::vec3(0.0f, 0.0f, 1.0f)
	};

	shader->use();

	shader->setVec3("viewPos", camera->Position);
	shader->setFloat("material.shininess", 32.0f);

	// directional light
	shader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	shader->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	shader->setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	shader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

	// point light 1
	shader->setVec3("pointLights[0].position", pointLightPositions[0]);
	shader->setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1,
	                pointLightColors[0].z * 0.1);
	shader->setVec3("pointLights[0].diffuse", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
	shader->setVec3("pointLights[0].specular", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
	shader->setFloat("pointLights[0].constant", 1.0f);
	shader->setFloat("pointLights[0].linear", 0.09f);
	shader->setFloat("pointLights[0].quadratic", 0.032f);
	// point light 2
	shader->setVec3("pointLights[1].position", pointLightPositions[1]);
	shader->setVec3("pointLights[1].ambient", pointLightColors[1].x * 0.1, pointLightColors[1].y * 0.1,
	                pointLightColors[1].z * 0.1);
	shader->setVec3("pointLights[1].diffuse", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
	shader->setVec3("pointLights[1].specular", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
	shader->setFloat("pointLights[1].constant", 1.0f);
	shader->setFloat("pointLights[1].linear", 0.09f);
	shader->setFloat("pointLights[1].quadratic", 0.032f);
	// point light 3
	shader->setVec3("pointLights[2].position", pointLightPositions[2]);
	shader->setVec3("pointLights[2].ambient", pointLightColors[2].x * 0.1, pointLightColors[2].y * 0.1,
	                pointLightColors[2].z * 0.1);
	shader->setVec3("pointLights[2].diffuse", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
	shader->setVec3("pointLights[2].specular", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
	shader->setFloat("pointLights[2].constant", 1.0f);
	shader->setFloat("pointLights[2].linear", 0.09f);
	shader->setFloat("pointLights[2].quadratic", 0.032f);
	// point light 4
	shader->setVec3("pointLights[3].position", pointLightPositions[3]);
	shader->setVec3("pointLights[3].ambient", pointLightColors[3].x * 0.1, pointLightColors[3].y * 0.1,
	                pointLightColors[3].z * 0.1);
	shader->setVec3("pointLights[3].diffuse", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
	shader->setVec3("pointLights[3].specular", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
	shader->setFloat("pointLights[3].constant", 1.0f);
	shader->setFloat("pointLights[3].linear", 0.09f);
	shader->setFloat("pointLights[3].quadratic", 0.032f);

}

Tmpl9::Tank::~Tank()
{
}
