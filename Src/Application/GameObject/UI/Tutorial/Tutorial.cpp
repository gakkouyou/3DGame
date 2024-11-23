#include "Tutorial.h"
#include "../../../Scene/SceneManager.h"

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
	DataLoad();

	std::string path = "Asset/Textures/Tutorial/";

	// 移動
	m_tex[Type::MoveKey].spTex = KdAssets::Instance().m_textures.GetData(path + "moveKey.png");
	m_tex[Type::MoveText].spTex = KdAssets::Instance().m_textures.GetData(path + "moveText.png");
	// ジャンプ
	m_tex[Type::JumpKey].spTex = KdAssets::Instance().m_textures.GetData(path + "jumpKey.png");
	m_tex[Type::JumpText].spTex = KdAssets::Instance().m_textures.GetData(path + "jumpText.png");
	// アクション
	m_tex[Type::ActionKey].spTex = KdAssets::Instance().m_textures.GetData(path + "actionKey.png");
	m_tex[Type::ActionText].spTex = KdAssets::Instance().m_textures.GetData(path + "actionText.png");
}

void Tutorial::DataLoad()
{
	// JSONファイルを読み込む
	std::ifstream file(m_path.data());
	if (!file.is_open()) return;

	nlohmann::json data;
	file >> data;

	// JSONデータを格納していく
	m_tex[Type::MoveKey].pos.x		= data["Tutorial"]["m_tex[Type::MoveKey].pos.x"];	// 移動キー座標
	m_tex[Type::MoveKey].pos.y		= data["Tutorial"]["m_tex[Type::MoveKey].pos.y"];
	m_tex[Type::MoveText].pos.x		= data["Tutorial"]["m_tex[Type::MoveText].pos.x"];	// 移動テキスト
	m_tex[Type::MoveText].pos.y		= data["Tutorial"]["m_tex[Type::MoveText].pos.y"];

	m_tex[Type::JumpKey].pos.x		= data["Tutorial"]["m_tex[Type::JumpKey].pos.x"];	// ジャンプキー座標
	m_tex[Type::JumpKey].pos.y		= data["Tutorial"]["m_tex[Type::JumpKey].pos.y"];
	m_tex[Type::JumpText].pos.x		= data["Tutorial"]["m_tex[Type::JumpText].pos.x"];	// ジャンプテキスト
	m_tex[Type::JumpText].pos.y		= data["Tutorial"]["m_tex[Type::JumpText].pos.y"];

	m_tex[Type::ActionKey].pos.x	= data["Tutorial"]["m_tex[Type::ActionKey].pos.x"];	// アクションキー座標
	m_tex[Type::ActionKey].pos.y	= data["Tutorial"]["m_tex[Type::ActionKey].pos.y"];
	m_tex[Type::ActionText].pos.x	= data["Tutorial"]["m_tex[Type::ActionText].pos.x"];// アクションテキスト
	m_tex[Type::ActionText].pos.y	= data["Tutorial"]["m_tex[Type::ActionText].pos.y"];

	// アルファ値加算量
	m_addAlpha = data["Tutorial"]["m_addAlpha"];
	
	// 止まってから表示するまでの時間
	m_displayTime = data["Tutorial"]["m_displayTime"];
}
