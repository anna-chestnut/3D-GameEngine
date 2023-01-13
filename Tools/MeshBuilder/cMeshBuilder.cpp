// Includes
//=========

#include "cMeshBuilder.h"

#include <Tools/AssetBuildLibrary/Functions.h>
#include <Engine/Platform/Platform.h>
#include <stdio.h>

// Inherited Implementation
//=========================

// Build
//------

eae6320::cResult eae6320::Assets::cMeshBuilder::Build(const std::vector<std::string>& i_arguments)
{
	auto result = eae6320::Results::Success;

	std::string errorMessage;
	// There are many reasons that a source should be rebuilt,
	// and so even if the target already exists it should just be written over
	constexpr auto noErrorIfTargetAlreadyExists = false;
	// Since we rely on timestamps to determine when a target was built
	// its file time should be updated when the source gets copied
	constexpr auto updateTheTargetFileTime = true;

	if (!(result = eae6320::Platform::CopyFile(m_path_source, m_path_target, noErrorIfTargetAlreadyExists, updateTheTargetFileTime, &errorMessage)))
	{
		eae6320::Assets::OutputErrorMessageWithFileInfo(m_path_source, errorMessage.c_str());
		return result;
	}

	// Load Data and output binary file
	if (!(result = LoadAsset(m_path_target)))
	{
		return result;
	}

	return result;
}



// Interface
//==========

eae6320::cResult eae6320::Assets::cMeshBuilder::ReadNestedTableValues(const char* const i_path, uint16_t*& i_indexData,
	eae6320::Graphics::VertexFormats::sVertex_mesh*& i_meshVertexData, unsigned int& i_indexDataCount, unsigned int& i_vertexDataCount)
{
	// The LoadAsset() function does _exactly_ what was shown
	// in the LoadTableFromFile examples.
	// After the table is loaded at the top of the stack, though,
	// LoadTableValues() is called,
	// so that is the function you should pay attention to.

	auto result = eae6320::Results::Success;

	//constexpr auto* const path = i_dataPath;
	if (!(result = LoadAsset(i_path)))
	{
		return result;
	}

	return result;
}




// Helper Function Definitions
//============================

