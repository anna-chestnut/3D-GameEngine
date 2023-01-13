// Includes
//=========

#include "cMyGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Gameplay/GameObject.h>
#include <Engine/Gameplay/Camera.h>
#include <Engine/Math/Functions.h>



// Static Data
//============

namespace
{
	// Geometry Data
	//--------------

	eae6320::Graphics::Mesh* s_mesh;

	uint16_t* s_meshIndexData;
	eae6320::Graphics::VertexFormats::sVertex_mesh* s_meshVertexData;

	eae6320::Graphics::Mesh* s_mesh2;
	uint16_t* s_mesh2IndexData;
	eae6320::Graphics::VertexFormats::sVertex_mesh* s_mesh2VertexData;

	eae6320::Graphics::Mesh* s_mesh3;
	uint16_t* s_mesh3IndexData;
	eae6320::Graphics::VertexFormats::sVertex_mesh* s_mesh3VertexData;

	bool isMeshVisible = true;
	// Shading Data
	//-------------
	eae6320::Graphics::Effect* s_effect;
	eae6320::Graphics::Effect* s_effect2;
	bool useEffect2 = true;

	eae6320::Gameplay::GameObject s_gameObject;
	eae6320::Gameplay::GameObject s_gameObject2;
	eae6320::Gameplay::GameObject s_gameObject3;

	eae6320::Gameplay::Camera s_camera;
	eae6320::Gameplay::Camera* s_useCamera;

}



// Inherited Implementation
//=========================

void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate) {

	eae6320::Graphics::SubmitElapsedTime(i_elapsedSecondCount_systemTime, i_elapsedSecondCount_sinceLastSimulationUpdate);
	float color[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	eae6320::Graphics::SubmitBackgroundColor(color);


	eae6320::Graphics::SubmitGameObject(s_gameObject);
	eae6320::Graphics::SubmitGameObject(s_gameObject2);
	eae6320::Graphics::SubmitGameObject(s_gameObject3);

	eae6320::Graphics::SubmitCamera(Math::cMatrix_transformation::CreateWorldToCameraTransform(s_useCamera->m_rigidBodyState.orientation, s_useCamera->m_rigidBodyState.position),
		Math::cMatrix_transformation::CreateCameraToProjectedTransform_perspective(eae6320::Math::ConvertDegreesToRadians(s_useCamera->m_degree), 1.0f, s_useCamera->m_nearPlane, s_useCamera->m_farPlane));

}

// Run
//----

void eae6320::cMyGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Escape) )
	{
		// Exit the application
		const auto result = Exit( EXIT_SUCCESS );
		EAE6320_ASSERT( result );
	}
}

void eae6320::cMyGame::UpdateBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate) 
{
	// Update gameobject position and matrix
	s_gameObject.m_rigidBodyState.Update(i_elapsedSecondCount_sinceLastUpdate);
	s_gameObject.m_constantData_drawcall.g_transform_localToWorld = s_gameObject.m_rigidBodyState.PredictFutureTransform(i_elapsedSecondCount_sinceLastUpdate);
	s_gameObject2.m_rigidBodyState.Update(i_elapsedSecondCount_sinceLastUpdate);
	s_gameObject2.m_constantData_drawcall.g_transform_localToWorld = s_gameObject2.m_rigidBodyState.PredictFutureTransform(i_elapsedSecondCount_sinceLastUpdate);
	
	// Update camera matrices
	s_useCamera->m_rigidBodyState.Update(i_elapsedSecondCount_sinceLastUpdate);
	//s_useCamera->m_constantData_frame.g_transform_worldToCamera = Math::cMatrix_transformation::CreateWorldToCameraTransform(s_useCamera->m_rigidBodyState.orientation, s_useCamera->m_rigidBodyState.position);
	//s_useCamera->m_constantData_frame.g_transform_cameraToProjected = Math::cMatrix_transformation::CreateCameraToProjectedTransform_perspective(eae6320::Math::ConvertDegreesToRadians(s_useCamera->m_degree), 1.0f, s_useCamera->m_nearPlane, s_useCamera->m_farPlane);
}

