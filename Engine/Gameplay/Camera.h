#pragma once

#include "../Physics/sRigidBodyState.h"
#include "../Graphics/ConstantBufferFormats.h"

namespace eae6320
{
	namespace Gameplay
	{
		struct Camera
		{
			eae6320::Physics::sRigidBodyState m_rigidBodyState;
			float m_degree;
			float m_nearPlane;
			float m_farPlane;
		};
	}
}