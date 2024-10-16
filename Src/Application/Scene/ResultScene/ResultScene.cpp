#include "ResultScene.h"
#include "../SceneManager.h"
#include "../../GameObject/SceneChange/SceneChange.h"
#include "Application/main.h"
#include "../../GameObject/Title/House/House.h"
#include "../../GameObject/BackGround/TitleBackGround/TitleBackGround.h"
#include "../../GameObject/BackGround/ResultBackGround/ResultBackGround.h"
#include "../../GameObject/Camera/ResultCamera/ResultCamera.h"
#include "../../GameObject/Character/ResultPlayer/ResultPlayer.h"
#include "../../GameObject/UI/TitleUI/TitleUI.h"

void ResultScene::Event()
{
	if (m_sceneStartFlg == false)
	{
		if (m_wpSceneChange.expired() == false)
		{
			m_wpSceneChange.lock()->StartScene(0, true, { 1, 1, 1 });

			if (m_wpSceneChange.lock()->GetFinishFlg() == true)
			{
				if (m_wpPlayer.expired() == false)
				{
					m_wpPlayer.lock()->SetStartAnimation();
				}
				m_sceneStartFlg = true;
				m_wpSceneChange.lock()->Reset();
			}
		}
	}
	else
	{
		if (m_wpPlayer.expired() == false)
		{
			if (m_wpPlayer.lock()->GetEndAnimation())
			{
				if (m_wpBackGround.expired() == false)
				{
					m_wpBackGround.lock()->StartAnimation();
				}

				m_ambientLight -= m_addAmbientLight;
				if (m_ambientLight < 0)
				{
					m_ambientLight = 0;
					SceneManager::Instance().SetNextScene(SceneManager::SceneType::Title);
				}
				KdShaderManager::Instance().WorkAmbientController().SetAmbientLight({ m_ambientLight, m_ambientLight, m_ambientLight, 1.0f });
			}
		}
	}
}

void ResultScene::Init()
{
	KdShaderManager::Instance().WorkAmbientController().SetDirLightShadowArea({ 50, 50 }, 50);

	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight({ m_ambientLight, m_ambientLight, m_ambientLight, 1.0f });
	KdShaderManager::Instance().WorkAmbientController().SetDirLight({ 0, -1, 1 }, { 0.7, 0.7, 0.7 });

	// シーンチェンジ
	std::shared_ptr<SceneChange> sceneChange = std::make_shared<SceneChange>();
	sceneChange->Init();
	AddObject(sceneChange);
	// 保持
	m_wpSceneChange = sceneChange;

	// タイトルUI
	std::shared_ptr<TitleUI> ui = std::make_shared<TitleUI>();
	ui->Init();
	//AddObject(ui);

	// 家
	std::shared_ptr<House> house = std::make_shared<House>();
	house->Init();
	AddObject(house);

	// 背景
	std::shared_ptr<ResultBackGround> backGround = std::make_shared<ResultBackGround>();
	backGround->Init();
	AddObject(backGround);
	m_wpBackGround = backGround;

	// プレイヤー
	std::shared_ptr<ResultPlayer> player = std::make_shared<ResultPlayer>();
	player->Init();
	AddObject(player);
	// 保持
	m_wpPlayer = player;

	// カメラ
	std::shared_ptr<ResultCamera> camera = std::make_shared<ResultCamera>();
	camera->Init();
	AddObject(camera);
}