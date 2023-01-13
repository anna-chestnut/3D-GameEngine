// Includes
//=========

#include "cFinalGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Gameplay/GameObject.h>
#include <Engine/Gameplay/Camera.h>
#include <Engine/Math/Functions.h>
#include "Collision.h"
#include <array>


// Static Data
//============

namespace
{
	// Geometry Data
	//--------------

	eae6320::Graphics::Mesh* s_mesh;

	uint16_t* s_meshIndexData;
	eae6320::Graphics::VertexFormats::sVertex_mesh* s_meshVertexData;

	eae6320::Graphics::Mesh* s_meshTable;
	uint16_t* s_meshTableIndexData;
	eae6320::Graphics::VertexFormats::sVertex_mesh* s_meshTableVertexData;

	eae6320::Graphics::Mesh* s_mesh3;
	uint16_t* s_mesh3IndexData;
	eae6320::Graphics::VertexFormats::sVertex_mesh* s_mesh3VertexData;

	eae6320::Graphics::Mesh* s_meshCustomer;
	uint16_t* s_meshCustomerIndexData;
	eae6320::Graphics::VertexFormats::sVertex_mesh* s_meshCustomerVertexData;

	eae6320::Graphics::Mesh* s_meshCup;
	uint16_t* s_meshCupIndexData;
	eae6320::Graphics::VertexFormats::sVertex_mesh* s_meshCupVertexData;

	bool isMeshVisible = true;
	// Shading Data
	//-------------
	eae6320::Graphics::Effect* s_effect;
	eae6320::Graphics::Effect* s_effect2;
	eae6320::Graphics::Effect* s_effectRed;
	eae6320::Graphics::Effect* s_effectBrown;
	eae6320::Graphics::Effect* s_effectCustomer;
	bool useEffect2 = true;

	eae6320::Gameplay::GameObject s_gameObject;
	eae6320::Gameplay::GameObject s_gameObject2;
	eae6320::Gameplay::GameObject s_playerGameObject;
	eae6320::Gameplay::GameObject s_table1;
	eae6320::Gameplay::GameObject s_table2;
	eae6320::Gameplay::GameObject s_table3;
	eae6320::Gameplay::GameObject s_customer1;
	eae6320::Gameplay::GameObject s_customer2;
	eae6320::Gameplay::GameObject s_customer3;
	eae6320::Gameplay::GameObject s_cup1;
	eae6320::Gameplay::GameObject s_cup2;
	eae6320::Gameplay::GameObject s_cup3;
	eae6320::Gameplay::GameObject s_cup4;
	eae6320::Gameplay::GameObject s_cup5;
	eae6320::Gameplay::GameObject s_cup6;

	eae6320::Gameplay::Camera s_camera;
	eae6320::Gameplay::Camera* s_useCamera;

	//int tableZposition[] = { 0.5, -1.0, -3.0 };
	std::array<int, 3> tableZposition{ 1.0, -2.0, -5.0 };
	int tablePosIndex = 0;
	std::vector<eae6320::Gameplay::GameObject*> gameobjectLists;
	std::queue<eae6320::Gameplay::GameObject*> cubLists;
	std::queue<eae6320::Gameplay::GameObject*> customerLists;

	bool nextClick = true;
	int s_round = 0;
	int s_score = 0;
	float s_velocity = 2.0f;
}



// Inherited Implementation
//=========================

void eae6320::cFinalGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate) {

	eae6320::Graphics::SubmitElapsedTime(i_elapsedSecondCount_systemTime, i_elapsedSecondCount_sinceLastSimulationUpdate);
	float color[4] = { 0.88f, 0.78f, 0.81f, 1.0f };
	eae6320::Graphics::SubmitBackgroundColor(color);

	for (auto go : gameobjectLists) {
		eae6320::Graphics::SubmitGameObject(*go);
	}

	eae6320::Graphics::SubmitCamera(Math::cMatrix_transformation::CreateWorldToCameraTransform(s_useCamera->m_rigidBodyState.orientation, s_useCamera->m_rigidBodyState.position),
		Math::cMatrix_transformation::CreateCameraToProjectedTransform_perspective(eae6320::Math::ConvertDegreesToRadians(s_useCamera->m_degree), 1.0f, s_useCamera->m_nearPlane, s_useCamera->m_farPlane));

}

// Run
//----

