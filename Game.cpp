#include "Game.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wswitch"

#include <iostream>
#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <bullet/btBulletDynamicsCommon.h>
#include "ImGUI/imgui.h"
#include <X11/keysym.h>

#pragma GCC diagnostic pop

#include "camera.h"
#include "DebugDrawer.h"
#include "model.h"
#include "Scene.h"
#include "Object.h"
#include "Physics.h"
#include "Procedural.h"
#include "shaders.h"
#include "Tank.h"
#include "Terrain.h"
#include "Template/Surface.h"

btVector3 world_point_ = {0, 0, 0};

void Tmpl9::Game::Init()
{
	// camera
	camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	lastX = new float(screen->GetResolution().x / 2.0f);
	lastY = new float(screen->GetResolution().y / 2.0f);

	// ------- Set Physics Stuff -------
	physicsObj = new Physics();
	DebugDrawer::SetCamera(camera);
	Terrain::SetCamera(camera);
	Tank::SetCamera(camera);
	Physics::SetCamera(camera);

	// configure global opengl state
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders for models
	modelShader = new Shader2("Shaders/model_loading_vs.glsl", "Shaders/model_loading_fs.glsl");
	terrainShader = new Shader2("Shaders/floor_model_vs.glsl", "Shaders/floor_model_fs.glsl");
	lightShader = new Shader2("Shaders/light_vs.glsl", "Shaders/light_fs.glsl");
	bulletShader = new Shader2("Shaders/bullet_vs.glsl", "Shaders/bullet_fs.glsl");

	
	tankModel = new Model("assets/Tank/SimpleTankReduced2.obj");
	lightModel = new Model("assets/cube.obj");
	bulletModel = new Model("assets/bullet.obj");


	// Procedural generation
	procedural_ = new Procedural();

	tank_ = new Tank();

	srand(time(nullptr));

	army_name_seed = rand() % 10000 + 1;
	enemy_army_seed = rand() % 10000 + 1;
	your_army_name = procedural_->NameGenerator(army_name_seed);
	enemy_army_name = procedural_->NameGenerator(enemy_army_seed);

	your_army_name = your_army + your_army_name;
	enemy_army_name = enemy_army + enemy_army_name;
}


// ------------ Key Down Function ------------

void Tmpl9::Game::KeyDown(int key)
{
	switch (key)
	{
	case XK_a:
		key_a = true;
		break;
	case XK_d:
		key_d = true;
		break;
	case XK_w:
		key_w = true;
		break;
	case XK_s:
		key_s = true;
		break;
	case XK_Shift_L:
		key_shiftl = true;
		break;
	}
}

// ------------ Key Up Function ------------

void Tmpl9::Game::KeyUp(const int key)
{
	switch (key)
	{
	case XK_a:
		key_a = false;
		break;
	case XK_d:
		key_d = false;
		break;
	case XK_w:
		key_w = false;
		break;
	case XK_s:
		key_s = false;
		break;
	case XK_Shift_L:
		key_shiftl = false;
		break;
	}
}

// ------------ Mouse Down Function ------------

void Tmpl9::Game::MouseDown(int button)
{
	switch (button)
	{
	case 0:
		mbutton_left = true;
		break;

	case 1:
		mbutton_right = true;
		break;

	case 2:
		mbutton_middle = true;
		break;
	}
}

// ------------ Key Up Function ------------

void Tmpl9::Game::MouseUp(int button)
{
	switch (button)
	{
	case 0:
		mbutton_left = false;
		break;
	case 1:
		mbutton_right = false;
		break;
	case 2:
		mbutton_middle = false;
		break;
	}
}

