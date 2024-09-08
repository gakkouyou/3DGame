#include "GameUI.h"
#include "../../Character/Player/Player.h"

void GameUI::Update()
{
}

void GameUI::DrawSprite()
{
	if (m_life.spTex)
	{
		if (!m_wpPlayer.expired())
		{
			for (int i = 0; i < m_wpPlayer.lock()->GetLife(); i++)
			{
				KdShaderManager::Instance().m_spriteShader.DrawTex(m_life.spTex, int(m_life.pos.x + i * m_lifeTexInterval), (int)m_life.pos.y);
			}
		}
	}
}

void GameUI::Init()
{
	if (!m_life.spTex)
	{
		m_life.spTex = std::make_shared<KdTexture>();
		m_life.spTex->Load("Asset/Textures/Game/UI/life.png");
	}
	m_life.pos = { -600, 300 };
}