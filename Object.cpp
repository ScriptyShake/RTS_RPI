#include "Object.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wswitch"
#include "btBulletDynamicsCommon.h"
#pragma GCC diagnostic pop

#include "glm/matrix.hpp"

Tmpl9::Object::Object(float mass, const btVector3& position, const btQuaternion& rotation,
                      btCollisionShape* shape) : shape_(shape)
{
	btTransform transform; // Create a transform object
	transform.setIdentity(); // Set the transform to the identity matrix (i.e., the default position and orientation)
	transform.setOrigin(position); // Set the position of the rigid body (in world coordinates)
	transform.setRotation(rotation);

	motion_state_ = new btDefaultMotionState(transform);

	btVector3 inertia = {0.0f, 0.0f, 0.0f};

	if (mass != 0)
	{
		shape_->calculateLocalInertia(mass, inertia);
	}

	btRigidBody::btRigidBodyConstructionInfo constructionInfo(mass, motion_state_, shape_, inertia);
	rigid_body_ = new btRigidBody(constructionInfo);
}

glm::mat4 Tmpl9::Object::GetTransform()
{
	btTransform trans;
	motion_state_->getWorldTransform(trans);
	float mat4[16]{0.0f};
	trans.getOpenGLMatrix(mat4);

	return {
		mat4[0], mat4[1], mat4[2], mat4[3],
		mat4[4], mat4[5], mat4[6], mat4[7],
		mat4[8], mat4[9], mat4[10], mat4[11],
		mat4[12], mat4[13], mat4[14], mat4[15],
	};
}

void Tmpl9::Object::GetTankName(std::string your_army, std::string enemy_army)
{
	if (is_enemy_tank)
	{
		int rand_num_ = rand() % 999;
		tank_name = tank_text + enemy_army + "-" + std::to_string(rand_num_);
	}
	else
	{
		int rand_num_ = rand() % 999;
		tank_name = tank_text + your_army + "-" + std::to_string(rand_num_);
	}
}

Tmpl9::Object::~Object()
{
	delete rigid_body_;
	delete motion_state_;
	delete shape_;
}
