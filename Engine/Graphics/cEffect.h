#pragma once

#include <Engine/Results/cResult.h>
#include "cShader.h"
#include "cRenderState.h"
#include "Engine/Assets/ReferenceCountedAssets.h"


namespace eae6320 {
	namespace Graphics {
		class Effect {

			// Interface
			// =========
		public:

			// Initialize / Clean Up
			static cResult Load(std::string i_vertexShaderPath, std::string i_fragmentShaderPath, Effect*& o_effect, uint8_t i_renderStateBits);

			eae6320::cResult InitializeShadingData(std::string i_vertexShaderPath, std::string i_fragmentShaderPath, uint8_t i_renderStateBits);
			eae6320::cResult InitializeShaders(std::string i_vertexShaderPath, std::string i_fragmentShaderPath, uint8_t i_renderStateBits);
			void CleanShaders();
			eae6320::cResult CleanUp();
			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(Effect)

			// Bind
			// ----
			void BindShadingData();

			// Reference Counting
			//-------------------
			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()

			// Data
			// ====
		private:

			Effect();
			~Effect();

			eae6320::Graphics::cShader* m_vertexShader;
			eae6320::Graphics::cShader* m_fragmentShader;
			eae6320::Graphics::cRenderState m_renderState;
#if defined( EAE6320_PLATFORM_GL )
			GLuint m_programId = 0;
#endif
			// Reference Counting
			// ------------------
			EAE6320_ASSETS_DECLAREREFERENCECOUNT()
		};
	}
}