#include "StageSelectUI.h"
#include "../../../Scene/SceneManager.h"

void StageSelectUI::Update()
{
	
}

void StageSelectUI::DrawSprite()
{
	int nowStage = SceneManager::Instance().GetNowStage();

	if (m_onHitFlg)
	{
		// "Stage"
		if (m_stage.spTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_stage.spTex, (int)m_stage.pos.x, (int)m_stage.pos.y);
		}

		// 数字
		if (m_number.spTex)
		{
			Math::Vector2 size = { (float)m_number.spTex->GetWidth() / 10, (float)m_number.spTex->GetHeight() };
			Math::Rectangle rect = { (long)size.x * (nowStage), (long)0, (long)size.x, (long)size.y };
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_number.spTex, (int)m_number.pos.x, (int)m_number.pos.y, (int)size.x, (int)size.y, &rect);
		}

		// Clear!
		if (m_clear.spTex)
		{
			if (SceneManager::Instance().GetStageInfo()[nowStage - 1] == SceneManager::Instance().Clear)
			{
				KdShaderManager::Instance().m_spriteShader.DrawTex(m_clear.spTex, (int)m_clear.pos.x, (int)m_clear.pos.y);
			}
		}
	}
	m_onHitFlg = false;

	//if (m_frame.spTex)
	//{
	//	KdShaderManager::Instance().m_spriteShader.DrawTex(m_frame.spTex, m_frame.pos.x, m_frame.pos.y);
	//}
}

void StageSelectUI::Init()
{
	// ステージ
	if (!m_stage.spTex)
	{
		m_stage.spTex = std::make_shared<KdTexture>();
		m_stage.spTex->Load("Asset/Textures/StageSelect/UI/stage.png");
	}
	m_stage.pos = { -500, 300 };

	// 数字
	if (!m_number.spTex)
	{
		m_number.spTex = std::make_shared<KdTexture>();
		m_number.spTex->Load("Asset/Textures/StageSelect/UI/number.png");
	}
	m_number.pos = { -370, 305 };

	// "PUSH SPACE"
	if (!m_space.spTex)
	{
		m_space.spTex = std::make_shared<KdTexture>();
		m_space.spTex->Load("Asset/Textures/StageSelect/UI/space.png");
	}
	m_space.pos = { 0, -250 };

	// プッシュスペースを見やすくするための白い画像
	if (!m_white.spTex)
	{
		m_white.spTex = std::make_shared<KdTexture>();
		m_white.spTex->Load("Asset/Textures/StageSelect/UI/white.png");
	}
	m_white.pos = m_space.pos;
	m_white.alpha = 0.7f;

	// "Clear!"
	if (!m_clear.spTex)
	{
		m_clear.spTex = std::make_shared<KdTexture>();
		m_clear.spTex->Load("Asset/Textures/StageSelect/UI/clear.png");
	}
	m_clear.pos = { -500, 230 };

	// 黒い画像
	if (!m_black.spTex)
	{
		m_black.spTex = std::make_shared<KdTexture>();
		m_black.spTex->Load("Asset/Textures/Scene/black.png");
	}
	m_black.alpha = 0.5f;

	//// ステージ数の枠
	//if (!m_frame.spTex)
	//{
	//	m_frame.spTex = std::make_shared<KdTexture>();
	//	m_frame.spTex->Load("Asset/Textures/StageSelect/UI/frame2.png");
	//}
	//m_frame.pos = { 0, -200 };
}

void StageSelectUI::OnHit()
{
	m_onHitFlg = true;
}
