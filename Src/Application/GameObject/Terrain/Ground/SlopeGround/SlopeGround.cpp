#include "SlopeGround.h"

void SlopeGround::Update()
{
}

void SlopeGround::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void SlopeGround::DrawLit()
{
	if (m_spModel)
	{
		// 少し白くする
		if (m_setParamFlg == true)
		{
			KdShaderManager::Instance().m_StandardShader.SetColorEnable(true);
			m_setParamFlg = false;
		}

		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

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
	m_param.targetName = _param.targetName;
	m_param.yetActive = _param.yetActive;

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_param.pos);
	Math::Matrix rotMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_param.degAng.y), DirectX::XMConvertToRadians(m_param.degAng.x), DirectX::XMConvertToRadians(m_param.degAng.z));
	
	Math::Matrix scaleMat;
	if (m_param.yetActive == true)
	{
		scaleMat = Math::Matrix::CreateScale(m_param.scale);
	}
	else
	{
		scaleMat = Math::Matrix::CreateScale(0);
	}

	m_mWorld = scaleMat * rotMat * transMat;
	m_nonScaleMat = rotMat * transMat;

	m_setParamFlg = true;
}

void SlopeGround::Active()
{
	m_activeFlg = true;
	m_scale += m_addScale;
	if (m_scale.y > m_param.scale.y)
	{
		m_scale = m_param.scale;
		m_stayCount++;
		if (m_stayCount > m_stayTime)
		{
			m_activeFlg = false;
		}
	}

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_param.pos);
	Math::Matrix rotMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_param.degAng.y), DirectX::XMConvertToRadians(m_param.degAng.x), DirectX::XMConvertToRadians(m_param.degAng.z));
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_scale);

	m_mWorld = scaleMat * rotMat * transMat;
}