#include "TitleUI.h"

void TitleUI::Update()
{
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		m_deleteFlg = true;
	}

	if (m_deleteFlg)
	{
		m_alpha -= 0.02f;
		if (m_alpha < 0)
		{
			m_alpha = 0;
		}
	}
}

void TitleUI::DrawSprite()
{
	Math::Color color = { 1, 1, 1, m_alpha };

	if (m_title.spTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_title.spTex, m_title.pos.x, m_title.pos.y, nullptr, &color);
	}
	if (m_pushSpace.spTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_pushSpace.spTex, m_pushSpace.pos.x, m_pushSpace.pos.y, nullptr, &color);
	}
}

void TitleUI::Init()
{
	if (!m_title.spTex)
	{
		m_title.spTex = std::make_shared<KdTexture>();
		m_title.spTex->Load("Asset/Textures/Title/title.png");
	}
	m_title.pos = { 0, 100 };

	if (!m_pushSpace.spTex)
	{
		m_pushSpace.spTex = std::make_shared<KdTexture>();
		m_pushSpace.spTex->Load("Asset/Textures/Title/pushSpace.png");
	}
	m_pushSpace.pos = { 0, -200 };
}
