/*
	This class builds shaders
*/

//#ifndef EAE6320_CSHADERBUILDER_H
//#define EAE6320_CSHADERBUILDER_H

// Includes
//=========

#include <Tools/AssetBuildLibrary/iBuilder.h>

#include <Engine/Results/Results.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <External/Lua/Includes.h>
#include <iostream>
#include <Engine/Graphics/VertexFormats.h>


// Class Declaration
//==================

namespace eae6320
{
	namespace Assets
	{
		class cMeshBuilder final : public iBuilder
		{
			// Inherited Implementation
			//=========================

		private:

			// Build
			//------

			cResult Build(const std::vector<std::string>& i_arguments) final;

			// Load Lua Data
			// -------------

			static eae6320::cResult ReadNestedTableValues(const char* const i_path, uint16_t*& i_indexData,
				eae6320::Graphics::VertexFormats::sVertex_mesh*& i_meshVertexData, unsigned int& i_indexDataCount, unsigned int& i_vertexDataCount);

			static eae6320::cResult LoadMeshTableValues(lua_State& io_luaState, std::string i_path);

			static eae6320::cResult LoadTableValues_indices(lua_State& io_luaState, std::string i_path);
			static eae6320::cResult LoadTableValues_indices_paths(lua_State& io_luaState, std::string i_path);

			static eae6320::cResult LoadTableValues_vertices(lua_State& io_luaState, std::string i_path);
			static eae6320::cResult LoadTableValues_vertices_paths(lua_State& io_luaState, std::string i_path);


			static eae6320::cResult LoadAsset(const char* const i_path);


		};
	}
}

//#endif	// EAE6320_CSHADERBUILDER_H
