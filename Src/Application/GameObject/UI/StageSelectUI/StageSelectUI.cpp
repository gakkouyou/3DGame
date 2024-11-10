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
			if ((int)SceneManager::Instance().GetStageInfo()[nowStage - 1] == SceneManager::Instance().StageInfo::Clear)
			{
				KdShaderManager::Instance().m_spriteShader.DrawTex(m_clear.spTex, (int)m_clear.pos.x, (int)m_clear.pos.y);
			}
		}

		// 「ステージにはいる」
		if (m_stageIn.spTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_stageIn.spTex, (int)m_stageIn.pos.x, (int)m_stageIn.pos.y);
		}
	}
	m_onHitFlg = false;
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

	// "Clear!"
	if (!m_clear.spTex)
	{
		m_clear.spTex = std::make_shared<KdTexture>();
		m_clear.spTex->Load("Asset/Textures/StageSelect/UI/clear.png");
	}
	m_clear.pos = { -500, 230 };

	// 「ステージにはいる」
	m_stageIn.spTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/StageSelect/UI/stageInText.png");
	m_stageIn.pos = { 0, -300 };
}

void StageSelectUI::OnHit()
{
	m_onHitFlg = true;
}