void eae6320::cFinalGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Escape) )
	{
		// Exit the application
		const auto result = Exit( EXIT_SUCCESS );
		EAE6320_ASSERT( result );
	}
}

void eae6320::cFinalGame::UpdateBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate) 
{
	// Update gameobject position and matrix
	
	for (auto go : gameobjectLists) {
		go->m_rigidBodyState.Update(i_elapsedSecondCount_sinceLastUpdate);
		go->m_constantData_drawcall.g_transform_localToWorld = go->m_rigidBodyState.PredictFutureTransform(i_elapsedSecondCount_sinceLastUpdate);
		if (go->m_tag == "cup" && go->m_rigidBodyState.position.x < -15.0) {
			go->m_rigidBodyState.position = Math::sVector(-15.0f, go->m_rigidBodyState.position.y, go->m_rigidBodyState.position.z);
			go->m_rigidBodyState.velocity = Math::sVector(0.0f, 0.0f, 0.0f);
			cubLists.push(go);
		}

		if (go->m_tag == "customer" && go->m_rigidBodyState.position.x < -15.0) {
			go->m_rigidBodyState.position = Math::sVector(-12.0f, go->m_rigidBodyState.position.y, go->m_rigidBodyState.position.z);
			go->m_rigidBodyState.velocity = Math::sVector(0.0f, 0.0f, 0.0f);
			customerLists.push(go);
		}
	}

	if (!customerLists.empty()) {
		eae6320::Gameplay::GameObject* customer = customerLists.front();
		customerLists.pop();
		float x = -14.5f + rand()%5;
		customer->m_rigidBodyState.position = Math::sVector(x, customer->m_rigidBodyState.position.y, customer->m_rigidBodyState.position.z);
		customer->m_rigidBodyState.velocity = Math::sVector(+s_velocity, 0.0f, 0.0f);
	}

	// Update camera matrices
	s_useCamera->m_rigidBodyState.Update(i_elapsedSecondCount_sinceLastUpdate);
	
	eae6320::Collision::Update(i_elapsedSecondCount_sinceLastUpdate);

	s_round++;
	if (s_round == 50) {
		s_round = 0;
		nextClick = true;
	}
	
}

void eae6320::cFinalGame::UpdateSimulationBasedOnInput()
{

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
		if (me.GetType() == eae6320::UserInput::MouseEvent::EventType::WheelDown) {

			tablePosIndex--;
			if (tablePosIndex <= 0) tablePosIndex = 0;
			s_playerGameObject.m_rigidBodyState.position = Math::sVector(s_playerGameObject.m_rigidBodyState.position.x, s_playerGameObject.m_rigidBodyState.position.y, tableZposition[tablePosIndex]-0.1f);
		}
		if (me.GetType() == eae6320::UserInput::MouseEvent::EventType::WheelUp) {
			
			tablePosIndex++;
			if (tablePosIndex > tableZposition.size() - 1) tablePosIndex = tableZposition.size() - 1;
			s_playerGameObject.m_rigidBodyState.position = Math::sVector(s_playerGameObject.m_rigidBodyState.position.x, s_playerGameObject.m_rigidBodyState.position.y, tableZposition[tablePosIndex]-0.1f);
		}

		// left release
		if (me.GetType() == eae6320::UserInput::MouseEvent::EventType::LRelease) {
			
		}

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
	if (mouse.IsLeftDown() && nextClick) {
		nextClick = false;
		if (!cubLists.empty()) {
			eae6320::Gameplay::GameObject* cup = cubLists.front();
			cubLists.pop();
			cup->m_rigidBodyState.position = Math::sVector(s_playerGameObject.m_rigidBodyState.position.x-1.0f, cup->m_rigidBodyState.position.y, s_playerGameObject.m_rigidBodyState.position.z);
			cup->m_rigidBodyState.velocity = Math::sVector(-s_velocity, 0.0f, 0.0f);
		}
	}

}

