#include "SlopeGround.h"

void SlopeGround::Init()
{
	// モデル読み込み
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/Ground/SlopeGround/slopeGround.gltf");
	}

	m_drawType = eDrawTypeLit;
	m_drawType |= eDrawTypeDepthOfShadow;

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("SlopeGround", m_spModel, KdCollider::TypeGround);

	// オブジェクトタイプ
	m_objectType = ObjectType::SlopeGround;

	TerrainBase::Init();
}

void SlopeGround::SetParam(const Param& _param)
{
	m_param.pos = _param.startPos;
	m_param.startPos = _param.startPos;
	m_param.scale = _param.scale;
	m_param.degAng = _param.degAng;

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_param.pos);
	Math::Matrix rotMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_param.degAng.y), DirectX::XMConvertToRadians(m_param.degAng.x), DirectX::XMConvertToRadians(m_param.degAng.z));
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_param.scale);

	m_mWorld = scaleMat * rotMat * transMat;

	m_setParamFlg = true;
}
