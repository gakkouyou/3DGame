#include "Propeller.h"

void Propeller::Update()
{

}

void Propeller::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/Object/Propeller/propeller.gltf");
	}

	m_drawType = eDrawTypeLit;

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Propeller", m_spModel, KdCollider::TypeGround);

	// オブジェクトタイプ
	m_objectType = ObjectType::Propeller;

	TerrainBase::Init();

	SetPos({ 7, 2, 0 });
}
