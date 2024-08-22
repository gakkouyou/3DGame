#include "StageSelectTexture.h"

void StageSelectTexture::Update()
{
	if (m_stopFlg == false)
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
	else
	{
		m_leftKeyFlg = true;
		m_rightKeyFlg = true;
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
	CSVLoader();

	for (int i = 0; i < StageNum::Max - 1; i++)
	{
		if (m_stageInfoList[i] == StageInfo::Clear)
		{
			if (m_stageInfoList[i + 1] == StageInfo::ImPossible)
			{
				m_stageInfoList[i + 1] = StageInfo::Possible;
				break;
			}
		}
	}

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

	// 止めるフラグ
	m_stopFlg = true;
}

void StageSelectTexture::CSVLoader()
{
	std::ifstream ifs("Asset/Data/CSV/StageInfo.csv");

	if (!ifs.is_open())
	{
		return;
	}

	std::string lineString;

	int cnt = 0;

	while (std::getline(ifs, lineString))
	{
		std::istringstream iss(lineString);
		std::string commaString;

		while (std::getline(iss, commaString, ','))
		{
			m_stageInfoList[cnt] = stoi(commaString);
			cnt++;
		}
	}
	ifs.close();
}

void StageSelectTexture::CSVWriter()
{
	std::ofstream ofs("Asset/Data/CSV/StageInfo.csv");

	for (int i = 0; i < StageNum::Max; i++)
	{
		ofs << m_stageInfoList[i];

		if (i != StageNum::Max - 1)
		{
			ofs << ",";
		}
	}
	ofs << std::endl;
}