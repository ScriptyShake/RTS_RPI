#include "Terrain.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wreturn-type"

#include <array>
#include <algorithm>
#include "Procedural.h"
#include "shaders.h"
#include "Game.h"
#include "camera.h"
#include "mesh.h"
#include "Template/PerlinNoise.hpp"
#include "Physics.h"

#pragma GCC diagnostic pop

Camera* Tmpl9::Terrain::camera = nullptr;
unsigned int VBO, terrainVAO;
unsigned int EBO;
glm::vec2 array_size_ = {30, 30};
std::vector<Vertex> vertices_;
std::vector<float> bullet_impl;

std::vector<unsigned int> indices_;

Tmpl9::Terrain::Terrain(Shader2* shader)
{
	physics_ = new Physics();
	const siv::PerlinNoise perlin{(unsigned)rand()};

	// Generate vertices
	vertices_.resize(array_size_.x * array_size_.y);
	bullet_impl.resize(array_size_.x * array_size_.y);

	for (int z = 0; z < array_size_.y; ++z)
	{
		for (int x = 0; x < array_size_.x; ++x)
		{
			float proc_height_ = 20 * perlin.octave2D_01((x * 0.1), (z * 0.1), 3);
			float height = pow(proc_height_, 0.7);

			vertices_[z * array_size_.x + x].Position = {x - array_size_.x*0.5, height, z - array_size_.y*0.5};
		}
	}
	
	for (int z = array_size_.y-1; z >= 0; --z)
	{
		for (int x = array_size_.x-1; x >= 0; --x)
		{
			bullet_impl[z * array_size_.x + x] = vertices_[z * array_size_.x + x].Position.y;
		}
	}
	

	shape_ = new btHeightfieldTerrainShape(array_size_.x, array_size_.y, bullet_impl.data(), 1/10, -10, 10, 1, PHY_ScalarType::PHY_FLOAT, false);

	const unsigned int numTriangles = ( array_size_.x - 1 ) * ( array_size_.y - 1 ) * 2;

	// 3 indices for each triangle in the terrain mesh
    indices_.resize( numTriangles * 3 );

	unsigned int index = 0; // Index in the index buffer
	for (unsigned int j = 0; j < (array_size_.y - 1); ++j )
    {
        for (unsigned int i = 0; i < (array_size_.x - 1); ++i )
        {
            int vertexIndex = ( j * array_size_.x) + i;
            // Top triangle (T0)
            indices_[index++] = vertexIndex;                           // V0
            indices_[index++] = vertexIndex + array_size_.x + 1;        // V3
            indices_[index++] = vertexIndex + 1;                       // V1
            // Bottom triangle (T1)
            indices_[index++] = vertexIndex;                           // V0
            indices_[index++] = vertexIndex + array_size_.x;            // V2
            indices_[index++] = vertexIndex + array_size_.x + 1;        // V3
        }
    }

	for ( unsigned int i = 0; i < indices_.size(); i += 3 )
    {
        glm::vec3 v0 = vertices_[indices_[i + 0]].Position;
        glm::vec3 v1 = vertices_[indices_[i + 1]].Position;
        glm::vec3 v2 = vertices_[indices_[i + 2]].Position;
 
        glm::vec3 normal = glm::normalize( glm::cross( v1 - v0, v2 - v0 ) );
 
        vertices_[indices_[i + 0]].Normal += normal;
        vertices_[indices_[i + 1]].Normal += normal;
        vertices_[indices_[i + 2]].Normal += normal;
    }
 
    const glm::vec3 UP( 0.0f, 1.0f, 0.0f );
    for ( unsigned int i = 0; i < vertices_.size(); ++i )
    {
        vertices_[i].Normal = glm::normalize(vertices_[i].Normal);
	}

	// load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------
    diffuseMap = loadTexture("assets/grass_diffuse.png");

	shader->use();
    shader->setInt("texture_diffuse1", 0);

	mesh_ = new Mesh(vertices_, indices_, std::vector<Texture>{{diffuseMap, "texture_diffuse"}});
}

void Tmpl9::Terrain::DrawTerrain(Shader2* shader, glm::mat4 projection, glm::mat4 view, glm::mat4 model, float deltatime)
{
	shader->use();

	// MVP Matrix
	shader->setMat4("projection", projection);
	shader->setMat4("view", view);
	shader->setMat4("model", model);
	shader->setFloat("time", deltatime);

	mesh_->Draw(*shader);
}

