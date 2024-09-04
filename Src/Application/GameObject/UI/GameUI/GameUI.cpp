#include "GameUI.h"

void GameUI::Update()
{
}

void GameUI::DrawSprite()
{
	if (m_life.spTex)
	{
		//KdShaderManager::Instance().m_spriteShader.DrawTex(m_life.spTex, m_life.pos.x, m_life.pos.y);
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