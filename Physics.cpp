#include "DebugDrawer.h"
#include "glm/matrix.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wswitch"

#include <bullet/btBulletDynamicsCommon.h>
#include "BulletCollision/CollisionShapes/btTriangleShape.h"

#pragma GCC diagnostic pop
#include "Physics.h"

#include "Object.h"
#include "Template/Surface.h"
#include "Game.h"
#include "camera.h"
#include "Tank.h"

Camera* Tmpl9::Physics::camera = nullptr;


Tmpl9::Physics::Physics()
{
	broadphase_ = new btDbvtBroadphase();
	collision_configuration_ = new btDefaultCollisionConfiguration();
	dispatcher_ = new btCollisionDispatcher(collision_configuration_);
	constraint_solver_ = new btSequentialImpulseConstraintSolver;
	debug_drawer_ = new DebugDrawer();
	dynamics_world_ = new btDiscreteDynamicsWorld(dispatcher_, broadphase_, constraint_solver_,
	                                              collision_configuration_);
	dynamics_world_->setGravity(btVector3(0, -9.81f, 0));
	//dynamics_world_->setDebugDrawer(debug_drawer_);	// uncomment to turn on!!!

	// somewhere in your init code you need to setup the callback
	gContactAddedCallback = myCustomMaterialCombinerCallback;
}

void Tmpl9::Physics::Step(const float deltatime) const
{
	dynamics_world_->stepSimulation(deltatime, 16, static_cast<btScalar>(1) / 60);
}

bool Tmpl9::Physics::myCustomMaterialCombinerCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap,
                                                      int partId0, int index0,
                                                      const btCollisionObjectWrapper* colObj1Wrap, int partId1,
                                                      int index1)
{
	// one-sided triangles
	if (colObj1Wrap->getCollisionShape()->getShapeType() == TRIANGLE_SHAPE_PROXYTYPE)
	{
		auto triShape = static_cast<const btTriangleShape*>(colObj1Wrap->getCollisionShape());
		const btVector3* v = triShape->m_vertices1;
		btVector3 faceNormalLs = btCross(v[1] - v[0], v[2] - v[0]);
		faceNormalLs.normalize();
		btVector3 faceNormalWs = colObj1Wrap->getWorldTransform().getBasis() * faceNormalLs;
		float nDotF = btDot(faceNormalWs, cp.m_normalWorldOnB);
		if (nDotF <= 0.0f)
		{
			// flip the contact normal to be aligned with the face normal
			cp.m_normalWorldOnB += -2.0f * nDotF * faceNormalWs;
		}
	}

	//this return value is currently ignored, but to be on the safe side: return false if you don't calculate friction
	return false;
}

Tmpl9::Object* Tmpl9::Physics::CreateObject(float mass, const btVector3& position, const btQuaternion& rotation,
                                            btCollisionShape* shape, short group, short mask)
{
	auto object = new Object(mass, position, rotation, shape);
	objects_.push_back(object);
	dynamics_world_->addRigidBody(object->GetRigidBody(), group, mask);
	return object;
}

void Tmpl9::Physics::DeleteObject(btRigidBody* object)
{
	for (int i = 0; i < objects_.size(); ++i)
	{
		if (object == objects_[i]->GetRigidBody())
		{
			delete objects_[i];
			objects_[i] = nullptr;
			objects_.erase(objects_.begin() + i);
			break;
		}
	}

	dynamics_world_->removeRigidBody(object);
}


void Tmpl9::Physics::DebugLineDrawer() const
{
	dynamics_world_->debugDrawWorld();
}


