#pragma once
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wswitch"

#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>

#include <btVector3.h>
#include "Collision.h"

#include "Template/PerlinNoise.hpp"
#include "Template/Surface.h"
#pragma GCC diagnostic pop

class btDynamicsWorld;
class Model;
class Shader2;
class Camera;
class btRigidBody;

namespace Tmpl9
{
	class Tank;
	class Procedural;
	class Terrain;
	class Object;
	class Surface;
	class Physics;
	class Scene;

	/**
	 * \brief main game class
	 */
	class Game
	{
	public:
		Game()
		{
		}

		/**
		 * \brief first frame
		 */
		void Init();
		/**
		 * \brief frame tick
		 * \param deltatime elapsed time
		 */
		void Tick(float deltatime);
		/**
		 * \brief shutdown
		 */
		void ShutDown();

		/**
		 * \brief mouse button got released
		 * \param button button that was released
		 */
		void MouseUp(int button);

		/**
		 * \brief mouse button got pressed
		 * \param button button that got pressed
		 */
		void MouseDown(int button);

		/**
		 * \brief mouse moved
		 * \param x 
		 * \param y 
		 */
		void MouseMove(int x, int y);

		/**
		 * \brief key got pressed
		 * \param key pressed key
		 */
		void KeyUp(int key);

		/**
		 * \brief key got released
		 * \param key released key
		 */
		void KeyDown(int key);

		/**
		 * \brief get the game's screen
		 * \return screen
		 */
		Surface* GetScreen() { return screen; }
		/**
		 * \brief set the game's screen
		 * \param screen 
		 */
		void SetScreen(Surface* screen) { this->screen = screen; }

		bool key_a;
		bool key_d;
		bool key_w;
		bool key_s;
		bool key_shiftl;
		bool mbutton_left; // checks if left mouse button was pressed
		bool mbutton_right; // checks if right mouse button was pressed
		bool mbutton_middle; // checks if middle mouse button was pressed

		bool should_close = false;
		bool initialization = false;

		unsigned terrain_seed = 1;
		int army_name_seed = 1;
		int enemy_army_seed = 1;

		std::string your_army_name;
		std::string enemy_army_name;

		std::string your_army = "Your army: ";
		std::string enemy_army = "Enemy army: ";

		int x_offset = 0;
		int lights_number = 0;
		int terrains_number = 0;
		int tank_num = 10;

		btVector3 world_point_ = {0, 0, 0};

		const siv::PerlinNoise::seed_type seed = rand() % 100000;

		ContactResultCallback callback;

	private:
		glm::ivec2 mousePos{};
		Surface* screen = nullptr;

		unsigned int texture1, texture2;
		unsigned int VBO, VAO;

		int width, height, nrChannels;
		const float cameraSpeed = 2.5; // adjust accordingly
		float* lastX;
		float* lastY;

		bool InitDone;
		bool firstMouse = true;

		// camera
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		Shader2* modelShader;
		Shader2* terrainShader;
		Shader2* lightShader;
		Shader2* bulletShader;

		Camera* camera;

		Model* tankModel;
		Model* lightModel;
		Model* bulletModel;

		Physics* physicsObj;
		Terrain* terrain_;
		Object* terrain_obj_;
		Procedural* procedural_;
		Tank* tank_;

		std::vector<Scene*> tanks_;
		std::vector<Scene*> light_models_;
		std::vector<Scene*> bullet_models_;
		std::vector<Object*> objects_;
		std::vector<Object*> lights_;
		std::vector<Object*> bullets_;

		btDynamicsWorld* world_;

		enum Levels { ONE, TWO, THREE };

		Levels levels_ = ONE;

		enum States { MAIN_MENU, GAME_LOOP, SETTINGS };

		States state_ = MAIN_MENU;
		States previous_state_;
	};
}
