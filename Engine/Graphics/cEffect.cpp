#include "cEffect.h"
#include <Engine/Platform/Platform.h>
#include <Engine/ScopeGuard/cScopeGuard.h>

// destructor
eae6320::Graphics::Effect::~Effect() {

	EAE6320_ASSERT(m_referenceCount == 0);
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}

eae6320::cResult eae6320::Graphics::Effect::Load(std::string i_vertexShaderPath, std::string i_fragmentShaderPath, Effect*& o_effect, uint8_t i_renderStateBits) {
	auto result = Results::Success;

	Platform::sDataFromFile dataFromFile;
	Effect* newEffect = nullptr;
	cScopeGuard scopeGuard([&o_effect, &result, &dataFromFile, &newEffect]
		{
			if (result)
			{
				EAE6320_ASSERT(newEffect != nullptr);
				o_effect = newEffect;
			}
			else
			{
				if (newEffect)
				{
					newEffect->DecrementReferenceCount();
					newEffect = nullptr;
				}
				o_effect = nullptr;
			}
		});

	// Allocate a new mesh
	{
		newEffect = new (std::nothrow) Effect();
		if (!newEffect)
		{
			result = Results::OutOfMemory;
			//EAE6320_ASSERTF(false, "Couldn't allocate memory for the shader %s", i_path.c_str());
			//Logging::OutputError("Failed to allocate memory for the shader %s", i_path.c_str());
			return result;
		}
	}

	// Initialize the platform-specific graphics API shader object
	if (!(result = newEffect->InitializeShadingData(i_vertexShaderPath, i_fragmentShaderPath, i_renderStateBits)))
	{
		EAE6320_ASSERTF(false, "Initialization of new effect failed");
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Graphics::Effect::InitializeShaders(std::string i_vertexShaderPath, std::string i_fragmentShaderPath, uint8_t i_renderStateBits) {

	auto result = eae6320::Results::Success;

	if (!(result = eae6320::Graphics::cShader::Load(i_vertexShaderPath,
		m_vertexShader, eae6320::Graphics::eShaderType::Vertex)))
	{
		EAE6320_ASSERTF(false, "Can't initialize shading data without vertex shader");
		return result;
	}
	if (!(result = eae6320::Graphics::cShader::Load(i_fragmentShaderPath,
		m_fragmentShader, eae6320::Graphics::eShaderType::Fragment)))
	{
		EAE6320_ASSERTF(false, "Can't initialize shading data without fragment shader");
		return result;
	}
	{
		constexpr auto renderStateBits = []
		{
			uint8_t renderStateBits = 0;

			eae6320::Graphics::RenderStates::DisableAlphaTransparency(renderStateBits);
			eae6320::Graphics::RenderStates::DisableDepthTesting(renderStateBits);
			eae6320::Graphics::RenderStates::DisableDepthWriting(renderStateBits);
			eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides(renderStateBits);

			return renderStateBits;
		}();
		if (!(result = m_renderState.Initialize(i_renderStateBits)))
		{
			EAE6320_ASSERTF(false, "Can't initialize shading data without render state");
			return result;
		}
	}

	return result;
}

void eae6320::Graphics::Effect::CleanShaders() {

	if (m_vertexShader)
	{
		m_vertexShader->DecrementReferenceCount();
		m_vertexShader = nullptr;
	}
	if (m_fragmentShader)
	{
		m_fragmentShader->DecrementReferenceCount();
		m_fragmentShader = nullptr;
	}
}