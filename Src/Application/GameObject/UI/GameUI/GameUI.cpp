#include "GameUI.h"

void GameUI::Update()
{
	// ポーズ中は処理しない
	if (m_pauseFlg == true) return;
	if (m_stopFlg == true) return;

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
	// ポーズ中は処理しない
	if (m_pauseFlg == true) return;
	if (m_stopFlg == true) return;

	if (m_spPauseTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spPauseTex, (int)m_pausePos.x, (int)m_pausePos.y);
	}

	if (m_oldDrawType == DrawType::Max) return;


	Texture tex = m_tex[m_oldDrawType];
	Math::Color color = { 1, 1, 1, tex.alpha };
	if (tex.spTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(tex.spTex, (int)tex.pos.x, (int)tex.pos.y, nullptr, &color);
	}
}

void GameUI::Init()
{
	DataLoad();

	m_tex[DrawType::Warp].spTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/Game/UI/warp.png");
	m_tex[DrawType::Warp].pos = { 0, -300 };
	m_tex[DrawType::Warp].addAlpha = 0.05f;

	m_tex[DrawType::HoldBox].spTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/Game/UI/holdBox.png");
	m_tex[DrawType::HoldBox].pos = { 0, -300 };
	m_tex[DrawType::HoldBox].addAlpha = 0.05f;

	m_spPauseTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/Game/UI/pause.png");
}

void GameUI::SetDrawType(DrawType _drawType)
{
	m_drawType = _drawType;
}

void GameUI::Reset()
{
	m_pauseFlg = false;

	for (int i = 0; i < DrawType::Max; i++)
	{
		m_tex[i].alpha = 0;
	}

	m_drawType = DrawType::Max;
	m_oldDrawType = DrawType::Max;
}

void GameUI::DataLoad()
{
	// JSONファイルを読み込む
	std::ifstream file(m_path.data());
	if (!file.is_open()) return;

	nlohmann::json data;
	file >> data;

	// JSONデータを格納していく
	m_tex[DrawType::Warp].pos.x			= data["GameUI"]["m_tex[DrawType::Warp].pos.x"];
	m_tex[DrawType::Warp].pos.y			= data["GameUI"]["m_tex[DrawType::Warp].pos.y"];
	m_tex[DrawType::Warp].addAlpha		= data["GameUI"]["m_tex[DrawType::Warp].addAlpha"];

	m_tex[DrawType::HoldBox].pos.x		= data["GameUI"]["m_tex[DrawType::HoldBox].pos.x"];
	m_tex[DrawType::HoldBox].pos.y		= data["GameUI"]["m_tex[DrawType::HoldBox].pos.y"];
	m_tex[DrawType::HoldBox].addAlpha	= data["GameUI"]["m_tex[DrawType::HoldBox].addAlpha"];
}
