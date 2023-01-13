#include "cMesh.h"
#include <vector>
#include <Engine/Platform/Platform.h>
#include <Engine/ScopeGuard/cScopeGuard.h>

// Initialize / Clean Up
//----------------------


// destructor
eae6320::Graphics::Mesh::~Mesh() {

	EAE6320_ASSERT(m_referenceCount == 0);
	//DecrementReferenceCount();
	//s_meshTracker.RemoveMesh(this);
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}

eae6320::cResult eae6320::Graphics::Mesh::Load(const char* const i_path, eae6320::Graphics::VertexFormats::sVertex_mesh *&i_vertexData, uint16_t *&i_indexData, Mesh*& o_mesh)
{
	
	auto result = Results::Success;
	uint16_t s_indexCount;
	uint16_t s_vertexCount;

	if (!(ReadBinaryData(i_path, i_indexData, i_vertexData, s_indexCount, s_vertexCount))) {
		Logging::OutputError("could not load Mesh from binary file");
		return result;
	}
	
	Platform::sDataFromFile dataFromFile;
	Mesh* newMesh = nullptr;
	cScopeGuard scopeGuard([&o_mesh, &result, &dataFromFile, &newMesh]
		{
			if (result)
			{
				EAE6320_ASSERT(newMesh != nullptr);
				o_mesh = newMesh;
			}
			else
			{
				if (newMesh)
				{
					newMesh->DecrementReferenceCount();
					newMesh = nullptr;
				}
				o_mesh = nullptr;
			}
		});

	// Allocate a new mesh
	{
		newMesh = new (std::nothrow) Mesh();
		if (!newMesh)
		{
			result = Results::OutOfMemory;
			//EAE6320_ASSERTF(false, "Couldn't allocate memory for the shader %s", i_path.c_str());
			//Logging::OutputError("Failed to allocate memory for the shader %s", i_path.c_str());
			return result;
		}
	}

	// Load the vertex data
	{
		if (i_vertexData == nullptr) {
			Logging::OutputError("could not load Mesh: no vertex data ");
			return Results::Failure;
		}
		else {
			newMesh->SetVertexData(i_vertexData);
			newMesh->SetVertexCount(s_vertexCount);
		}
	}
	// Load the index data
	{
		if (i_indexData == nullptr) {
			Logging::OutputError("could not load Mesh: no index data ");
			return Results::Failure;
		}
		else {
			newMesh->SetIndexData(i_indexData);
			newMesh->SetIndexCountToRender(s_indexCount);
		}
	}

	// Initialize the platform-specific graphics API shader object
	if (!(result = newMesh->InitializeGeometry()))
	{
		EAE6320_ASSERTF(false, "Initialization of new mesh failed");
		return result;
	}

	return result;
}

// Interface
//==========

eae6320::cResult eae6320::Graphics::Mesh::ReadBinaryData(const char* const i_path, uint16_t*& i_indexData,
	eae6320::Graphics::VertexFormats::sVertex_mesh*& i_meshVertexData, uint16_t& i_indexDataCount, uint16_t& i_vertexDataCount) {

	auto result = eae6320::Results::Success;

	FILE* pFile;
	long lSize;
	char* buffer;
	size_t copyResult;

	pFile = fopen(i_path, "rb");

	if (pFile == NULL) { return eae6320::Results::Failure; }

	// obtain file size:
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	// allocate memory to contain the whole file:
	buffer = (char*)malloc(sizeof(char) * lSize);
	if (buffer == NULL) { return eae6320::Results::Failure; }

	// copy the file into the buffer:
	copyResult = fread(buffer, 1, lSize, pFile);
	if (copyResult != lSize) { return eae6320::Results::Failure;  }

	auto currentOffset = reinterpret_cast<uintptr_t>(buffer);
	const auto finalOffset = currentOffset + lSize;

	// index data
	
	i_indexDataCount = *reinterpret_cast<uint16_t*>(currentOffset);
	
	currentOffset += sizeof(uint16_t);
	//i_indexData = reinterpret_cast<uint16_t*>(currentOffset);
	i_indexData = new uint16_t[i_indexDataCount];
	memcpy(i_indexData, reinterpret_cast<void*>(currentOffset), sizeof(uint16_t) * i_indexDataCount);

	// vertex data
	currentOffset += sizeof(uint16_t) * i_indexDataCount;
	i_vertexDataCount = *reinterpret_cast<uint16_t*>(currentOffset);

	currentOffset += sizeof(uint16_t);
	i_meshVertexData = new eae6320::Graphics::VertexFormats::sVertex_mesh[i_vertexDataCount];
	//i_meshVertexData = reinterpret_cast<eae6320::Graphics::VertexFormats::sVertex_mesh*>(currentOffset);
	memcpy(i_meshVertexData, reinterpret_cast<void*>(currentOffset), sizeof(eae6320::Graphics::VertexFormats::sVertex_mesh) * i_vertexDataCount);

	/* the whole file is now loaded in the memory buffer. */

	// terminate
	fclose(pFile);
	free(buffer);

	return result;
}
