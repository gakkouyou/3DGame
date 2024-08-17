#include "SceneChange.h"

void SceneChange::Update()
{
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
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spCircleTex, 0, 0, 180, 180);

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

void SceneChange::StartScene()
{
	m_size = { 0, 0 };
	m_startFlg = true;
}

void SceneChange::EndScene()
{
	m_size = { 1280, 720 };
	m_endFlg = true;
}