void Tmpl9::Game::MouseMove(int x, int y)
{
	mousePos.x = x, mousePos.y = y;

	float xpos = static_cast<float>(x);
	float ypos = static_cast<float>(y);

	if (firstMouse)
	{
		*lastX = xpos;
		*lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - *lastX;
	float yoffset = *lastY - ypos; // reversed since y-coordinates go from bottom to top

	*lastX = xpos;
	*lastY = ypos;

	if (mbutton_right)
	{
		camera->ProcessMouseMovement(xoffset, yoffset);
	}
}


void Tmpl9::Game::Tick(float deltatime)
{
	switch (state_)
	{
	case MAIN_MENU:
		// render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

		previous_state_ = MAIN_MENU;

		ImGui::Begin("Welcome and have fun!", nullptr,
		             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		             ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse |
		             ImGuiTableFlags_NoSavedSettings);
		ImGui::SetWindowPos({0, 0});
		ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);
		ImGui::SetCursorPos({ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2});
		if (ImGui::Button("PLAY", {100, 100}))
		{
			// Transition to gameplay state
			state_ = GAME_LOOP;
		}
		ImGui::SetCursorPos({ImGui::GetIO().DisplaySize.x / 2, ImGui::GetCursorPosY()});
		if (ImGui::Button("SETTINGS", {100, 100}))
		{
			state_ = SETTINGS;
		}
		ImGui::SetCursorPos({ImGui::GetIO().DisplaySize.x / 2, ImGui::GetCursorPosY()});
		if (ImGui::Button("QUIT", {100, 100}))
		{
			should_close = true;
		}

		ImGui::End();

		ImGui::Begin("Space", nullptr,
		             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		             ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse |
		             ImGuiTableFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground);
		ImGui::SetWindowPos({0, 0});

		if (ImGui::BeginCombo("Instructions", "Instructions"))
		{
			ImGui::TextColored({1.0, 0.0, 0.0, 1.0}, "HOW TO PLAY:");
			ImGui::TextColored({1.0, 0.5, 0.0, 1.0}, "RIGHT MOUSE BUTTON + MOUES DRAG: ");
			ImGui::SameLine();
			ImGui::Text("Look around with the camera.");
			ImGui::TextColored({1.0, 0.5, 0.0, 1.0}, "WASD Keys: ");
			ImGui::SameLine();
			ImGui::Text("Move around the world with the camera.");
			ImGui::TextColored({1.0, 0.5, 0.0, 1.0}, "MIDDLE MOUSE BUTTON: ");
			ImGui::SameLine();
			ImGui::Text("Select a tank ");
			ImGui::SameLine();
			ImGui::TextColored({1.0, 0.0, 0.0, 1.0}, "!Make sure your mouse cursor is in the center of the screen or the selection won't work!");
			ImGui::TextColored({1.0, 0.5, 0.0, 1.0}, "LEFT MOUSE BUTTON: ");
			ImGui::SameLine();
			ImGui::Text("Click somewhere on the map and the tank will go there.");
			ImGui::SameLine();
			ImGui::TextColored({1.0, 0.0, 0.0, 1.0}, "!Make sure your mouse cursor is in the center of the screen or the tank won't go precisely where you want it to go!");
			ImGui::TextColored({1.0, 0.5, 0.0, 1.0}, "LEFT MOUSE BUTTON + LEFT SHIFT: ");
			ImGui::SameLine();
			ImGui::Text("Click somewhere on the map and the tank shoots a bullet.");
			ImGui::SameLine();
			ImGui::TextColored({1.0, 0.0, 0.0, 1.0}, "!Make sure your mouse cursor is in the center of the screen or bullet won't go exactly where you want it to go, also make sure you have a tank selected to shoot!");
			ImGui::Text("The enemy tanks are colored in yellow, your tanks are green. Defeat all the enemy tanks to win.");
			ImGui::EndCombo();
		}

		ImGui::End();

		break;
	case SETTINGS:
		// render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

		ImGui::Begin("Settings", nullptr,
		             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		             ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse |
		             ImGuiTableFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground);
		ImGui::SetWindowPos({0, 0});
		ImGui::SliderInt("Number of tanks", &tank_num, 5, 20);
		if (ImGui::Button("BACK", {100, 50}))
		{
			state_ = previous_state_;
		}
		ImGui::End();
		break;

	case GAME_LOOP:
		previous_state_ = GAME_LOOP;

		physicsObj->Step(deltatime);

	// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

	// transformations
		glm::mat4 view = camera->GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
		                                        static_cast<float>(screen->GetResolution().x) / static_cast<float>(
			                                        screen->
			                                        GetResolution().y),
		                                        0.1f, 100.0f);

		ImGui::Begin("Space", nullptr,
		             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		             ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse |
		             ImGuiTableFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground);
		ImGui::SetWindowPos({0, 0});
		if (ImGui::Button("Settings"))
		{
			state_ = SETTINGS;
		}

		ImGui::SameLine();

		if (ImGui::BeginCombo("Exit", "Exit"))
		{
			if (ImGui::Button("Exit to Main Menu")) { state_ = MAIN_MENU; }
			if (ImGui::Button("Exit to Desktop")) { should_close = true; }
			ImGui::EndCombo();
		}
		ImGui::End();

		ImGui::Begin("Toolbox");
		if (ImGui::Button("Generate Level"))
		{
			if (initialization == true)
			{
				for (int i = 0; i < objects_.size(); ++i)
				{
					physicsObj->DeleteObject(objects_[i]->GetRigidBody());
				}
				objects_.clear();
				for (const Scene* tank : tanks_)
				{
					delete tank;
				}
				delete terrain_;
				tanks_.clear();
				initialization = false;
			}

			// ----------------- NAMES
			army_name_seed = rand() % 10000 + 1;
			enemy_army_seed = rand() % 10000 + 1;
			your_army_name = procedural_->NameGenerator(army_name_seed);
			enemy_army_name = procedural_->NameGenerator(enemy_army_seed);

			your_army_name = your_army + your_army_name;
			enemy_army_name = enemy_army + enemy_army_name;


			terrain_ = new Terrain(terrainShader);

			// ----------------- TANKS
			for (int i = 0; i < tank_num; ++i)
			{
				// Create Object
				btVector3 tankpos = tank_->RandomPosition();
				btVector3 position = tankpos;

				btQuaternion rotation = {0.0f, 0.0f, 0.0f, 1.0f};
				btVector3 scale(0.35, 0.35, 0.35);
				btCollisionShape* shape = new btBoxShape(btVector3(1.6f, 1.0f, 3.3f) * scale);
				shape->setMargin(0.1); // set the collision margin to 0.1

				// tanks
				objects_.push_back(physicsObj->CreateObject(1.0f, position, rotation, shape, 1, 1));

				// Create Model
				auto tank = new Scene();
				tanks_.push_back(tank);
			}
			for (int i = 0; i < objects_.size() / 2; ++i)
			{
				objects_[i]->is_enemy_tank = true;
			}

			for (int i = 0; i < objects_.size(); ++i)
			{
				objects_[i]->GetTankName(your_army_name, enemy_army_name);
			}

			// ----------------- FLOOR
			// Create floor rigid body object
			btVector3 position_terrain = {0.0f, -10.0f, 0.0f};
			btQuaternion rotation_terrain = {0.0f, 0.0f, 0.0f, 1.0f};
			btCollisionShape* shape_terrain = terrain_->GetShape();
			shape_terrain->setMargin(0.1); // set the collision margin to 0.01
			terrain_obj_ = physicsObj->CreateObject(0.0f, position_terrain, rotation_terrain, shape_terrain, 1, 1);
			initialization = true;
		}

		if (initialization == true)
		{
			const char* cstr_your_army_name = your_army_name.c_str();
			const char* cstr_enemy_army_name = enemy_army_name.c_str();
			ImGui::Text(cstr_your_army_name);
			ImGui::Text(cstr_enemy_army_name);


			// -------------- LIGHT CUBE MODELS --------------

			// Create lights
			if (lights_number < 4)
			{
				btVector3 position_light = {-10.0f + x_offset, 20.0f, 0.0f};
				btQuaternion rotation_light = {0.0f, 0.0f, 0.0f, 1.0f};
				btCollisionShape* shape_light = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
				lights_.push_back(physicsObj->CreateObject(0.0f, position_light, rotation_light, shape_light, 0, 0));

				glm::vec3 lightPos = {-10.0f + x_offset, 0.0f, 0.0f};

				// Create model
				auto light = new Scene();
				light_models_.push_back(light);

				x_offset += 10;
				lights_number++;
			}

			// positions of the point lights
			glm::vec3 pointLightPositions[4] = {
				glm::vec3(-10.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(10.0f, 0.0f, 0.0f),
				glm::vec3(20.0f, 0.0f, 0.0f)
			};

			// Draw Model
			for (int i = 0; i < light_models_.size(); ++i)
			{
				glm::mat4 model_light = lights_[i]->GetTransform();
				light_models_[i]->DrawModel(lightShader, lightModel, view, projection, model_light, false, false);
			}
			tank_->Lighting(modelShader, pointLightPositions);


			// --------------- Tanks Update ----------
			// Tank selection with Ray Cast
			physicsObj->SelectionTank(projection, view, screen, mbutton_middle, mousePos, objects_);
			physicsObj->SelectionGoTo(projection, view, screen, mbutton_left, mousePos);

			if (physicsObj->raycast_)
			{
				world_point_ = physicsObj->world_hit_point_;
			}

			// Draw Model
			for (int i = 0; i < objects_.size(); ++i)
			{
				if (objects_[i]->selected)
				{
					ImGui::Text(objects_[i]->tank_name.c_str());


					if (mbutton_left && key_shiftl && !objects_[i]->is_enemy_tank)
					{
						btVector3 position_bullet = objects_[i]->GetRigidBody()->getWorldTransform().getOrigin();
						btQuaternion rotation_bullet = {0.0f, 0.0f, 0.0f, 1.0f};
						btCollisionShape* shape_bullet = new btSphereShape(static_cast<btScalar>(0.2));
						bullets_.push_back(
							physicsObj->CreateObject(0.1f, position_bullet, rotation_bullet, shape_bullet, 1, 1));

						// Create model
						auto bullet = new Scene();
						bullet_models_.push_back(bullet);
					}

					for (int y = 0; y < bullet_models_.size(); ++y)
					{
						glm::mat4 model_bullet = bullets_[y]->GetTransform();
						bullet_models_[y]->DrawModel(bulletShader, bulletModel, view, projection, model_bullet, false,
						                             false);
					}


					if (physicsObj->raycast_ && !objects_[i]->is_enemy_tank)
					{
						objects_[i]->GetRigidBody()->setLinearVelocity(
							(world_point_ - objects_[i]->GetRigidBody()->getWorldTransform().getOrigin()).normalize());
						objects_[i]->GetRigidBody()->activate();
					}
				}


				for (int z = 0; z < bullets_.size(); ++z)
				{
					if (bullets_[z]->bullet_draw)
					{
						bullets_[z]->GetRigidBody()->applyCentralImpulse(
							(world_point_ - bullets_[z]->GetRigidBody()->getWorldTransform().getOrigin()).
							normalize());
						bullets_[z]->GetRigidBody()->activate();
						bullets_[z]->bullet_draw = false;
					}

					if (objects_[i]->is_enemy_tank && bullets_[z]->bullet_check_once)
					{
						physicsObj->dynamics_world_->contactPairTest(bullets_[z]->GetRigidBody(),
						                                             objects_[i]->GetRigidBody(), callback);
						if (callback.collision)
						{
							objects_[i]->show_tank_ = false;
							bullets_[z]->bullet_check_once = false;
						}
					}
				}


				// Draw tanks
				if (objects_[i]->show_tank_)
				{
					glm::mat4 model = objects_[i]->GetTransform();
					model = scale(model, glm::vec3(0.35f));
					tanks_[i]->DrawModel(modelShader, tankModel, view, projection, model, objects_[i]->selected,
					                     objects_[i]->is_enemy_tank);
				}
			}


			// -------------- FLOOR MODEL --------------

			// Apply lighting
			terrain_->Lighting(terrainShader, pointLightPositions);

			// Draw Model
			glm::mat4 model_terrain = terrain_obj_->GetTransform();
			//model_terrain = glm::scale(model_terrain, glm::vec3(2.0f));
			terrain_->DrawTerrain(terrainShader, projection, view, model_terrain, deltatime);
		}
		ImGui::End();


	// Move the camera
		if (key_a)
		{
			camera->ProcessKeyboard(LEFT, deltatime);
		}
		if (key_d)
		{
			camera->ProcessKeyboard(RIGHT, deltatime);
		}
		if (key_w)
		{
			camera->ProcessKeyboard(FORWARD, deltatime);
		}
		if (key_s)
		{
			camera->ProcessKeyboard(BACKWARD, deltatime);
		}

		physicsObj->DebugLineDrawer();
		break;
	}
}

void Tmpl9::Game::ShutDown()
{
	delete camera;
	delete lastX;
	delete lastY;
	delete tankModel;
	delete lightModel;
	delete bulletModel;
	delete terrain_;
	delete procedural_;
	delete tank_;
	delete modelShader;
	delete terrainShader;
	delete lightShader;
	delete physicsObj;
	delete bulletShader;

	for (const Scene* light : light_models_)
	{
		delete light;
	}

	for (const Scene* tank : tanks_)
	{
		delete tank;
	}

	for (const Scene* bullet : bullet_models_)
	{
		delete bullet;
	}

	// de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}
