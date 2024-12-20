﻿#include "SceneManager.h"

#include "BaseScene/BaseScene.h"
#include "TitleScene/TitleScene.h"
#include "GameScene/GameScene.h"
#include "StageSelectScene/StageSelectScene.h"
#include "ResultScene/ResultScene.h"

void SceneManager::PreUpdate()
{
	// シーン切替
	if (m_currentSceneType != m_nextSceneType)
	{
		ChangeScene(m_nextSceneType);
	}

	m_currentScene->PreUpdate();
}

void SceneManager::Update()
{
	m_currentScene->Update();
}

void SceneManager::PostUpdate()
{
	m_currentScene->PostUpdate();
}

void SceneManager::PreDraw()
{
	m_currentScene->PreDraw();
}

void SceneManager::Draw()
{
	m_currentScene->Draw();
}

void SceneManager::DrawSprite()
{
	m_currentScene->DrawSprite();
}

void SceneManager::DrawDebug()
{
	m_currentScene->DrawDebug();
}

const std::list<std::shared_ptr<KdGameObject>>& SceneManager::GetObjList()
{
	return m_currentScene->GetObjList();
}

void SceneManager::AddObject(const std::shared_ptr<KdGameObject>& obj)
{
	m_currentScene->AddObject(obj);
}

bool SceneManager::GetDebug() const
{
	return m_currentScene->GetDebug();
}

void SceneManager::ChangeScene(SceneType sceneType)
{
	// 次のシーンを作成し、現在のシーンにする
	switch (sceneType)
	{
	case SceneType::Title:
		m_currentScene = std::make_shared<TitleScene>();
		break;
	case SceneType::StageSelect:
		m_currentScene = std::make_shared<StageSelectScene>();
		break;
	case SceneType::Game:
		m_currentScene = std::make_shared<GameScene>();
		break;
	case SceneType::Result:
		m_currentScene = std::make_shared<ResultScene>();
		break;
	}
	m_currentScene->Init();

	// 現在のシーン情報を更新
	m_currentSceneType = sceneType;
}

void SceneManager::CSVReset()
{
	{
		std::ifstream ifs("Asset/Data/CSV/Terrain/StageSelectBase.csv");

		std::ofstream ofs("Asset/Data/CSV/Terrain/StageSelect.csv");

		if (!ifs.is_open())
		{
			return;
		}

		std::string lineString;

		while (std::getline(ifs, lineString))
		{
			std::istringstream iss(lineString);
			std::string commaString;

			while (std::getline(iss, commaString, ','))
			{
				ofs << commaString << ",";
			}
			ofs << std::endl;
		}
		ifs.close();
	}

	{
		std::ifstream ifs("Asset/Data/CSV/EventObject/StageSelectBase.csv");

		std::ofstream ofs("Asset/Data/CSV/EventObject/StageSelect.csv");

		if (!ifs.is_open())
		{
			return;
		}

		std::string lineString;

		while (std::getline(ifs, lineString))
		{
			std::istringstream iss(lineString);
			std::string commaString;

			while (std::getline(iss, commaString, ','))
			{
				ofs << commaString << ",";
			}
			ofs << std::endl;
		}
		ifs.close();
	}

	for (int i = 1; i < Stage::Max; i++)
	{
		std::ifstream ifs("Asset/Data/CSV/Terrain/Stage" + std::to_string(i) + "Base.csv");

		if (!ifs.is_open())
		{
			continue;
		}

		std::ofstream ofs("Asset/Data/CSV/Terrain/Stage" + std::to_string(i) + ".csv");

		std::string lineString;

		while (std::getline(ifs, lineString))
		{
			std::istringstream iss(lineString);
			std::string commaString;

			while (std::getline(iss, commaString, ','))
			{
				ofs << commaString << ",";
			}
			ofs << std::endl;
		}
		ifs.close();
	}

	for (int i = 1; i < Stage::Max; i++)
	{
		std::ifstream ifs("Asset/Data/CSV/EventObject/Stage" + std::to_string(i) + "Base.csv");

		if (!ifs.is_open())
		{
			continue;
		}

		std::ofstream ofs("Asset/Data/CSV/EventObject/Stage" + std::to_string(i) + ".csv");

		std::string lineString;

		while (std::getline(ifs, lineString))
		{
			std::istringstream iss(lineString);
			std::string commaString;

			while (std::getline(iss, commaString, ','))
			{
				ofs << commaString << ",";
			}
			ofs << std::endl;
		}
		ifs.close();
	}
}