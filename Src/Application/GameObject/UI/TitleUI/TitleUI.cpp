#include "TitleUI.h"

void TitleUI::Update()
{
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		m_deleteFlg = true;
	}

	// 徐々に消していく
	if (m_deleteFlg)
	{
		m_alpha -= m_addAlpha;
		if (m_alpha < 0)
		{
			m_alpha = 0;
		}
		return;
	}
	
	// 徐々に出していく
	if(m_startFlg == false)
	{
		m_alpha += m_addAlpha;
		m_pushSpaceAlpha += m_addAlpha;
		if (m_alpha > 1.0f)
		{
			m_alpha = 1.0f;
			m_startFlg = true;
		}
		return;
	}

	// "PUSH SPACE"のアルファ値を0.3～1.0に
	m_degAng += m_addDegAng;
	if (m_degAng >= 360)
	{
		m_degAng -= 360;
	}
	m_pushSpaceAlpha = (sin(DirectX::XMConvertToRadians(m_degAng)) + 1.0f) * 0.35f + 0.3f;
}

void TitleUI::DrawSprite()
{
	Math::Color color = { 1, 1, 1, m_alpha };

	if (m_title.spTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_title.spTex, m_title.pos.x, m_title.pos.y, nullptr, &color);
	}

	color = { 1, 1, 1, m_pushSpaceAlpha };
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
