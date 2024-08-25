#include "Pause.h"
#include "../../Scene/SceneManager.h"

void Pause::Update()
{
	// デバッグモードの時は更新しない
	if (SceneManager::Instance().GetDebug()) return;

	if (m_stopFlg)return;

	// ポーズ画面に移行する
	if (GetAsyncKeyState('P') & 0x8000)
	{
		if ((m_keyFlg & KeyFlg::pause) == 0)
		{
			m_pauseFlg = !m_pauseFlg;
			m_keyFlg |= KeyFlg::pause;

			// ポーズ画面の開始は必ずつづけるから始まる
			m_nowSelect = Select::Return;
			m_select	= Select::None;
		}
	}
	else
	{
		m_keyFlg &= (~KeyFlg::pause);
	}

	// ポーズ中の時
	if (m_pauseFlg == true)
	{
		// 上キー
		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			if ((m_keyFlg & KeyFlg::up) == 0)
			{
				m_nowSelect--;
				m_keyFlg |= KeyFlg::up;
			}
		}
		else
		{
			m_keyFlg &= (~KeyFlg::up);
		}

		// 下キー
		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			if ((m_keyFlg & KeyFlg::down) == 0)
			{
				m_nowSelect++;
				m_keyFlg |= KeyFlg::down;
			}
		}
		else
		{
			m_keyFlg &= (~KeyFlg::down);
		}

		// 上の限界に行ったら一番下、下の限界に行ったら一番上にする
		if (m_nowSelect > Select::Back)
		{
			m_nowSelect = Select::Return;
		}
		if (m_nowSelect < Select::Return)
		{
			m_nowSelect = Select::Back;
		}

		// 確定させる
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			m_select = m_nowSelect;

			switch (m_select)
			{
			case Return:	// 戻るだけ
				m_pauseFlg = false;
				break;

			case Again:		// ステージをやり直す
				// 動かせなくする
				m_stopFlg = true;
				break;

			case Back:		// ステージから出る
				// 動かせなくする
				m_stopFlg = true;
				break;
			}
		}
	}
}

void Pause::DrawSprite()
{
	if (m_pauseFlg)
	{
	/*	if (m_whiteFrame.spTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_whiteFrame.spTex, m_whiteFrame.pos.x, m_whiteFrame.pos.y, 500, 500);
		}*/

		if (m_grayFrame.spTex)
		{
			Math::Color color = { 1, 1, 1, 1 };
			if (m_nowSelect == Select::Return)
			{
				color = { 1, 1, 0, 1 };
			}
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_grayFrame.spTex, m_return.pos.x, m_return.pos.y, nullptr, &color);
			color = { 1, 1, 1, 1 };
			if (m_nowSelect == Select::Again)
			{
				color = { 1, 1, 0, 1 };
			}
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_grayFrame.spTex, m_again.pos.x, m_again.pos.y, nullptr, &color);
			color = { 1, 1, 1, 1 };
			if (m_nowSelect == Select::Back)
			{
				color = { 1, 1, 0, 1 };
			}
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_grayFrame.spTex, m_back.pos.x, m_back.pos.y, nullptr, &color);
		}

		if (m_return.spTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_return.spTex, m_return.pos.x, m_return.pos.y);
		}

		if (m_again.spTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_again.spTex, m_again.pos.x, m_again.pos.y);
		}

		if (m_back.spTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_back.spTex, m_back.pos.x, m_back.pos.y);
		}
	}
}

void Pause::Init()
{
	// つづける
	if (!m_return.spTex)
	{
		m_return.spTex = std::make_shared<KdTexture>();
		m_return.spTex->Load("Asset/Textures/Pause/return.png");
	}
	m_return.pos.y = 100.0f;

	// やりなおす
	if (!m_again.spTex)
	{
		m_again.spTex = std::make_shared<KdTexture>();
		m_again.spTex->Load("Asset/Textures/Pause/agein.png");
	}

	// ステージから出る
	if (!m_back.spTex)
	{
		m_back.spTex = std::make_shared<KdTexture>();
		m_back.spTex->Load("Asset/Textures/Pause/back.png");
	}
	m_back.pos.y = -100.0f;

	// 後ろの白い枠
	if (!m_whiteFrame.spTex)
	{
		m_whiteFrame.spTex = std::make_shared<KdTexture>();
		m_whiteFrame.spTex->Load("Asset/Textures/Pause/whiteFrame.png");
	}

	// 文字の後ろの灰色の枠
	if (!m_grayFrame.spTex)
	{
		m_grayFrame.spTex = std::make_shared<KdTexture>();
		m_grayFrame.spTex->Load("Asset/Textures/Pause/grayFrame.png");
	}

	m_stopFlg = true;
}

void Pause::Reset()
{
	m_pauseFlg = false;	// ポーズフラグ
	m_select = None;	// 選んだもの
	m_nowSelect = None;	// 選んでいるもの
}
