#include "NormalGround.h"

void NormalGround::Update()
{
	m_mWorld = Math::Matrix::CreateTranslation(m_info.pos);
}

void NormalGround::Init()
{
	// モデル読み込み
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/Ground/NormalGround/normalGround.gltf");
	}

	m_drawType = eDrawTypeLit;

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Ground", m_spModel, KdCollider::TypeGround);

	// オブジェクトタイプ
	m_objectType = ObjectType::NormalGround;
}

void NormalGround::SetInfo(Math::Vector3 _startPos, Math::Vector3 _goalPos, float _speed, int _stayTime, Math::Vector3 _degAng)
{
	m_info.pos = _startPos;
	m_info.startPos = _startPos;
}
