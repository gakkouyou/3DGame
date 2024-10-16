#include "GameUI.h"
#include "../../Character/Player/Player.h"

void GameUI::Update()
{
}

void GameUI::DrawSprite()
{
	if (!m_wpPlayer.expired())
	{
		// ライフの表示
		{
			// 今のライフ数
			int nowLife = m_wpPlayer.lock()->GetLife();
			for (int i = 0; i < m_wpPlayer.lock()->GetLifeMax(); i++)
			{
				// 普通のハートを出す
				if (i < nowLife)
				{
					if (m_life.spTex)
					{
						KdShaderManager::Instance().m_spriteShader.DrawTex(m_life.spTex, int(m_life.pos.x + i * m_lifeTexInterval), (int)m_life.pos.y);
					}
				}
				else
				{
					if (m_emptyLife.spTex)
					{
						KdShaderManager::Instance().m_spriteShader.DrawTex(m_emptyLife.spTex, int(m_life.pos.x + i * m_lifeTexInterval), (int)m_life.pos.y);
					}
				}
			}
		}
	}

	//// 残機の表示
	//{
	//	// 残機のアイコン
	//	if (m_stockIcon.spTex)
	//	{
	//		KdShaderManager::Instance().m_spriteShader.DrawTex(m_stockIcon.spTex, (int)m_stockIcon.pos.x, (int)m_stockIcon.pos.y);
	//	}
	//}
}

void GameUI::Init()
{
	if (!m_life.spTex)
	{
		m_life.spTex = std::make_shared<KdTexture>();
		m_life.spTex->Load("Asset/Textures/Game/UI/life.png");
	}
	m_life.pos = { -600, 300 };

	if (!m_emptyLife.spTex)
	{
		m_emptyLife.spTex = std::make_shared<KdTexture>();
		m_emptyLife.spTex->Load("Asset/Textures/Game/UI/emptyLife.png");
	}

	if (!m_stockIcon.spTex)
	{
		m_stockIcon.spTex = std::make_shared<KdTexture>();
		m_stockIcon.spTex->Load("Asset/Textures/Game/UI/stockIcon.png");
	}
	m_stockIcon.pos = { -600, 200 };
}