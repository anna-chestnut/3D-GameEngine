#pragma once

#include "../Graphics/cEffect.h"
#include "../Graphics/cMesh.h"
#include "../Physics/sRigidBodyState.h"
#include "../Graphics/ConstantBufferFormats.h"

namespace eae6320
{
	namespace Gameplay
	{
		struct GameObject
		{
			eae6320::Graphics::Mesh* m_mesh;
			eae6320::Graphics::Effect* m_effect;
			bool m_visible;
			eae6320::Physics::sRigidBodyState m_rigidBodyState;
			eae6320::Graphics::ConstantBufferFormats::sDrawCall m_constantData_drawcall;
			std::string m_tag = "none";
		};
	}
}