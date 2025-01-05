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
			m_keyFlg |= KeyFlg::pause | KeyFlg::space;

			// ポーズ画面の開始は必ずつづけるから始まる
			m_nowSelect = Select::Return;
			m_select	= Select::None;

			// 音を鳴らす
			if (m_pauseSE.expired() == false)
			{
				m_pauseSE.lock()->Play();
			}
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
		if ((GetAsyncKeyState(VK_SPACE) & 0x8000) || (GetAsyncKeyState('Z') & 0x8000))
		{
			// キー制御
			if (m_keyFlg & KeyFlg::space) return;

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

			if (!m_selectSE.expired())
			{
				m_selectSE.lock()->Play();
			}
		}
		else
		{
			m_keyFlg &= (~KeyFlg::space);
		}
	}
}

void Pause::DrawSprite()
{
	if (m_pauseFlg)
	{
		if (m_grayFrame.spTex)
		{
			Math::Color color = { 1, 1, 1, 1 };
			// 選ばれていたら黄色にする
			if (m_nowSelect == Select::Return)
			{
				color = { 1, 1, 0, 1 };
			}
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_grayFrame.spTex, (int)m_return.pos.x, (int)m_return.pos.y, nullptr, &color);
			color = { 1, 1, 1, 1 };
			// 選ばれていたら黄色にする
			if (m_nowSelect == Select::Again)
			{
				color = { 1, 1, 0, 1 };
			}
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_grayFrame.spTex, (int)m_again.pos.x, (int)m_again.pos.y, nullptr, &color);
			color = { 1, 1, 1, 1 };
			// 選ばれていたら黄色にする
			if (m_nowSelect == Select::Back)
			{
				color = { 1, 1, 0, 1 };
			}
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_grayFrame.spTex, (int)m_back.pos.x, (int)m_back.pos.y, nullptr, &color);
		}

		if (m_return.spTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_return.spTex, (int)m_return.pos.x, (int)m_return.pos.y);
		}

		if (m_again.spTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_again.spTex, (int)m_again.pos.x, (int)m_again.pos.y);
		}

		if (m_back.spTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_back.spTex, (int)m_back.pos.x, (int)m_back.pos.y);
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
		m_again.spTex->Load("Asset/Textures/Pause/again.png");
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

	// 選択したときの音
	m_selectSE = KdAudioManager::Instance().Play("Asset/Sounds/SE/select.wav");
	if (!m_selectSE.expired())
	{
		m_selectSE.lock()->SetVolume(0.2f);
		m_selectSE.lock()->Stop();
	}

	// ポーズ画面を開いた時の音
	m_pauseSE = KdAudioManager::Instance().Play("Asset/Sounds/SE/pause.wav");
	if (!m_pauseSE.expired())
	{
		m_pauseSE.lock()->SetVolume(0.1f);
		m_pauseSE.lock()->Stop();
	}
}

void Pause::Reset()
{
	m_pauseFlg = false;	// ポーズフラグ
	m_select = None;	// 選んだもの
	m_nowSelect = None;	// 選んでいるもの
}
