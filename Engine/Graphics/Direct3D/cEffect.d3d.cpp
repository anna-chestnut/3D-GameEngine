#include "../cEffect.h"
#include "../sContext.h"
//#include "Includes.h"

// constructor
eae6320::Graphics::Effect::Effect() : m_vertexShader(nullptr), m_fragmentShader(nullptr) {
}

eae6320::cResult eae6320::Graphics::Effect::InitializeShadingData(std::string i_vertexShaderPath, std::string i_fragmentShaderPath, uint8_t i_renderStateBits) {

	return InitializeShaders(i_vertexShaderPath, i_fragmentShaderPath, i_renderStateBits);
}

void eae6320::Graphics::Effect::BindShadingData() {

	auto* const direct3dImmediateContext = sContext::g_context.direct3dImmediateContext;
	EAE6320_ASSERT(direct3dImmediateContext);

	// Bind the shading data
	{
		{
			constexpr ID3D11ClassInstance* const* noInterfaces = nullptr;
			constexpr unsigned int interfaceCount = 0;
			// Vertex shader
			{
				EAE6320_ASSERT((m_vertexShader != nullptr) && (m_vertexShader->m_shaderObject.vertex != nullptr));
				direct3dImmediateContext->VSSetShader(m_vertexShader->m_shaderObject.vertex, noInterfaces, interfaceCount);
			}
			// Fragment shader
			{
				EAE6320_ASSERT((m_fragmentShader != nullptr) && (m_fragmentShader->m_shaderObject.vertex != nullptr));
				direct3dImmediateContext->PSSetShader(m_fragmentShader->m_shaderObject.fragment, noInterfaces, interfaceCount);
			}
		}
		// Render state
		{
			m_renderState.Bind();
		}
	}
}

eae6320::cResult eae6320::Graphics::Effect::CleanUp() {

	CleanShaders();

	return Results::Success;
}