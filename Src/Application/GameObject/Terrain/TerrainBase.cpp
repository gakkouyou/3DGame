#include "TerrainBase.h"

void TerrainBase::GenerateDepthMapFromLight()
{
	if (m_drawType & eDrawTypeDepthOfShadow)
	{
		if (m_spModel)
		{
			KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
		}
	}
}

void TerrainBase::DrawLit()
{
	if (m_drawFlg == false) return;

	if (m_drawType & eDrawTypeLit)
	{
		if (m_spModel)
		{
			// 少し白くする
			if (m_setParamFlg == true)
			{
				KdShaderManager::Instance().m_StandardShader.SetColorEnable(true);
				m_setParamFlg = false;
			}

			// アルファディザ
			if (m_ditherFlg == true)
			{
				KdShaderManager::Instance().m_StandardShader.SetDitherEnable(true);
			}
			KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
		}
	}
}

void TerrainBase::DrawBright()
{
	if (m_drawType & eDrawTypeBright)
	{
		if (m_spModel)
		{
			KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
		}
	}
}

void TerrainBase::Init()
{
	// 大まかなオブジェクトのタイプ
	m_baseObjectType = BaseObjectType::Ground;
}

bool TerrainBase::CheckInScreen(const DirectX::BoundingFrustum& frustum)
{
	return Intersects(frustum);
}

void TerrainBase::SetTarget(const std::weak_ptr<TerrainBase>& _wpTarget)
{
	m_wpTarget = _wpTarget;
	if (m_wpTarget.expired() == false)
	{
		m_param.targetName = m_wpTarget.lock()->GetObjectName();
	}
}
