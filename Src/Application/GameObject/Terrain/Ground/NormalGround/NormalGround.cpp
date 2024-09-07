#include "NormalGround.h"

//void NormalGround::Update()
//{
//	m_mWorld = Math::Matrix::CreateTranslation(m_param.pos);
//}

void NormalGround::Init()
{
	// モデル読み込み
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/Ground/NormalGround/normalGround.gltf");
	}

	m_drawType = eDrawTypeLit;
	m_drawType |= eDrawTypeDepthOfShadow;

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Ground", m_spModel, KdCollider::TypeGround);

	// オブジェクトタイプ
	m_objectType = ObjectType::NormalGround;

	TerrainBase::Init();
}

void NormalGround::SetParam(Param _param)
{
	m_param.pos			= _param.startPos;
	m_param.startPos	= _param.startPos;
	m_param.scale		= _param.scale;

	Math::Matrix transMat	= Math::Matrix::CreateTranslation(m_param.pos);
	Math::Matrix scaleMat	= Math::Matrix::CreateScale(m_param.scale);

	m_mWorld = scaleMat * transMat;
}
