eae6320::Graphics::cEffect* effect1;
eae6320::Graphics::cEffect* effect2;

void eae6320::cMyGame::GenerateDemo() {

	eae6320::Graphics::cEffect::Load(effect1, "data/Shaders/Vertex/standard.shader", "data/Shaders/Fragment/standard.shader");
	eae6320::Graphics::cEffect::Load(effect2, "data/Shaders/Vertex/standard.shader", "data/Shaders/Fragment/red.shader");

	Collision::Initialize();
	for (int i = 0; i < 1000; i++) {
		AddObject("data/Shaders/Vertex/standard.shader", "data/Shaders/Fragment/standard.shader", "data/Meshes/cube.hrj");
		float xPos = (float)(3 - (float)(rand() % 600) / 100.0);
		float yPos = (float)(3 - (float)(rand() % 600) / 100.0);
		float zPos = (float)(3 - (float)(rand() % 600) / 100.0);
		gameObjects[i]->m_RigidBodyState->position = Math::sVector(xPos, yPos, zPos);
		float xVel = (float)(1 - (float)(rand() % 600) / 300.0);
		float yVel = (float)(1 - (float)(rand() % 600) / 300.0);
		float zVel = (float)(1 - (float)(rand() % 600) / 300.0);
		gameObjects[i]->m_RigidBodyState->velocity = Math::sVector(xVel, yVel, zVel);

		Collision::AABBCollider* collider = new Collision::AABBCollider(gameObjects[i]->m_RigidBodyState, 0.1f, 0.1f, 0.1f);

		collider->OnColliderEnter = [this, i](const Collision::ColliderBase* collider) {
			gameObjects[i]->SetEffect(effect2);
		};
		collider->OnColliderExit = [this, i](const Collision::ColliderBase* collider) {
			gameObjects[i]->SetEffect(effect1);
		};
		Collision::AddCollider(collider);

	}
}