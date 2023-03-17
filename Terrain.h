#pragma once
#include <vector>

#include "Scene.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wswitch"
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#pragma GCC diagnostic pop

class Camera;

	class Mesh;
	struct Vertex;
namespace Tmpl9
{
	class Object;
	class Physics;
	class Procedural;

	class Terrain:
		public Scene
	{
	public:
		Terrain(Shader2* shader);
		static void SetCamera(Camera* camera)
		{
			Terrain::camera = camera;
		}
		void DrawTerrain(Shader2* shader, glm::mat4 projection, glm::mat4 view, glm::mat4 model, float deltatime);
		Tmpl9::Object* CreateTerrain();
		void Lighting(const Shader2* shader, glm::vec3 point_light_positions[4]) const;
		[[nodiscard]] btHeightfieldTerrainShape* GetShape() const;
		static std::vector<Vertex> GetVertices();
		~Terrain();

	private:
		float rand_float_;
		Procedural* procedural_;
		static Camera* camera;
		unsigned int diffuseMap;
		unsigned int specularMap;
		Mesh* mesh_;
		btHeightfieldTerrainShape* shape_;
		Physics* physics_;
	};
}
