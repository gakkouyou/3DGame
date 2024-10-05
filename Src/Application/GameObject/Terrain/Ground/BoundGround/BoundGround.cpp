#include "BoundGround.h"

//void BoundGround::Update()
//{
//	m_mWorld = Math::Matrix::CreateTranslation(m_param.pos);
//}

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

	TerrainBase::Init();
}

void BoundGround::OnHit()
{
	
}

void BoundGround::SetParam(const Param& _param)
{
	m_param.pos			= _param.startPos;
	m_param.startPos	= _param.startPos;
	m_param.scale		= _param.scale;

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_param.pos);
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_param.scale);

	m_mWorld = scaleMat * transMat;

	m_setParamFlg = true;
}
