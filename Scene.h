#pragma once
#include <glm/fwd.hpp>

class Shader2;
class Model;

namespace Tmpl9
{

	class Scene
	{
	public:
		Scene();
		void DrawModel(Shader2* shader, Model* obj, glm::mat4 view, glm::mat4 projection, glm::mat4 model, bool selected, bool is_enemy);
		unsigned int loadTexture(char const * path);
		~Scene();
	};
}

