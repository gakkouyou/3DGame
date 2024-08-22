#include "StageStart.h"

void StageStart::Update()
{
	if (m_endFlg)
	{
		m_alpha -= m_alphaDiff;
		if (m_alpha < 0)
		{
			m_alpha = 0;
		}
	}
}

void StageStart::DrawSprite()
{
	if (m_spTex)
	{
		Math::Color color = { 1, 1, 1, m_alpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex, 0, 0, nullptr, &color);
	}
}

void StageStart::Init()
{
	if (!m_spTex)
	{
		m_spTex = std::make_shared<KdTexture>();
		m_spTex->Load("Asset/Textures/Text/start.png");
	}
	m_alpha = 1.0f;
}

void StageStart::Reset()
{
	m_alpha = 1.0f;
	m_endFlg = false;
}
