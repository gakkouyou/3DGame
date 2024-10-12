#include "FencePillar.h"

void FencePillar::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/Object/FencePillar/fencePillar.gltf");
	}

	// 描画タイプ
	m_drawType |= eDrawTypeLit;
	m_drawType |= eDrawTypeDepthOfShadow;

	// 当たり判定
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("FencePillar", m_spModel, KdCollider::TypeGround);

	// オブジェクトのタイプ
	m_objectType = KdGameObject::ObjectType::FencePillar;

	TerrainBase::Init();
}

void FencePillar::SetParam(const Param& _param)
{
	m_param.pos		= _param.startPos;
	m_param.startPos= _param.startPos;
	m_param.degAng	= _param.degAng;
	m_param.scale	= _param.scale;

	Math::Matrix rotMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_param.degAng.y), DirectX::XMConvertToRadians(m_param.degAng.x), DirectX::XMConvertToRadians(m_param.degAng.z));
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_param.startPos);
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_param.scale);

	m_mWorld = scaleMat * rotMat * transMat;

	m_setParamFlg = true;
}