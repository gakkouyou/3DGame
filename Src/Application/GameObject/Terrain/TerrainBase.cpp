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
	if (m_drawType & eDrawTypeLit)
	{
		if (m_spModel)
		{
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
