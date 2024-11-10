#include "GameUI.h"

void GameUI::Update()
{
	if (m_drawType == DrawType::Max && m_oldDrawType == DrawType::Max) return;

	if (m_drawType == DrawType::Max)
	{
		m_tex[m_oldDrawType].alpha -= m_tex[m_oldDrawType].addAlpha;
		if (m_tex[m_oldDrawType].alpha <= 0)
		{
			m_tex[m_oldDrawType].alpha = 0;
			m_oldDrawType = DrawType::Max;
		}
	}
	else
	{
		m_tex[m_drawType].alpha += m_tex[m_drawType].addAlpha;
		if (m_tex[m_drawType].alpha >= 1.0f)
		{
			m_tex[m_drawType].alpha = 1.0f;
		}
		m_oldDrawType = m_drawType;
	}
	m_drawType = DrawType::Max;
}

void GameUI::DrawSprite()
{
	if (m_drawType == DrawType::Max && m_oldDrawType == DrawType::Max) return;


	Texture tex = m_tex[m_oldDrawType];
	Math::Color color = { 1, 1, 1, tex.alpha };
	if (tex.spTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(tex.spTex, tex.pos.x, tex.pos.y, nullptr, &color);
	}
}

void GameUI::Init()
{
	m_tex[DrawType::Warp].spTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/Game/UI/warp.png");
	m_tex[DrawType::Warp].pos = { 0, -300 };
	m_tex[DrawType::Warp].addAlpha = 0.05f;

	m_tex[DrawType::HoldBox].spTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/Game/UI/holdBox.png");
	m_tex[DrawType::HoldBox].pos = { 0, -300 };
	m_tex[DrawType::HoldBox].addAlpha = 0.05f;
}

void GameUI::SetDrawType(DrawType _drawType)
{
	m_drawType = _drawType;
}