void eae6320::cMyGame::UpdateSimulationBasedOnInput()
{

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Space))
	{ 
		s_gameObject.m_visible = false;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::F1))
	{
		s_gameObject2.m_effect = s_effect;
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::F2))
	{
		s_gameObject2.m_mesh = s_mesh;
	}

	// Gameobject movement
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
	{
		s_gameObject2.m_rigidBodyState.velocity = Math::sVector(0.2f, 0.0f, 0.0f);
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Left))
	{
		s_gameObject2.m_rigidBodyState.velocity = Math::sVector(-0.2f, 0.0f, 0.0f);
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
	{
		s_gameObject2.m_rigidBodyState.velocity = Math::sVector(0.0f, 0.2f, 0.0f);
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
	{
		s_gameObject2.m_rigidBodyState.velocity = Math::sVector(0.0f, -0.2f, 0.0f);
	}


	// Camera movement
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::A))
	{
		s_useCamera->m_rigidBodyState.velocity = Math::sVector(-0.8f, 0.0f, 0.0f);
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::D))
	{
		s_useCamera->m_rigidBodyState.velocity = Math::sVector(0.8f, 0.0f, 0.0f);
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::W))
	{
		s_useCamera->m_rigidBodyState.velocity = Math::sVector(0.0f, 0.0f, -0.8f);
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::S))
	{
		s_useCamera->m_rigidBodyState.velocity = Math::sVector(0.0f, 0.0f, 0.8f);
	}

	// Need to obtain mouse first
	// --------------------------
	eae6320::UserInput::Mouse mouse = eae6320::UserInput::GetMouse();

	// Mouse Events
	// ------------
	while (!mouse.EventBufferIsEmpty()) {

		// Read the event first 
		eae6320::UserInput::MouseEvent me = mouse.ReadEvent();

		// check which event you get

		// wheel up and down
		/*if (me.GetType() == eae6320::UserInput::MouseEvent::EventType::WheelDown) {
			s_gameObject2.m_rigidBodyState.velocity = Math::sVector(0.0f, -0.2f, 0.0f);
		}
		if (me.GetType() == eae6320::UserInput::MouseEvent::EventType::WheelUp) {
			s_gameObject2.m_rigidBodyState.velocity = Math::sVector(0.0f, 0.2f, 0.0f);
		}

		// left press
		if (me.GetType() == eae6320::UserInput::MouseEvent::EventType::LPress) {
			s_gameObject.m_visible = false;
		}

		// middle release
		if (me.GetType() == eae6320::UserInput::MouseEvent::EventType::MRelease) {
			s_gameObject2.m_effect = s_effect;
		}*/

	}
	

	// Mouse Position ( change camera velocity )
	// -----------------------------------------

	// top left is (0, 0)

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Z)) {

		if (mouse.GetPosX() > mouse.GetPrePosX()) {
			s_useCamera->m_rigidBodyState.velocity = Math::sVector(-0.8f, s_useCamera->m_rigidBodyState.velocity.y, 0.0f);
		}
		else {
			s_useCamera->m_rigidBodyState.velocity = Math::sVector(0.8f, s_useCamera->m_rigidBodyState.velocity.y, 0.0f);
		}

		if (mouse.GetPosY() > mouse.GetPrePosY()) {
			s_useCamera->m_rigidBodyState.velocity = Math::sVector(s_useCamera->m_rigidBodyState.velocity.x, 0.8f, 0.0f);
		}
		else {
			s_useCamera->m_rigidBodyState.velocity = Math::sVector(s_useCamera->m_rigidBodyState.velocity.x, -0.8f, 0.0f);
		}
	}
	else {
		s_useCamera->m_rigidBodyState.velocity = Math::sVector(0.0f, 0.0f, 0.0f);
	}

	// Mouse Click
	// -----------
	if (mouse.IsLeftDown()) {
		eae6320::Logging::OutputMessage("Left Mouse Down");
	}

	if (mouse.IsRightDown()) {
		eae6320::Logging::OutputMessage("Right Mouse Down");
	}
}

