#include "DropGround.h"

void DropGround::Update()
{
	if (!m_onHitFlg)
	{
		m_stayCount = 0;
	}
	m_onHitFlg = false;

	if (m_dropFlg)
	{
		m_param.pos.y -= 0.5f;
	}


	Math::Matrix transMat;
	transMat = Math::Matrix::CreateTranslation(m_param.pos.x, m_param.pos.y, m_param.pos.z);

	m_mWorld = transMat;
}

void DropGround::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/Ground/DropGround/dropGround.gltf");
	}

	// 当たり判定
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("DropGroundCollision", m_spModel, KdCollider::TypeGround);

	// オブジェクトのタイプ
	m_objectType = ObjectType::DropGround;

	TerrainBase::Init();

	// 描画タイプ
	m_drawType = eDrawTypeLit;

	m_param.stayTime = 60;
}

void DropGround::OnHit()
{
	m_onHitFlg = true;
	m_stayCount++;
	if (m_stayCount > m_param.stayTime)
	{
		m_dropFlg = true;
	}
}

void DropGround::SetParam(Math::Vector3 _startPos, Math::Vector3 _goalPos, float _speed, int _stayTime, Math::Vector3 _degAng)
{
	m_param.startPos	= _startPos;
	m_param.pos			= _startPos;
	m_param.stayTime	= _stayTime;
	m_param.speed		= _speed;
}