eae6320::cResult eae6320::Assets::cMeshBuilder::LoadMeshTableValues(lua_State& io_luaState, std::string i_path)
{
	auto result = eae6320::Results::Success;

	if (!(result = LoadTableValues_indices(io_luaState, i_path)))
	{
		return result;
	}
	if (!(result = LoadTableValues_vertices(io_luaState, i_path)))
	{
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Assets::cMeshBuilder::LoadTableValues_indices(lua_State& io_luaState, std::string i_path)
{
	auto result = eae6320::Results::Success;

	// Right now the asset table is at -1.
	// After the following table operation it will be at -2
	// and the "textures" table will be at -1:
	constexpr auto* const key = "indexData";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	// It can be hard to remember where the stack is at
	// and how many values to pop.
	// There are two ways that I suggest making it easier to keep track of this:
	//	1) Use scope guards to pop things automatically
	//	2) Call a different function when you are at a new level
	// Right now we know that we have an original table at -2,
	// and a new one at -1,
	// and so we _know_ that we always have to pop at least _one_
	// value before leaving this function
	// (to make the original table be back to index -1).
	// We can create a scope guard immediately as soon as the new table has been pushed
	// to guarantee that it will be popped when we are done with it:
	eae6320::cScopeGuard scopeGuard_popTextures([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});
	// Additionally, I try not to do any further stack manipulation in this function
	// and call other functions that assume the "textures" table is at -1
	// but don't know or care about the rest of the stack
	if (lua_istable(&io_luaState, -1))
	{
		if (!(result = LoadTableValues_indices_paths(io_luaState, i_path)))
		{
			return result;
		}
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Assets::cMeshBuilder::LoadTableValues_indices_paths(lua_State& io_luaState, std::string i_path)
{
	// Right now the asset table is at -2
	// and the textures table is at -1.
	// NOTE, however, that it doesn't matter to me in this function
	// that the asset table is at -2.
	// Because I've carefully called a new function for every "stack level"
	// The only thing I care about is that the textures table that I care about
	// is at the top of the stack.
	// As long as I make sure that when I leave this function it is _still_
	// at -1 then it doesn't matter to me at all what is on the stack below it.


	auto result = eae6320::Results::Success;

	// Write indices count data
	// ------------------------

	const uint16_t indicesCount = (uint16_t)luaL_len(&io_luaState, -1);

	FILE* pFile;
	unsigned int index = i_path.find(".lua");
	std::string fileLoc = i_path.substr(0, index);
	std::string s = fileLoc + ".bin";
	pFile = fopen(s.c_str(), "wb");
	fwrite(&indicesCount, sizeof(uint16_t), 1, pFile);
	fclose(pFile);
	
	/*uint16_t tmp = 4;
	fwrite(&tmp, sizeof(uint16_t), sizeof(tmp), pFile);
	fclose(pFile);*/

	/*char buffer[] = { 'x' , 'y' , 'z' };
	pFile = fopen(s.c_str(), "ab");
	fwrite(buffer, sizeof(char), sizeof(buffer), pFile);
	fclose(pFile);*/


	// Write indices data
	// ------------------------
	uint16_t* s_indexData = new uint16_t[indicesCount];

	for (int i = 1; i <= indicesCount; ++i)
	{
		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popIndicesPath([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});

		s_indexData[i - 1] = std::stoi(lua_tostring(&io_luaState, -1));
	}

	s = fileLoc + ".bin";
	pFile = fopen(s.c_str(), "ab");
	fwrite(s_indexData, sizeof(uint16_t), indicesCount, pFile);
	fclose(pFile);

	return result;
}

eae6320::cResult eae6320::Assets::cMeshBuilder::LoadTableValues_vertices(lua_State& io_luaState, std::string i_path)
{
	auto result = eae6320::Results::Success;

	// Right now the asset table is at -1.
	// After the following table operation it will be at -2
	// and the "textures" table will be at -1:
	constexpr auto* const key = "vertexData";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	// It can be hard to remember where the stack is at
	// and how many values to pop.
	// There are two ways that I suggest making it easier to keep track of this:
	//	1) Use scope guards to pop things automatically
	//	2) Call a different function when you are at a new level
	// Right now we know that we have an original table at -2,
	// and a new one at -1,
	// and so we _know_ that we always have to pop at least _one_
	// value before leaving this function
	// (to make the original table be back to index -1).
	// We can create a scope guard immediately as soon as the new table has been pushed
	// to guarantee that it will be popped when we are done with it:
	eae6320::cScopeGuard scopeGuard_popTextures([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});
	// Additionally, I try not to do any further stack manipulation in this function
	// and call other functions that assume the "textures" table is at -1
	// but don't know or care about the rest of the stack
	if (lua_istable(&io_luaState, -1))
	{
		if (!(result = LoadTableValues_vertices_paths(io_luaState, i_path)))
		{
			return result;
		}
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Assets::cMeshBuilder::LoadTableValues_vertices_paths(lua_State& io_luaState, std::string i_path)
{
	// Right now the asset table is at -2
	// and the textures table is at -1.
	// NOTE, however, that it doesn't matter to me in this function
	// that the asset table is at -2.
	// Because I've carefully called a new function for every "stack level"
	// The only thing I care about is that the textures table that I care about
	// is at the top of the stack.
	// As long as I make sure that when I leave this function it is _still_
	// at -1 then it doesn't matter to me at all what is on the stack below it.


	auto result = eae6320::Results::Success;

	std::cout << "Iterating through every vertex path:" << std::endl;
	const auto dataCount = luaL_len(&io_luaState, -1);

	// Write indices count data
	// ------------------------

	const uint16_t vertexCount = (unsigned int)dataCount / 3;
	//const uint16_t vertexFloatCount = (uint16_t)luaL_len(&io_luaState, -1);


	FILE* pFile;
	unsigned int index = i_path.find(".lua");
	std::string fileLoc = i_path.substr(0, index);
	std::string s = fileLoc + ".bin";
	pFile = fopen(s.c_str(), "ab");
	fwrite(&vertexCount, sizeof(uint16_t), 1, pFile);
	fclose(pFile);


	// Write vertices data
	// ------------------------
	eae6320::Graphics::VertexFormats::sVertex_mesh* s_vertexData = new eae6320::Graphics::VertexFormats::sVertex_mesh[vertexCount];

	int vertexNum = 0;

	for (int i = 1; i <= dataCount; ++i)
	{
		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popIndicesPath([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});

		if (i % 3 == 1) {
			s_vertexData[vertexNum].x = std::stof(lua_tostring(&io_luaState, -1));
		}
		else if (i % 3 == 2) {
			s_vertexData[vertexNum].y = std::stof(lua_tostring(&io_luaState, -1));
		}
		else {
			s_vertexData[vertexNum].z = std::stof(lua_tostring(&io_luaState, -1));
			vertexNum++;
		}
	}
	/*
	float* s_vertexData = new float[vertexFloatCount];

	for (int i = 1; i <= vertexFloatCount; ++i)
	{
		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popIndicesPath([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});

		s_vertexData[i - 1] = std::stof(lua_tostring(&io_luaState, -1));
	}
	*/
	s = fileLoc + ".bin";
	pFile = fopen(s.c_str(), "ab");
	fwrite(s_vertexData, sizeof(eae6320::Graphics::VertexFormats::sVertex_mesh), vertexCount, pFile);
	fclose(pFile);

	return result;
}

eae6320::cResult eae6320::Assets::cMeshBuilder::LoadAsset(const char* const i_path)
{
	auto result = eae6320::Results::Success;

	// Create a new Lua state
	lua_State* luaState = nullptr;
	eae6320::cScopeGuard scopeGuard_onExit([&luaState]
		{
			if (luaState)
			{
				// If I haven't made any mistakes
				// there shouldn't be anything on the stack
				// regardless of any errors
				//EAE6320_ASSERT(lua_gettop(luaState) == 0);

				lua_close(luaState);
				luaState = nullptr;
			}
		});
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			result = eae6320::Results::OutOfMemory;
			std::cerr << "Failed to create a new Lua state" << std::endl;
			return result;
		}
	}

	// Load the asset file as a "chunk",
	// meaning there will be a callable function at the top of the stack
	const auto stackTopBeforeLoad = lua_gettop(luaState);
	{
		const auto luaResult = luaL_loadfile(luaState, i_path);
		if (luaResult != LUA_OK)
		{
			result = eae6320::Results::Failure;
			std::cerr << lua_tostring(luaState, -1) << std::endl;
			// Pop the error message
			lua_pop(luaState, 1);
			return result;
		}
	}
	// Execute the "chunk", which should load the asset
	// into a table at the top of the stack
	{
		constexpr int argumentCount = 0;
		constexpr int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
		constexpr int noMessageHandler = 0;
		const auto luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
		if (luaResult == LUA_OK)
		{
			// A well-behaved asset file will only return a single value
			const auto returnedValueCount = lua_gettop(luaState) - stackTopBeforeLoad;
			if (returnedValueCount == 1)
			{
				// A correct asset file _must_ return a table
				if (!lua_istable(luaState, -1))
				{
					result = eae6320::Results::InvalidFile;
					std::cerr << "Asset files must return a table (instead of a " <<
						luaL_typename(luaState, -1) << ")" << std::endl;
					// Pop the returned non-table value
					lua_pop(luaState, 1);
					return result;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "Asset files must return a single table (instead of " <<
					returnedValueCount << " values)" << std::endl;
				// Pop every value that was returned
				lua_pop(luaState, returnedValueCount);
				return result;
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			std::cerr << lua_tostring(luaState, -1) << std::endl;
			// Pop the error message
			lua_pop(luaState, 1);
			return result;
		}
	}

	// If this code is reached the asset file was loaded successfully,
	// and its table is now at index -1
	eae6320::cScopeGuard scopeGuard_popAssetTable([luaState]
		{
			lua_pop(luaState, 1);
		});
	result = LoadMeshTableValues(*luaState, i_path);

	return result;
}