void eae6320::cMyGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	//s_gameObject.m_rigidBodyState.Update(i_elapsedSecondCount_sinceLastUpdate);
	//s_gameObject.m_constantData_drawcall.g_transform_localToWorld = s_gameObject.m_rigidBodyState.PredictFutureTransform(i_elapsedSecondCount_sinceLastUpdate);
	//s_gameObject2.m_rigidBodyState.Update(i_elapsedSecondCount_sinceLastUpdate);
	//s_gameObject2.m_constantData_drawcall.g_transform_localToWorld = s_gameObject2.m_rigidBodyState.PredictFutureTransform(i_elapsedSecondCount_sinceLastUpdate);

}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	eae6320::Logging::OutputMessage("Initialize MyGame");

	auto result = eae6320::Results::Success;
	{
		if (!(result = eae6320::Graphics::Mesh::Load("data/Meshes/plane.bin", s_meshVertexData, s_meshIndexData, s_mesh)))
		{
			EAE6320_ASSERTF(false, "Can't initialize mesh data");
		}

		/*if (!(result = eae6320::Graphics::Mesh::Load("data/Meshes/plane.lua", s_meshVertexData, s_meshIndexData, s_mesh)))
		{
			EAE6320_ASSERTF(false, "Can't initialize mesh data");
		}*/

	}
	{
		if (!(result = eae6320::Graphics::Mesh::Load("data/Meshes/cube.bin", s_mesh2VertexData, s_mesh2IndexData, s_mesh2)))
		{
			EAE6320_ASSERTF(false, "Can't initialize mesh data");
		}
	}
	{
		if (!(result = eae6320::Graphics::Mesh::Load("data/Meshes/sphere.bin", s_mesh2VertexData, s_mesh2IndexData, s_mesh3)))
		{
			EAE6320_ASSERTF(false, "Can't initialize mesh data");
		}
	}

	if (!(result = eae6320::Graphics::Effect::Load("data/Shaders/Vertex/standard.shader", "data/Shaders/Fragment/animatedColor.shader", s_effect, 0x6)))
	{
		EAE6320_ASSERTF(false, "Can't initialize effect data");
		return Results::Failure;
	}

	if (!(result = eae6320::Graphics::Effect::Load("data/Shaders/Vertex/standard.shader", "data/Shaders/Fragment/mycolor.shader", s_effect2, 0x6)))
	{
		EAE6320_ASSERTF(false, "Can't initialize effect2 data");
		return Results::Failure;
	}

	

	s_gameObject.m_mesh = s_mesh;
	s_gameObject.m_effect = s_effect;
	s_gameObject.m_visible = true;
	s_gameObject.m_rigidBodyState.position = Math::sVector(0.0f,0.0f,0.0f);

	s_gameObject2.m_mesh = s_mesh2;
	s_gameObject2.m_effect = s_effect2;
	s_gameObject2.m_visible = true;
	s_gameObject.m_rigidBodyState.position = Math::sVector(0.0f, 0.0f, 0.0f);

	s_gameObject3.m_mesh = s_mesh3;
	s_gameObject3.m_effect = s_effect2;
	s_gameObject3.m_visible = true;
	s_gameObject3.m_rigidBodyState.position = Math::sVector(0.0f, 0.0f, 0.0f);

	s_camera.m_rigidBodyState.position = Math::sVector(0.0f, 1.0f, 5.0f);
	s_camera.m_rigidBodyState.orientation = Math::cQuaternion( 0.0f, Math::sVector(0.0f, 0.0f, -1.0f));
	s_camera.m_degree = 60.0f;
	s_camera.m_nearPlane = 0.1f;
	s_camera.m_farPlane = 50.0f;

	s_useCamera = &s_camera;

	return Results::Success;
}

eae6320::cResult eae6320::cMyGame::CleanUp()
{
	eae6320::Logging::OutputMessage("Clean up MyGame");

	s_effect->DecrementReferenceCount();
	s_effect = nullptr;
	s_effect2->DecrementReferenceCount();
	s_effect2 = nullptr;
	s_mesh->DecrementReferenceCount();
	s_mesh = nullptr;
	s_mesh2->DecrementReferenceCount();
	s_mesh2 = nullptr;
	return Results::Success;
}
