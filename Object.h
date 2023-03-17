#pragma once
#include <string>
#include <glm/fwd.hpp>

class btCollisionShape;
class btMotionState;
class btRigidBody;
class btVector3;
class btQuaternion;

namespace Tmpl9
{
	class Object
	{
	public:
		btRigidBody* rigid_body_;
		btCollisionShape* shape_;
		btMotionState* motion_state_;
		bool selected = false;

		Object(float mass, const btVector3 &position, const btQuaternion &rotation, btCollisionShape* shape);
		btRigidBody* GetRigidBody() const {return rigid_body_;}
		glm::mat4 GetTransform();
		void GetTankName(std::string your_army, std::string enemy_army);
		~Object();

		std::string tank_name;
		std::string tank_text = "Tank: ";
		bool is_enemy_tank = false;
		bool bullet_draw = true;
		bool show_tank_ = true;
		bool bullet_check_once = true;

	private:
	};
}

