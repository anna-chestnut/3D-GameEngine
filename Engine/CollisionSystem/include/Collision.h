#ifndef EAE6320_COLLISION_H
#define EAE6320_COLLISION_H

#include "Engine/Results/Results.h"
#include "ColliderBase.h"
#include "SphereCollider.h"
#include "AABBCollider.h"
#include "OBBCollider.h"
#include "Octree.h"

class eae6320::Gameplay::GameObject;

namespace eae6320{
	namespace Collision {
		
		cResult AddCollider(eae6320::Collision::ColliderBase* s_Collider);
		cResult Update(const float i_secondCountToIntegrate); //Collision is detected here.
		cResult Initialize();
		cResult CleanUp();
		bool CheckCollision(const ColliderBase* i_collider0, const ColliderBase* i_collider1, const float i_secondCountToIntegrate);

		//helper
		//--------------

		bool BoxBox(const AABBCollider* i_collider0, const AABBCollider* i_collider1, const float i_secondCountToIntegrate);
		bool BoxSphere(const AABBCollider* i_collider0, const SphereCollider* i_collider1, const float i_secondCountToIntegrate);
		bool SphereSphere(const SphereCollider* i_collider0, const SphereCollider* i_collider1, const float i_secondCountToIntegrate);

		bool BoxBoxOBB(const AABBCollider* i_collider0, const OBBCollider* i_collider1, const float i_secondCountToIntegrate);
		bool BoxOBBBoxOBB(const OBBCollider* i_collider0, const OBBCollider* i_collider1, const float i_secondCountToIntegrate);
		bool BoxSphereOBB(const OBBCollider* i_collider0, const SphereCollider* i_collider1, const float i_secondCountToIntegrate);
	}

}

#endif //EAE6320_COLLISION_H