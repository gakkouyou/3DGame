#include "SceneChange.h"
#include "Application/main.h"

void SceneChange::Update()
{
	// シーン開始
	if (m_startFlg)
	{
		m_stayCnt++;
		if (m_stayCnt > m_stayTime)
		{
			if (m_blackFlg == false)
			{
				// 円を大きくする
				m_size += m_addSize;

				// サイズが最大値を上回ったら終了
				if (m_size > m_maxSize)
				{
					m_finishFlg = true;
					m_startFlg = false;
					m_stayCnt = 0;
				}
			}
			// 黒フェードインの場合
			else
			{
				// 黒を薄くする
				m_alpha -= m_distAlpha;

				// アルファ値が０以下になったら終了
				if (m_alpha <= 0)
				{
					m_finishFlg = true;
					m_startFlg = false;
					m_stayCnt = 0;
					m_alpha = 0;
				}
			}
		}
	}

	// シーン終了
	if (m_endFlg)
	{
		m_stayCnt++;
		if (m_stayCnt > m_stayTime)
		{
			if (m_blackFlg == false)
			{
				// 円を小さくする
				m_size -= m_subSize;

				// サイズが最小値を下回ったら終了
				if (m_size < m_minSize)
				{
					m_finishFlg = true;
					m_size = m_minSize;
					m_stayCnt = 0;
				}
			}
			// 黒フェードインの場合
			else
			{
				// 黒を濃くする
				m_alpha += m_distAlpha;

				// アルファ値が１以上になったら終了
				if (m_alpha >= 1)
				{
					m_finishFlg = true;
					m_startFlg = false;
					m_stayCnt = 0;
					m_alpha = 1;
				}
			}
		}
	}
}

void SceneChange::DrawSprite()
{
	if (m_startFlg == false && m_endFlg == false) return;

	if (m_blackFlg == false)
	{
		// 描画先のテクスチャを透明色(α値が0)でクリア
		KdDirect3D::Instance().WorkDevContext()->ClearRenderTargetView(m_spTmpTex->WorkRTView(), Math::Color(0.0f, 0.0f, 0.0f, 1.0f));

		// 描画先をテクスチャへ切り替え
		//KdDirect3D::Instance().GetDevContext()->
		KdDirect3D::Instance().WorkDevContext()->OMSetRenderTargets(1, m_spTmpTex->WorkRTViewAddress(), KdDirect3D::Instance().WorkZBuffer()->WorkDSView());

		// ブレンド方法を切り替え
		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);

		// ステンシル画像
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spCircleTex, 0, 0, (int)m_size, (int)m_size);

		// 描画先をバックバッファに切り替え
		KdDirect3D::Instance().WorkDevContext()->OMSetRenderTargets(1, KdDirect3D::Instance().WorkBackBuffer()->WorkRTViewAddress(), KdDirect3D::Instance().WorkZBuffer()->WorkDSView());

		// ブレンド方法を切り替え　ステンシル
		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Stencil);

		// ステンシル画像
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTmpTex, 0, 0, 1280, 720);

		// ブレンド方法を切り替え
		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);
	}
	// 黒フェードインアウトの場合
	else
	{
		Math::Color color = { m_color.x, m_color.y, m_color.z, m_alpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spWhiteTex, 0, 0, nullptr, &color);
	}
}

void SceneChange::Init()
{
	// 丸
	if (!m_spCircleTex)
	{
		m_spCircleTex = std::make_shared<KdTexture>();
		m_spCircleTex->Load("Asset/Textures/SceneChange/circle.png");
	}
	// 仮画像
	if (!m_spTmpTex)
	{
		m_spTmpTex = std::make_shared<KdTexture>();
		m_spTmpTex->CreateRenderTarget(1280, 720);
	}

	// 黒画像
	if (!m_spWhiteTex)
	{
		m_spWhiteTex = std::make_shared<KdTexture>();
		m_spWhiteTex->Load("Asset/Textures/SceneChange/white.png");
	}
}

void SceneChange::Reset()
{
	m_finishFlg = false;
	m_startFlg = false;
	m_endFlg = false;
	m_stayCnt = 0;
}

void SceneChange::StartScene(int _stayTime, bool _black, Math::Vector3 _color)
{
	if (m_endFlg == true) return;

	m_blackFlg = _black;

	if (!m_startFlg)
	{
		// シーンを始める際は、サイズを0から大きくしていく
		if (m_blackFlg == false)
		{
			m_size = m_minSize;
			m_startFlg = true;
			m_stayCnt = 0;
			m_stayTime = _stayTime;
		}
		// 黒フェードインの場合
		else
		{
			m_startFlg = true;
			m_alpha = 1.0f;
			m_color = { _color.x, _color.y, _color.z };
		}
	}
}

void SceneChange::EndScene(int _stayTime, bool _black, Math::Vector3 _color)
{
	if (m_startFlg == true) return;

	m_blackFlg = _black;

	if (!m_endFlg)
	{
		// シーンを終える際は、サイズを最大から小さくしていく
		if (m_blackFlg == false)
		{
			m_size = m_maxSize;
			m_endFlg = true;
			m_stayCnt = 0;
			m_stayTime = _stayTime;
		}
		// 黒フェードアウトの場合
		else
		{
			m_endFlg = true;
			m_alpha = 0.0f;
			m_color = { _color.x, _color.y, _color.z };
		}
	}
}