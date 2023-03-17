#pragma once
#include <btVector3.h>
#include "Scene.h"
#include "Terrain.h"

class Camera;

namespace Tmpl9
{
	class Tank :
		public Scene
	{
	public:
		Tank();

		static void SetCamera(Camera* camera)
		{
			Tank::camera = camera;
		}

		btVector3 RandomPosition() const;

		static void Lighting(Shader2* shader, glm::vec3 pointLightPositions[4]);
		~Tank();

	private:
		static Camera* camera;
		Terrain* terrain_;
	};
}
