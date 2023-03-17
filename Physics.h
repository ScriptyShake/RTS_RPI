#pragma once
#include <vector>
#include <glm/fwd.hpp>
#include <btVector3.h>

class btCollisionShape;
class btBroadphaseInterface;
class btCollisionDispatcher;
class btCollisionConfiguration;
class btConstraintSolver;
class btDynamicsWorld;
class btRigidBody;
class btQuaternion;
class DebugDrawer;
class btManifoldPoint;
class btCollisionObjectWrapper;

namespace Tmpl9
{
	class Object;
	class Surface;

	class Physics
	{
	public:
		Physics();
		void Step(float deltatime) const;
		static bool myCustomMaterialCombinerCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap,
		                                             int partId0, int index0,
		                                             const btCollisionObjectWrapper* colObj1Wrap, int partId1,
		                                             int index1
		);

		static void SetCamera(Camera* camera)
		{
			Physics::camera = camera;
		}

		Object* CreateObject(float mass, const btVector3& position, const btQuaternion& rotation,
		                     btCollisionShape* shape, short group, short mask);
		void DeleteObject(btRigidBody* object);
		void DebugLineDrawer() const;
		void SelectionTank(glm::mat4 projection, glm::mat4 view, const Surface* screen, bool mbutton_middle,
		               glm::ivec2 mouse_pos, std::vector<Object*>& tanks);
		void SelectionGoTo(glm::mat4 projection, glm::mat4 view, const Surface* screen, bool mbutton_middle,
		               glm::ivec2 mouse_pos);
		~Physics();
		bool test = false;

		btVector3 lastRayFrom;
		btVector3 lastRayTo;
		Object* selected_tank{};
		btVector3 world_hit_point_;

		bool raycast_ = false;

		btDynamicsWorld* dynamics_world_{};
		

	private:
		btBroadphaseInterface* broadphase_{};
		btCollisionDispatcher* dispatcher_;
		btCollisionConfiguration* collision_configuration_;
		btConstraintSolver* constraint_solver_{};
		DebugDrawer* debug_drawer_;
		std::vector<Object*> objects_{};
		static Camera* camera;
	};
}
