#include "StageSelectTexture.h"

void StageSelectTexture::Update()
{
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		if (m_rightKeyFlg == false)
		{
			m_rightKeyFlg = true;
			m_nowStage++;
			if (m_nowStage >= StageNum::Max)
			{
				m_nowStage = StageNum::Max - 1;
			}
		}
	}
	else
	{
		m_rightKeyFlg = false;
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		if (m_leftKeyFlg == false)
		{
			m_leftKeyFlg = true;
			m_nowStage--;
			if (m_nowStage < 0)
			{
				m_nowStage = 0;
			}
		}
	}
	else
	{
		m_leftKeyFlg = false;
	}
}

void StageSelectTexture::DrawSprite()
{
	if (m_spTex[m_nowStage])
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex[m_nowStage], 0, 0);
	}
}

void StageSelectTexture::Init()
{
	for (int i = 0; i < StageNum::Max; i++)
	{
		if (!m_spTex[i])
		{
			m_spTex[i] = std::make_shared<KdTexture>();

			// ファイルパス
			std::string filePath = "Asset/Textures/StageSelect/Stage/Stage" + std::to_string(i + 1) + ".png";

			// 画像ロード
			m_spTex[i]->Load(filePath);
		}
	}
}