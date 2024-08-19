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
	}

	// シーン終了
	if (m_endFlg)
	{
		// 円を小さくする
		m_size -= m_addSize;

		// サイズが最小値を下回ったら終了
		if (m_size < m_minSize)
		{
			m_finishFlg = true;
			m_size = m_minSize;
			//m_endFlg = false;
		}
	}
}

void SceneChange::DrawSprite()
{
	if (m_startFlg == false && m_endFlg == false) return;
	// 描画先のテクスチャを透明色(α値が0)でクリア
	KdDirect3D::Instance().WorkDevContext()->ClearRenderTargetView(m_spTmpTex->WorkRTView(), Math::Color(0.0f, 0.0f, 0.0f, 1.0f));

	// 描画先をテクスチャへ切り替え
	//KdDirect3D::Instance().GetDevContext()->
	KdDirect3D::Instance().WorkDevContext()->OMSetRenderTargets(1, m_spTmpTex->WorkRTViewAddress(), KdDirect3D::Instance().WorkZBuffer()->WorkDSView());
	
	// ブレンド方法を切り替え
	KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);

	// ステンシル画像
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spCircleTex, 0, 0, m_size, m_size);

	// 描画先をバックバッファに切り替え
	KdDirect3D::Instance().WorkDevContext()->OMSetRenderTargets(1, KdDirect3D::Instance().WorkBackBuffer()->WorkRTViewAddress(), KdDirect3D::Instance().WorkZBuffer()->WorkDSView());

	// ブレンド方法を切り替え　ステンシル
	KdShaderManager::Instance().ChangeBlendState(KdBlendState::Stencil);

	// ステンシル画像
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTmpTex, 0, 0, 1280, 720);

	// ブレンド方法を切り替え
	KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);
}

void SceneChange::Init()
{
	if (!m_spCircleTex)
	{
		m_spCircleTex = std::make_shared<KdTexture>();
		m_spCircleTex->Load("Asset/Textures/Scene/circle.png");
	}
	if (!m_spTmpTex)
	{
		m_spTmpTex = std::make_shared<KdTexture>();
		m_spTmpTex->CreateRenderTarget(1280, 720);
	}
}

void SceneChange::Reset()
{
	m_finishFlg = false;
	m_startFlg = false;
	m_endFlg = false;
	m_stayCnt = 0;
}

void SceneChange::StartScene()
{
	// シーンを始める際は、サイズを0から大きくしていく
	if (!m_startFlg)
	{
		m_size = m_minSize;
		m_startFlg = true;
	}
}

void SceneChange::EndScene()
{
	// シーンを終える際は、サイズを最大から小さくしていく
	if (!m_endFlg)
	{
		m_size = m_maxSize;
		m_endFlg = true;
	}
}
