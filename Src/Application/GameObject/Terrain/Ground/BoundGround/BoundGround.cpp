#include "BoundGround.h"
#include "../../../Character/BaseCharacter.h"

void BoundGround::Init()
{
	// モデルセット
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/Ground/BoundGround/boundGround.gltf");
	}

	// 座標セット
	SetPos({ 20.0f, 0.0f, 0.0f });

	// DrawLit
	m_drawType = eDrawTypeLit;

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("BoundGround", m_spModel, KdCollider::TypeGround);

	// オブジェクトタイプ
	m_objectType = ObjectType::BoundGround;
}

void BoundGround::OnHit()
{
	
}