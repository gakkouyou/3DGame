#include "NormalGround.h"

void NormalGround::DrawLit()
{
	if (m_spModel)
	{
		// 少し白くする
		if (m_setParamFlg == true)
		{
			KdShaderManager::Instance().m_StandardShader.SetColorEnable(true);
			m_setParamFlg = false;
		}

		KdShaderManager::Instance().m_StandardShader.SetColorEnable(true, { 0.2f, 0.2f, 0.2f });

		// アルファディザ
		if (m_ditherFlg == true)
		{
			KdShaderManager::Instance().m_StandardShader.SetDitherEnable(true);
		}
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
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
	m_drawType |= eDrawTypeDepthOfShadow;

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Ground", m_spModel, KdCollider::TypeGround);

	// オブジェクトタイプ
	m_objectType = ObjectType::NormalGround;

	TerrainBase::Init();
}

void NormalGround::SetParam(const Param& _param)
{
	m_param.pos			= _param.startPos;
	m_param.startPos	= _param.startPos;
	m_param.scale		= _param.scale;

	Math::Matrix transMat	= Math::Matrix::CreateTranslation(m_param.pos);
	Math::Matrix scaleMat	= Math::Matrix::CreateScale(m_param.scale);

	m_mWorld = scaleMat * transMat;

	m_setParamFlg = true;
}
