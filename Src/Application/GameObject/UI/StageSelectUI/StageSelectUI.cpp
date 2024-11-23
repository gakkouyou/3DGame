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
	DataLoad();

	// ステージ
	m_stage.spTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/StageSelect/UI/stage.png");
	// 数字
	m_number.spTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/StageSelect/UI/number.png");
	// "Clear!"
	m_clear.spTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/StageSelect/UI/clear.png");
	// 「ステージにはいる」
	m_stageIn.spTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/StageSelect/UI/stageInText.png");
}

void StageSelectUI::OnHit()
{
	m_onHitFlg = true;
}

void StageSelectUI::DataLoad()
{
	// JSONファイルを読み込む
	std::ifstream file(m_path.data());
	if (!file.is_open()) return;

	nlohmann::json data;
	file >> data;

	// JSONデータを格納していく
	// Stage
	m_stage.pos.x	= data["StageSelectUI"]["m_stage.pos.x"];
	m_stage.pos.y	= data["StageSelectUI"]["m_stage.pos.y"];
	// Stageの数
	m_number.pos.x	= data["StageSelectUI"]["m_number.pos.x"];
	m_number.pos.y	= data["StageSelectUI"]["m_number.pos.y"];
	// "Clear!"
	m_clear.pos.x	= data["StageSelectUI"]["m_clear.pos.x"];
	m_clear.pos.y	= data["StageSelectUI"]["m_clear.pos.y"];
	// "ステージにはいる
	m_stageIn.pos.x = data["StageSelectUI"]["m_stageIn.pos.x"];
	m_stageIn.pos.y = data["StageSelectUI"]["m_stageIn.pos.y"];
}
