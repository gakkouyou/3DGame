#include "SceneChange.h"

void SceneChange::Update()
{
}

void SceneChange::DrawSprite()
{
	//// 描画先のテクスチャを透明色(α値が0)でクリア
	////m_spTmpTex->ClearRenderTarget(Math::Color(0.0f, 0.0f, 0.0f, 1.0f));
	//KdDirect3D::Instance().WorkDevContext()->ClearRenderTargetView(m_spTmpTex->WorkRTView(), Math::Color(0.0f, 0.0f, 0.0f, 1.0f));

	//// 描画先をテクスチャへ切り替え
	////KdDirect3D::Instance().GetDevContext()->
	//KdDirect3D::Instance().WorkDevContext()->OMSetRenderTargets(1, m_spTmpTex->WorkRTViewAddress(), KdDirect3D::Instance().WorkZBuffer()->WorkDSView());
	//
	//// ブレンド方法を切り替え
	//KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);

	//// ステンシル画像
	//KdShaderManager::Instance().m_spriteShader.DrawTex(m_spCircleTex, 0, 0);

	//// 描画先をバックバッファに切り替え
	//KdDirect3D::Instance().WorkDevContext()->OMSetRenderTargets(1, KdDirect3D::Instance().WorkBackBuffer()->WorkRTViewAddress(), KdDirect3D::Instance().WorkZBuffer()->WorkDSView());

	//// ブレンド方法を切り替え
	//D3D.SetBlendState(BlendMode::Alpha);

	// 黒画像
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spBlackTex, 0, 0);

	// ブレンド方法を切り替え　ステンシル
	KdDirect3D::Instance().CreateBlendState(KdBlendMode::Stencil);

	// ステンシル画像
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spCircleTex, 0, 0);

	// ブレンド方法を切り替え　ステンシル
	KdDirect3D::Instance().CreateBlendState(KdBlendMode::Alpha);
}

void SceneChange::Init()
{
	if (!m_spBlackTex)
	{
		m_spBlackTex = std::make_shared<KdTexture>();
		m_spBlackTex->Load("Asset/Textures/Scene/black.png");
	}
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
	m_size = { 6400, 3600 };
}

void SceneChange::EndScene()
{
	m_size = { 1280, 720 };
}
