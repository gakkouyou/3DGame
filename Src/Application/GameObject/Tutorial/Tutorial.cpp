#include "Tutorial.h"
#include "../../Scene/SceneManager.h"

void Tutorial::Update()
{
	// ポーズ中は終了
	if (m_pauseFlg == true) return;

	// 表示する状態の場合
	if (m_displayFlg)
	{
		// もしステージセレクトシーンならすぐに表示する
		if (SceneManager::Instance().GetNowScene() == SceneManager::SceneType::StageSelect)
		{
			// アルファ値を加算
			m_alpha += m_addAlpha;
		}
		// 違うならカウントを進めて、時間を超えたら表示する
		else
		{
			m_displayCount++;

			if (m_displayCount > m_displayTime)
			{
				m_alpha += m_addAlpha;
				m_displayCount = m_displayTime;
			}
			// カウントが時間を過ぎてないなら消していく
			else
			{
				// アルファ値を減算
				m_alpha -= m_addAlpha;

				// アルファ値を下限以上にとどめる
				if (m_alpha < m_minAlpha)
				{
					m_alpha = m_minAlpha;
				}
			}
		}

		// アルファ値を上限に留める
		if (m_alpha > m_maxAlpha)
		{
			m_alpha = m_maxAlpha;
		}
	}
	// 表示しない状態の場合
	else
	{
		// アルファ値を減算
		m_alpha -= m_addAlpha;
		
		// アルファ値を下限以上にとどめる
		if (m_alpha < m_minAlpha)
		{
			m_alpha = m_minAlpha;
		}

		// カウントをリセット
		m_displayCount = 0;
	}

	// 表示フラグリセット
	m_displayFlg = false;
}

void Tutorial::DrawSprite()
{
	// ポーズ中は終了
	if (m_pauseFlg == true) return;
	Math::Color color = { 1, 1, 1, m_alpha };
	for (int i = 0; i < Type::Max; i++)
	{
		if (!m_tex[i].spTex) continue;
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_tex[i].spTex, (int)m_tex[i].pos.x, (int)m_tex[i].pos.y, nullptr, &color);
	}
}

void Tutorial::Init()
{
	std::string path = "Asset/Textures/Tutorial/";

	m_tex[Type::MoveKey].spTex = KdAssets::Instance().m_textures.GetData(path + "moveKey.png");
	m_tex[Type::MoveKey].pos = { 450, 300 };
	m_tex[Type::MoveText].spTex = KdAssets::Instance().m_textures.GetData(path + "moveText.png");
	m_tex[Type::MoveText].pos = { 550, 300 };

	m_tex[Type::JumpKey].spTex = KdAssets::Instance().m_textures.GetData(path + "jumpKey.png");
	m_tex[Type::JumpKey].pos = { 450, 250 };
	m_tex[Type::JumpText].spTex = KdAssets::Instance().m_textures.GetData(path + "jumpText.png");
	m_tex[Type::JumpText].pos = { 550, 250 };

	m_tex[Type::ActionKey].spTex = KdAssets::Instance().m_textures.GetData(path + "actionKey.png");
	m_tex[Type::ActionKey].pos = { 450, 200 };
	m_tex[Type::ActionText].spTex = KdAssets::Instance().m_textures.GetData(path + "actionText.png");
	m_tex[Type::ActionText].pos = { 550, 200 };
}
