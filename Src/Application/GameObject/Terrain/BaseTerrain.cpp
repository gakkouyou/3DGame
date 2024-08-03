#include "BaseTerrain.h"

void BaseTerrain::GenerateDepthMapFromLight()
{
	if (m_drawType & eDrawTypeDepthOfShadow)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void BaseTerrain::DrawLit()
{
	if (m_drawType & eDrawTypeLit)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void BaseTerrain::DrawBright()
{
	if (m_drawType & eDrawTypeBright)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}