Tmpl9::Object* Tmpl9::Terrain::CreateTerrain()
{
	btVector3 position_terrain = {0.0f, -10.0f, 0.0f};
	btQuaternion rotation_terrain = {0.0f, 0.0f, 0.0f, 1.0f};
	//btVector3 scale(0.2, 0.2, 0.2);
	btCollisionShape* shape_terrain = shape_;
	shape_terrain->setMargin(0.1); // set the collision margin to 0.01
	Object* object_;
	object_ = physics_->CreateObject(0.0f, position_terrain, rotation_terrain, shape_terrain, 1, 1);
	return object_;
}

void Tmpl9::Terrain::Lighting(const Shader2* shader, glm::vec3 point_light_positions[4]) const
{
	constexpr glm::vec3 point_light_colors[] = {
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
	shader->setVec3("pointLights[0].position", point_light_positions[0]);
	shader->setVec3("pointLights[0].ambient", point_light_colors[0].x * 0.1, point_light_colors[0].y * 0.1,
	                point_light_colors[0].z * 0.1);
	shader->setVec3("pointLights[0].diffuse", point_light_colors[0].x, point_light_colors[0].y, point_light_colors[0].z);
	shader->setVec3("pointLights[0].specular", point_light_colors[0].x, point_light_colors[0].y, point_light_colors[0].z);
	shader->setFloat("pointLights[0].constant", 1.0f);
	shader->setFloat("pointLights[0].linear", 0.09f);
	shader->setFloat("pointLights[0].quadratic", 0.032f);
	// point light 2
	shader->setVec3("pointLights[1].position", point_light_positions[1]);
	shader->setVec3("pointLights[1].ambient", point_light_colors[1].x * 0.1, point_light_colors[1].y * 0.1,
	                point_light_colors[1].z * 0.1);
	shader->setVec3("pointLights[1].diffuse", point_light_colors[1].x, point_light_colors[1].y, point_light_colors[1].z);
	shader->setVec3("pointLights[1].specular", point_light_colors[1].x, point_light_colors[1].y, point_light_colors[1].z);
	shader->setFloat("pointLights[1].constant", 1.0f);
	shader->setFloat("pointLights[1].linear", 0.09f);
	shader->setFloat("pointLights[1].quadratic", 0.032f);
	// point light 3
	shader->setVec3("pointLights[2].position", point_light_positions[2]);
	shader->setVec3("pointLights[2].ambient", point_light_colors[2].x * 0.1, point_light_colors[2].y * 0.1,
	                point_light_colors[2].z * 0.1);
	shader->setVec3("pointLights[2].diffuse", point_light_colors[2].x, point_light_colors[2].y, point_light_colors[2].z);
	shader->setVec3("pointLights[2].specular", point_light_colors[2].x, point_light_colors[2].y, point_light_colors[2].z);
	shader->setFloat("pointLights[2].constant", 1.0f);
	shader->setFloat("pointLights[2].linear", 0.09f);
	shader->setFloat("pointLights[2].quadratic", 0.032f);
	// point light 4
	shader->setVec3("pointLights[3].position", point_light_positions[3]);
	shader->setVec3("pointLights[3].ambient", point_light_colors[3].x * 0.1, point_light_colors[3].y * 0.1,
	                point_light_colors[3].z * 0.1);
	shader->setVec3("pointLights[3].diffuse", point_light_colors[3].x, point_light_colors[3].y, point_light_colors[3].z);
	shader->setVec3("pointLights[3].specular", point_light_colors[3].x, point_light_colors[3].y, point_light_colors[3].z);
	shader->setFloat("pointLights[3].constant", 1.0f);
	shader->setFloat("pointLights[3].linear", 0.09f);
	shader->setFloat("pointLights[3].quadratic", 0.032f);

	// bind diffuse map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
}

btHeightfieldTerrainShape* Tmpl9::Terrain::GetShape() const
{
	return shape_;
}

std::vector<Vertex> Tmpl9::Terrain::GetVertices()
{
	return vertices_;
}

Tmpl9::Terrain::~Terrain()
{
	delete procedural_;
}
