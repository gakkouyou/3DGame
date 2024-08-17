#include "BoundGround.h"

void BoundGround::Update()
{
	m_mWorld = Math::Matrix::CreateTranslation(m_param.pos);
}

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

	// 大まかなオブジェクトのタイプ
	m_baseObjectType = BaseObjectType::Ground;
}

void BoundGround::OnHit()
{
	
}

void BoundGround::SetParam(Math::Vector3 _startPos, Math::Vector3 _goalPos, float _speed, int _stayTime, Math::Vector3 _degAng)
{
	m_param.pos = _startPos;
	m_param.startPos = _startPos;
}