void eae6320::cFinalGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cFinalGame::Initialize()
{
	eae6320::Logging::OutputMessage("Initialize MyGame");

	eae6320::Collision::Initialize();

	auto result = eae6320::Results::Success;
	{
		if (!(result = eae6320::Graphics::Mesh::Load("data/Meshes/plane.bin", s_meshVertexData, s_meshIndexData, s_mesh)))
		{
			EAE6320_ASSERTF(false, "Can't initialize mesh data");
		}
	}
	{
		if (!(result = eae6320::Graphics::Mesh::Load("data/Meshes/table.bin", s_meshTableVertexData, s_meshTableIndexData, s_meshTable)))
		{
			EAE6320_ASSERTF(false, "Can't initialize mesh data");
		}
	}
	{
		if (!(result = eae6320::Graphics::Mesh::Load("data/Meshes/astro.bin", s_mesh3VertexData, s_mesh3IndexData, s_mesh3)))
		{
			EAE6320_ASSERTF(false, "Can't initialize mesh data");
		}
	}
	{
		if (!(result = eae6320::Graphics::Mesh::Load("data/Meshes/customer.bin", s_meshCustomerVertexData, s_meshCustomerIndexData, s_meshCustomer)))
		{
			EAE6320_ASSERTF(false, "Can't initialize mesh data");
		}
	}
	{
		if (!(result = eae6320::Graphics::Mesh::Load("data/Meshes/cup.bin", s_meshCupVertexData, s_meshCupIndexData, s_meshCup)))
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

	if (!(result = eae6320::Graphics::Effect::Load("data/Shaders/Vertex/standard.shader", "data/Shaders/Fragment/redcolor.shader", s_effectRed, 0x6)))
	{
		EAE6320_ASSERTF(false, "Can't initialize effect2 data");
		return Results::Failure;
	}

	if (!(result = eae6320::Graphics::Effect::Load("data/Shaders/Vertex/standard.shader", "data/Shaders/Fragment/brownColor.shader", s_effectBrown, 0x6)))
	{
		EAE6320_ASSERTF(false, "Can't initialize effect2 data");
		return Results::Failure;
	}

	if (!(result = eae6320::Graphics::Effect::Load("data/Shaders/Vertex/standard.shader", "data/Shaders/Fragment/animatedColorCustomer.shader", s_effectCustomer, 0x6)))
	{
		EAE6320_ASSERTF(false, "Can't initialize effect data");
		return Results::Failure;
	}

	s_playerGameObject.m_mesh = s_mesh3;
	s_playerGameObject.m_effect = s_effectRed;
	s_playerGameObject.m_visible = true;
	s_playerGameObject.m_rigidBodyState.position = Math::sVector(3.0f, -1.0f, 0.5f);
	s_playerGameObject.m_rigidBodyState.orientation = Math::cQuaternion(3.0f, Math::sVector(0.0f, 1.0f, 0.0f));
	eae6320::Collision::AABBCollider* colliderPlayer = new Collision::AABBCollider(&s_playerGameObject.m_rigidBodyState, 0.1f, 0.1f, 0.1f);
	eae6320::Collision::AddCollider(colliderPlayer);
	colliderPlayer->OnColliderEnter = [this](const Collision::ColliderBase* collider) {

		s_playerGameObject.m_rigidBodyState.orientation = Math::cQuaternion(1.0f, Math::sVector(1.0f, 0.0f, 0.0f));
		const auto result = Exit(EXIT_SUCCESS);
		EAE6320_ASSERT(result);
	};

	// Customer
	// --------
	float customerOffset = 0.6f;//0.4
	Math::sVector customerCollider = Math::sVector(0.3f, 0.6f, 0.5f);

	s_customer1.m_mesh = s_meshCustomer;
	s_customer1.m_effect = s_effectCustomer;
	s_customer1.m_visible = true;
	s_customer1.m_tag = "customer";
	s_customer1.m_rigidBodyState.position = Math::sVector(-7.0f, -1.2f, tableZposition[2] - customerOffset);//-2.0
	s_customer1.m_rigidBodyState.velocity = Math::sVector(s_velocity, 0.0f, 0.0f);//-2.0
	s_customer1.m_rigidBodyState.orientation = Math::cQuaternion(1.0f, Math::sVector(0.0f, 1.0f, 0.0f));
	eae6320::Collision::AABBCollider* colliderCustomer1 = new Collision::AABBCollider(&s_customer1.m_rigidBodyState, customerCollider.x, customerCollider.y, customerCollider.z);
	eae6320::Collision::AddCollider(colliderCustomer1);
	colliderCustomer1->OnColliderEnter = [this](const Collision::ColliderBase* collider) {

		s_customer1.m_rigidBodyState.velocity = Math::sVector(-s_velocity, 0.0f, 0.0f);
		s_score++;
	};

	s_customer2.m_mesh = s_meshCustomer;
	s_customer2.m_effect = s_effectCustomer;
	s_customer2.m_visible = true;
	s_customer2.m_tag = "customer";
	s_customer2.m_rigidBodyState.position = Math::sVector(-6.0f, -1.2f, tableZposition[1] - customerOffset);//-2.0
	s_customer2.m_rigidBodyState.velocity = Math::sVector(s_velocity, 0.0f, 0.0f);//-2.0
	s_customer2.m_rigidBodyState.orientation = Math::cQuaternion(1.0f, Math::sVector(0.0f, 1.0f, 0.0f));
	eae6320::Collision::AABBCollider* colliderCustomer2 = new Collision::AABBCollider(&s_customer2.m_rigidBodyState, customerCollider.x, customerCollider.y, customerCollider.z);
	eae6320::Collision::AddCollider(colliderCustomer2);
	colliderCustomer2->OnColliderEnter = [this](const Collision::ColliderBase* collider) {

		s_customer2.m_rigidBodyState.velocity = Math::sVector(-s_velocity, 0.0f, 0.0f);
		s_score++;
	};

	s_customer3.m_mesh = s_meshCustomer;
	s_customer3.m_effect = s_effectCustomer;
	s_customer3.m_visible = true;
	s_customer3.m_tag = "customer";
	s_customer3.m_rigidBodyState.position = Math::sVector(-8.0f, -1.2f, tableZposition[0] - customerOffset);//-2.0
	s_customer3.m_rigidBodyState.velocity = Math::sVector(s_velocity, 0.0f, 0.0f);//-2.0
	s_customer3.m_rigidBodyState.orientation = Math::cQuaternion(1.0f, Math::sVector(0.0f, 1.0f, 0.0f));
	eae6320::Collision::AABBCollider* colliderCustomer3 = new Collision::AABBCollider(&s_customer3.m_rigidBodyState, customerCollider.x, customerCollider.y, customerCollider.z);
	eae6320::Collision::AddCollider(colliderCustomer3);
	colliderCustomer3->OnColliderEnter = [this](const Collision::ColliderBase* collider) {

		s_customer3.m_rigidBodyState.velocity = Math::sVector(-s_velocity, 0.0f, 0.0f);
		s_score++;

	};

	s_camera.m_rigidBodyState.position = Math::sVector(0.0f, 2.0f, 3.2f);
	s_camera.m_rigidBodyState.orientation = Math::cQuaternion( 0.0f, Math::sVector(0.0f, 0.0f, -1.0f));
	s_camera.m_degree = 120.0f;
	s_camera.m_nearPlane = 0.1f;
	s_camera.m_farPlane = 50.0f;

	s_useCamera = &s_camera;

	// table
	// -----
	s_table1.m_mesh = s_meshTable;
	s_table1.m_effect = s_effect;
	s_table1.m_visible = true;
	s_table1.m_rigidBodyState.position = Math::sVector(-8.0f, 0.0f, tableZposition[2]);

	s_table2.m_mesh = s_meshTable;
	s_table2.m_effect = s_effect;
	s_table2.m_visible = true;
	s_table2.m_rigidBodyState.position = Math::sVector(-8.0f, 0.0f, tableZposition[1]);

	s_table3.m_mesh = s_meshTable;
	s_table3.m_effect = s_effect;
	s_table3.m_visible = true;
	s_table3.m_rigidBodyState.position = Math::sVector(-8.0f, 0.0f, tableZposition[0]);

	// cup
	// ---
	s_cup1.m_mesh = s_meshCup;
	s_cup1.m_effect = s_effectBrown;
	s_cup1.m_visible = true;
	s_cup1.m_rigidBodyState.position = Math::sVector(-14.0f, 0.5f, 0.0f);
	s_cup1.m_tag = "cup";
	eae6320::Collision::AABBCollider* colliderCup1 = new Collision::AABBCollider(&s_cup1.m_rigidBodyState, 0.3f, 1.4f, 1.0f);//1.4f
	eae6320::Collision::AddCollider(colliderCup1);
	cubLists.push(&s_cup1);

	s_cup2.m_mesh = s_meshCup;
	s_cup2.m_effect = s_effectBrown;
	s_cup2.m_visible = true;
	s_cup2.m_rigidBodyState.position = Math::sVector(-14.0f, 0.5f, 0.0f);
	s_cup2.m_tag = "cup";
	eae6320::Collision::AABBCollider* colliderCup2 = new Collision::AABBCollider(&s_cup2.m_rigidBodyState, 0.3f, 1.4f, 1.0f);//1.4f
	eae6320::Collision::AddCollider(colliderCup2);
	cubLists.push(&s_cup2);

	s_cup3.m_mesh = s_meshCup;
	s_cup3.m_effect = s_effectBrown;
	s_cup3.m_visible = true;
	s_cup3.m_rigidBodyState.position = Math::sVector(-14.0f, 0.5f, 0.0f);
	s_cup3.m_tag = "cup";
	eae6320::Collision::AABBCollider* colliderCup3 = new Collision::AABBCollider(&s_cup3.m_rigidBodyState, 0.3f, 1.4f, 1.0f);//1.4f
	eae6320::Collision::AddCollider(colliderCup3);
	cubLists.push(&s_cup3);

	s_cup4.m_mesh = s_meshCup;
	s_cup4.m_effect = s_effectBrown;
	s_cup4.m_visible = true;
	s_cup4.m_rigidBodyState.position = Math::sVector(-14.0f, 0.5f, 0.0f);
	s_cup4.m_tag = "cup";
	eae6320::Collision::AABBCollider* colliderCup4 = new Collision::AABBCollider(&s_cup4.m_rigidBodyState, 0.3f, 1.4f, 1.0f);//1.4f
	eae6320::Collision::AddCollider(colliderCup4);
	cubLists.push(&s_cup4);

	s_cup5.m_mesh = s_meshCup;
	s_cup5.m_effect = s_effectBrown;
	s_cup5.m_visible = true;
	s_cup5.m_rigidBodyState.position = Math::sVector(-14.0f, 0.5f, 0.0f);
	s_cup5.m_tag = "cup";
	eae6320::Collision::AABBCollider* colliderCup5 = new Collision::AABBCollider(&s_cup5.m_rigidBodyState, 0.3f, 1.4f, 1.0f);//1.4f
	eae6320::Collision::AddCollider(colliderCup5);
	cubLists.push(&s_cup5);

	s_cup6.m_mesh = s_meshCup;
	s_cup6.m_effect = s_effectBrown;
	s_cup6.m_visible = true;
	s_cup6.m_rigidBodyState.position = Math::sVector(-14.0f, 0.5f, 0.0f);
	s_cup6.m_tag = "cup";
	eae6320::Collision::AABBCollider* colliderCup6 = new Collision::AABBCollider(&s_cup6.m_rigidBodyState, 0.3f, 1.4f, 1.0f);//1.4f
	eae6320::Collision::AddCollider(colliderCup6);
	cubLists.push(&s_cup6);

	// Add to gameobjects list
	// -----------------------
	gameobjectLists.push_back(&s_playerGameObject);

	gameobjectLists.push_back(&s_customer1);
	gameobjectLists.push_back(&s_table1);

	gameobjectLists.push_back(&s_customer2);
	gameobjectLists.push_back(&s_table2);

	gameobjectLists.push_back(&s_customer3);
	gameobjectLists.push_back(&s_table3);
	gameobjectLists.push_back(&s_cup1);
	gameobjectLists.push_back(&s_cup2);
	gameobjectLists.push_back(&s_cup3);
	gameobjectLists.push_back(&s_cup4);
	gameobjectLists.push_back(&s_cup5);
	gameobjectLists.push_back(&s_cup6);

	return Results::Success;
}

eae6320::cResult eae6320::cFinalGame::CleanUp()
{

	eae6320::Logging::OutputMessage("Your Score: %i\n ", s_score);
	eae6320::Logging::OutputMessage("Clean up MyGame");

	s_effect->DecrementReferenceCount();
	s_effect = nullptr;
	s_effect2->DecrementReferenceCount();
	s_effect2 = nullptr;
	s_effectRed->DecrementReferenceCount();
	s_effectRed = nullptr;
	s_effectBrown->DecrementReferenceCount();
	s_effectBrown = nullptr;
	s_effectCustomer->DecrementReferenceCount();
	s_effectCustomer = nullptr;

	s_mesh->DecrementReferenceCount();
	s_mesh = nullptr;
	s_meshTable->DecrementReferenceCount();
	s_meshTable = nullptr;
	s_meshCustomer->DecrementReferenceCount();
	s_meshCustomer = nullptr;
	s_meshCup->DecrementReferenceCount();
	s_meshCup = nullptr;

	return Results::Success;
}
