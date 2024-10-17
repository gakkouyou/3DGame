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
					m_wpPlayer.lock()->SetFirstAnimationStart();
				}
				m_sceneStartFlg = true;
				m_wpSceneChange.lock()->Reset();
			}
		}
	}

	if (m_wpPlayer.expired() == false)
	{
		// プレイヤーの一つ目のアニメーションが終わった時の処理
		if (m_wpPlayer.lock()->GetFirstAnimationEnd())
		{
			if (m_wpBackGround.expired() == false)
			{
				// 空をオレンジにしていく
				m_wpBackGround.lock()->OrangeAnimation();

				// オレンジになりきったら、プレイヤーの二つ目のアニメーションを始める
				if (m_wpBackGround.lock()->GetOrangeAnimationEnd())
				{
					m_wpPlayer.lock()->SetSecondAnimationStart();
				}
			}
		}

		// ドアを開ける
		if (m_wpPlayer.lock()->GetOpenDoor())
		{
			if (m_wpHouse.expired() == false)
			{
				// ドアを開ける
				m_wpHouse.lock()->OpenDoor();
			}
		}

		// ドアを閉める
		if (m_wpPlayer.lock()->GetCloseDoor())
		{
			if (m_wpHouse.expired() == false)
			{
				// ドアを閉める
				m_wpHouse.lock()->CloseDoor();
			}
		}

		// プレイヤーのアニメーションが完全に終了したときの処理
		if (m_wpPlayer.lock()->GetAnimationEnd())
		{
			if (m_wpBackGround.expired() == false)
			{
				m_wpBackGround.lock()->BlackAnimation();
			}
		}
	}
}

void ResultScene::Init()
{
	KdShaderManager::Instance().WorkAmbientController().SetDirLightShadowArea({ 50, 50 }, 50);

	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight({ 0.6, 0.6, 0.6, 1.0f });
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
	// 保持
	m_wpHouse = house;

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