void Tmpl9::Physics::SelectionTank(const glm::mat4 projection, const glm::mat4 view, const Surface* screen,
                               bool mbutton_middle, glm::ivec2 mouse_pos, std::vector<Object*>& tanks)
{
	bool selection_done = false;
	if (mbutton_middle)
	{
		glm::mat4 invMat = glm::inverse(projection * view);
		auto near = glm::vec4((mouse_pos.x - (screen->GetResolution().x / 2)) / (screen->GetResolution().x / 2),
		                      -1 * (mouse_pos.y - (screen->GetResolution().y / 2)) / (screen->GetResolution().y / 2),
		                      -1, 1.0);
		auto far = glm::vec4((mouse_pos.x - (screen->GetResolution().x / 2)) / (screen->GetResolution().x / 2),
		                     -1 * (mouse_pos.y - (screen->GetResolution().y / 2)) / (screen->GetResolution().y / 2), 1,
		                     1.0);

		glm::vec4 nearResult = invMat * near;
		glm::vec4 farResult = invMat * far;
		nearResult /= nearResult.w;
		farResult /= farResult.w;
		auto dir = glm::vec3(farResult - nearResult);
		glm::vec3 normDir = normalize(dir);

		auto rayFrom = btVector3(camera->Position.x, camera->Position.y, camera->Position.z);
		btVector3 rayTo = rayFrom + btVector3(normDir.x, normDir.y, normDir.z) * btVector3(1000, 1000, 1000);

		btCollisionWorld::ClosestRayResultCallback ray_result_callback(rayFrom, rayTo);
		dynamics_world_->rayTest(rayFrom, rayTo, ray_result_callback);

		if (ray_result_callback.hasHit())
		{
			auto body = (btRigidBody*)btRigidBody::upcast(ray_result_callback.m_collisionObject);
			lastRayFrom = rayFrom;
			lastRayTo = rayTo;
			// now check what tank it is by looping through the tank object vector and check each of this local object and the tank object their GetWorldTranform->OriginPoint and if they match then we know what tank we selected!!!!

			for (int i = 0; i < tanks.size(); ++i)
			{
				tanks[i]->selected = false;
				if (body->getWorldTransform().getOrigin() == tanks[i]->GetRigidBody()->getWorldTransform().getOrigin())
				{
					//tanks[i]->GetRigidBody()->setLinearVelocity({0, 0, 50});
					tanks[i]->selected = true;
				}
			}
		}
	}
	//dynamics_world_->getDebugDrawer()->drawLine(lastRayFrom, lastRayTo, {1.0, 0.0, 0.0});	// uncomment for debug lines!
}

void Tmpl9::Physics::SelectionGoTo(glm::mat4 projection, glm::mat4 view, const Surface* screen, bool mbutton_left,
	glm::ivec2 mouse_pos)
{
	//bool selection_done = false;
	if (mbutton_left)
	{
		glm::mat4 invMat = glm::inverse(projection * view);
		auto near = glm::vec4((mouse_pos.x - (screen->GetResolution().x / 2)) / (screen->GetResolution().x / 2),
		                      -1 * (mouse_pos.y - (screen->GetResolution().y / 2)) / (screen->GetResolution().y / 2),
		                      -1, 1.0);
		auto far = glm::vec4((mouse_pos.x - (screen->GetResolution().x / 2)) / (screen->GetResolution().x / 2),
		                     -1 * (mouse_pos.y - (screen->GetResolution().y / 2)) / (screen->GetResolution().y / 2), 1,
		                     1.0);

		glm::vec4 nearResult = invMat * near;
		glm::vec4 farResult = invMat * far;
		nearResult /= nearResult.w;
		farResult /= farResult.w;
		auto dir = glm::vec3(farResult - nearResult);
		glm::vec3 normDir = normalize(dir);

		auto rayFrom = btVector3(camera->Position.x, camera->Position.y, camera->Position.z);
		btVector3 rayTo = rayFrom + btVector3(normDir.x, normDir.y, normDir.z) * btVector3(1000, 1000, 1000);

		btCollisionWorld::ClosestRayResultCallback ray_result_callback(rayFrom, rayTo);
		dynamics_world_->rayTest(rayFrom, rayTo, ray_result_callback);

		if (ray_result_callback.hasHit())
		{
			raycast_ = true;
			world_hit_point_ = ray_result_callback.m_hitPointWorld;
			lastRayFrom = rayFrom;
			lastRayTo = rayTo;
		}
	}
}


Tmpl9::Physics::~Physics()
{
	for (const Object* object : objects_)
	{
		delete object;
	}

	delete dynamics_world_;
	delete collision_configuration_;
	delete dispatcher_;
	delete broadphase_;
	delete constraint_solver_;
	delete debug_drawer_;
}
