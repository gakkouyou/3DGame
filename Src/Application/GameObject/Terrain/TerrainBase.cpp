#include "TerrainBase.h"

void TerrainBase::GenerateDepthMapFromLight()
{
	if (m_drawType & eDrawTypeDepthOfShadow)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void TerrainBase::DrawLit()
{
	if (m_drawType & eDrawTypeLit)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void TerrainBase::DrawBright()
{
	if (m_drawType & eDrawTypeBright)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}