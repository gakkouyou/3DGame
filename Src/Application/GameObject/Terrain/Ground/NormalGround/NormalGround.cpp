#include "NormalGround.h"

void NormalGround::Update()
{
	m_mWorld = Math::Matrix::CreateTranslation(m_param.pos);
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

	TerrainBase::Init();
}

void NormalGround::SetParam(Math::Vector3 _startPos, Math::Vector3 _goalPos, float _speed, int _stayTime, Math::Vector3 _degAng)
{
	m_param.pos = _startPos;
	m_param.startPos = _startPos;
}
