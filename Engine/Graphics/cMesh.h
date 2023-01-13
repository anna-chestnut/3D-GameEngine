#pragma once

// Includes
//=========

#if defined( EAE6320_PLATFORM_D3D )
	#include "cVertexFormat.h"
	#include "Direct3D/Includes.h"
#endif

#include "VertexFormats.h"
#include "Engine/Assets/ReferenceCountedAssets.h"
#include <Engine/Concurrency/cMutex.h>
#include <Engine/Logging/Logging.h>

#include <Engine/Results/Results.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <External/Lua/Includes.h>
#include <iostream>

namespace eae6320
{
	namespace Graphics
	{
		class Mesh {

			// Interface
			// =========
		public:


			// Access 
			// ------
			//void SetTriangleCount(unsigned int i_triangleCount) { m_triangleCount = i_triangleCount; };
			void SetVertexCount(uint16_t i_vertexCount) { m_vertexCount = i_vertexCount; };
			void SetVertexData(eae6320::Graphics::VertexFormats::sVertex_mesh* i_vertexData) { m_vertexData = i_vertexData; };
			void SetIndexCountToRender(uint16_t i_indexCountToRender) { m_indexCountToRender = i_indexCountToRender; };
			void SetIndexData(uint16_t* i_indexData) { m_indexData = i_indexData; };

#if defined( EAE6320_PLATFORM_D3D )
			void SetVertexFormat(eae6320::Graphics::cVertexFormat* i_vertexFormat);
#endif
			// Initialize / Clean Up
			// ---------------------
			static cResult Load(const char* const i_path, eae6320::Graphics::VertexFormats::sVertex_mesh*& i_vertexData, uint16_t*& i_indexData, Mesh*& o_mesh);

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(Mesh)

			// Draw
			// ----
			void Draw();

			// Reference Counting
			//-------------------
			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()

			// Data
			// ====

		private:

			Mesh();
			~Mesh();

			// Initialize / Clean Up
			// ---------------------
			eae6320::cResult InitializeGeometry();
			eae6320::cResult CleanUp();

			EAE6320_ASSETS_DECLAREREFERENCECOUNT()
			uint16_t m_vertexCount;
			uint16_t m_indexCountToRender;
			eae6320::Graphics::VertexFormats::sVertex_mesh* m_vertexData;
			uint16_t* m_indexData;

			static eae6320::cResult ReadBinaryData(const char* const i_path, uint16_t*& i_indexData,
				eae6320::Graphics::VertexFormats::sVertex_mesh*& i_meshVertexData, uint16_t& i_indexDataCount, uint16_t& i_vertexDataCount);

#if defined( EAE6320_PLATFORM_D3D )
			eae6320::Graphics::cVertexFormat* m_vertexFormat;
			// A vertex buffer holds the data for each vertex
			ID3D11Buffer* m_vertexBuffer;
			// Index buffer 
			ID3D11Buffer* m_indexBuffer;
#elif defined( EAE6320_PLATFORM_GL )
			// A vertex buffer holds the data for each vertex
			GLuint m_vertexBufferId;
			// A vertex array encapsulates the vertex data as well as the vertex input layout
			GLuint m_vertexArrayId;
			GLuint m_indexBufferId;
#endif
		};
	}
}