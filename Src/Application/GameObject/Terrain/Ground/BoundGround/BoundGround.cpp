#include "BoundGround.h"

void BoundGround::Update()
{
	m_mWorld = Math::Matrix::CreateTranslation(m_info.pos);
}

void BoundGround::Init()
{
	// モデルセット
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/Ground/BoundGround2/boundGround.gltf");
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

void BoundGround::SetInfo(Math::Vector3 _startPos, Math::Vector3 _goalPos, float _speed, int _stayTime, Math::Vector3 _degAng)
{
	m_info.pos = _startPos;
	m_info.startPos = _startPos;
}
