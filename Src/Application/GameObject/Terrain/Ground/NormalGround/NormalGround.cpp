#include "NormalGround.h"

void NormalGround::Init()
{
	// モデル読み込み
	m_spModel = std::make_shared<KdModelData>();
	m_spModel->Load("Asset/Models/Terrain/Ground/NormalGround/normalGround.gltf");

	m_drawType = eDrawTypeLit;

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Ground", m_spModel, KdCollider::TypeGround);

	// オブジェクトタイプ
	m_objectType = ObjectType::NormalGround;
}