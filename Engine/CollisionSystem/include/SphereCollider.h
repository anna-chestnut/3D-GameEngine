#include "ColliderBase.h"
namespace eae6320 {
	namespace Collision {
		class SphereCollider: public ColliderBase
		{
		public:
			SphereCollider(const Physics::sRigidBodyState* rigidBodyState, float r = 0.5f);
			void SetSize(float r);
			float GetSize() const;

		private:
			float radius;
		};
	}
}

