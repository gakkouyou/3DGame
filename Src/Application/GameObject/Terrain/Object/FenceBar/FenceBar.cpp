#include "FenceBar.h"

void FenceBar::Init()
{
	m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Terrain/Object/FenceBar/fenceBar.gltf");

	// 描画タイプ
	m_drawType |= eDrawTypeLit;
	m_drawType |= eDrawTypeDepthOfShadow;

	// 当たり判定
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("FenceBar", m_spModel, KdCollider::TypeGround | KdCollider::TypeDebug);

	// オブジェクトのタイプ
	m_objectType = KdGameObject::ObjectType::FenceBar;

	TerrainBase::Init();
}

void FenceBar::SetParam(const Param& _param)
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