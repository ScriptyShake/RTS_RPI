#pragma once
#include <BulletCollision/CollisionDispatch/btCollisionObjectWrapper.h>
#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <BulletCollision/NarrowPhaseCollision/btManifoldPoint.h>

namespace Tmpl9
{
	class ContactResultCallback : public btCollisionWorld::ContactResultCallback
	{
	public:
		bool collision;

		ContactResultCallback() : collision(false)
		{
		}

		btScalar addSingleResult(btManifoldPoint& cp,
		                         const btCollisionObjectWrapper* colObj0, int partId0, int index0,
		                         const btCollisionObjectWrapper* colObj1, int partId1, int index1) override
		{
			collision = true;
			return 0;
		}
	};
}
