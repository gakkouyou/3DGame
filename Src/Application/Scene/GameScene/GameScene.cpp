﻿#include "GameScene.h"
#include "../SceneManager.h"

#include "../../GameObject/Camera/TPSCamera/TPSCamera.h"
#include "../../GameObject/Character/Player/Player.h"
#include "../../GameObject/BackGround/BackGround.h"
#include "../../GameObject/EventObject/Goal/Goal.h"
#include "../../GameObject/SceneChange/SceneChange.h"
#include "../../GameObject/Result/Result.h"
#include "../../GameObject/StageStart/StageStart.h"
#include "../../GameObject/Effect/Smoke/Smoke.h"
#include "../../GameObject/Pause/Pause.h"
#include "../../GameObject/UI/GameUI/GameUI.h"

#include "../../Tool/DebugWindow/DebugWindow.h"
#include "../../Tool/ObjectController/TerrainController/TerrainController.h"
#include "../../Tool/ObjectController/EnemyController/EnemyController.h"
#include "../../Tool/ObjectController/CarryObjectController/CarryObjectController.h"
#include "../../Tool/ObjectController/EventObjectController/EventObjectController.h"

#include "../../GameObject/EventObject/SavePoint/SavePoint.h"

#include "../../main.h"

void GameScene::Event()
{
	// ポーズ画面の時はやらない
	// デバッグ用　ENTERを押すと、マップを配置できるデバッグモードになる
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		if (!m_debugKeyFlg)
		{
			if (!m_pauseFlg)
			{
				m_debugFlg = !m_debugFlg;
			}
			m_debugKeyFlg = true;
		}
	}
	else
	{
		m_debugKeyFlg = false;
	}

	// シーンが開始した時の処理
	if (!m_sceneStartFlg)
	{
		StartGameScene();
	}

	// プレイヤーが死んだときの処理
	if (!m_resetFlg)
	{
		std::shared_ptr<Player> spPlayer = m_wpPlayer.lock();
		if (spPlayer)
		{
			if (spPlayer->GetAliveFlg() == false)
			{
				if (!m_wpResult.expired())
				{
					m_wpResult.lock()->GameOver();
				}

				for (auto& obj : m_objList)
				{
					// 全てのオブジェクトを止める(プレイヤーは止まらない)
					obj->SetPauseFlg(true);
				}

				// ゲームシーンは終了せずに終了
				GameEnd(60);
			}
		}
	}



	// ポーズ画面の処理
	if (!m_wpPause.expired())
	{
		m_pauseFlg = m_wpPause.lock()->GetPauseFlg();
	}
	// ポーズ画面かどうかが変わっていた時だけ処理する
	if (m_pauseFlg != m_oldPauseFlg)
	{
		// ポーズかどうかをオブジェクトに渡す
		for (auto& obj : m_objList)
		{
			if (obj)
			{
				obj->SetPauseFlg(m_pauseFlg);
			}
		}
	}
	// 前のポーズフラグを保持しておく
	m_oldPauseFlg = m_pauseFlg;

	// 選ばれたポーズのボタンによって、処理を変えていく
	if (!m_wpPause.expired())
	{
		if (!m_wpSceneChange.expired())
		{
			switch (m_wpPause.lock()->GetSelectButton())
			{
			case Pause::Again:	// ステージをやり直す
				GameEnd(0);
				break;

			case Pause::Back:	// ステージから出る
				if (m_wpSceneChange.lock()->GetFinishFlg())
				{
					SceneManager::Instance().SetNextScene(SceneManager::SceneType::StageSelect);
				}
				else
				{
					m_wpSceneChange.lock()->EndScene();
					// 音をフェードアウト
					m_vol -= 0.001f;
					if (m_vol < 0)
					{
						m_vol = 0;
					}
					if (m_bgm.expired() == false)
					{
						m_bgm.lock()->SetVolume(m_vol);
					}
				}
				break;
			}
		}
	}

	// シーンを切り替えずに、ゲームシーンを再開する
	if (m_resetFlg)
	{
		GameSceneReStart();
	}

	// クリアした時の処理
	if (!m_wpPlayer.expired())
	{
		if (m_wpPlayer.lock()->GetGoalFlg())
		{
			// 音をフェードアウト
			m_vol -= 0.001f;
			if (m_vol < 0)
			{
				m_vol = 0;
			}
			if (m_bgm.expired() == false)
			{
				m_bgm.lock()->SetVolume(m_vol);
			}

			// UIにクリアしたことを伝える
			if (m_wpGameUI.expired() == false)
			{
				m_wpGameUI.lock()->SetClearFlg(true);
			}

			if (!m_wpCamera.expired())
			{
				m_wpCamera.lock()->SetGoalFlg(true);

				// 動き終わった後の処理
				if (m_wpCamera.lock()->GetGoalProcessFinish())
				{
					if (!m_wpResult.expired())
					{
						// ステージクリアのリザルトを出す
						m_wpResult.lock()->StageClear();

						if (m_wpResult.lock()->GetClearFinish())
						{
							if (!m_wpSceneChange.expired())
							{
								// シーンを終了
								m_wpSceneChange.lock()->EndScene();

								// 処理が終了したらシーンを変更
								if (m_wpSceneChange.lock()->GetFinishFlg())
								{
									// 初クリアかどうかをチェック
									if (SceneManager::Instance().GetStageInfo()[m_nowStage - 1] == SceneManager::StageInfo::NotClear)
									{
										SceneManager::Instance().WorkStageInfo()[m_nowStage - 1] = SceneManager::StageInfo::FirstClear;
									}

									// ステージのCSVをリセット
									ResetCSV();

									// CSVに書き込む
									//SceneManager::Instance().StageInfoCSVWriter();
									// シーンを変更
									SceneManager::Instance().SetNextScene(SceneManager::SceneType::StageSelect);
								}
							}
						}
					}
				}
			}
			// ポーズも止める
			if (!m_wpPause.expired())
			{
				m_wpPause.lock()->SetStopFlg(true);
			}
		}
	}
}

void GameScene::Init()
{
	KdShaderManager::Instance().WorkAmbientController().SetDirLight({ 1, -1, 1 }, { 3, 3, 3 });

	KdShaderManager::Instance().WorkAmbientController().SetDirLightShadowArea({50,50},50);

	// ②フォグ(霧)                                                 ↓距離 ↓高さ
	//KdShaderManager::Instance().WorkAmbientController().SetFogEnable(true, true);

	// 距離フォグ                                                        ↓色RGB      ↓密度
	//KdShaderManager::Instance().WorkAmbientController().SetDistanceFog({ 0, 0, 0.5 }, 0.001);

	// 高さフォグ                                                    ↓色       ↓上↓下↓距離
	//KdShaderManager::Instance().WorkAmbientController().SetheightFog({ 0, 0, 0.5 }, 10, -10, 0.001f);

	// ステージをゲット
	m_nowStage = SceneManager::Instance().GetNowStage();

	// 背景(最初に描画する！！！！！！)
	std::shared_ptr<BackGround> backGround = std::make_shared<BackGround>();
	backGround->Init();
	AddObject(backGround);

	// プレイヤー
	std::shared_ptr<Player> player = std::make_shared<Player>();
	player->Init();
	AddObject(player);
	// 保持
	m_wpPlayer = player;

	// TPSカメラ
	std::shared_ptr<TPSCamera> tpsCamera = std::make_shared<TPSCamera>();
	// TPSカメラにターゲットをセットする
	tpsCamera->SetPlayer(player);
	tpsCamera->Init();
	AddObject(tpsCamera);
	// カメラの情報を保持する
	m_wpCamera = tpsCamera;

	// プレイヤーにTPSカメラをセットする
	player->SetCamera(tpsCamera);

	// リザルト
	std::shared_ptr<Result> result = std::make_shared<Result>();
	result->Init();
	AddObject(result);
	// 保持
	m_wpResult = result;

	// "Stage Start"
	//std::shared_ptr<StageStart> stageStart = std::make_shared<StageStart>();
	//stageStart->Init();
	//AddObject(stageStart);
	//// 保持
	//m_wpStart = stageStart;

	// ポーズ画面
	std::shared_ptr<Pause> pause = std::make_shared<Pause>();
	pause->Init();
	AddObject(pause);
	// 保持
	m_wpPause = pause;

	// UI
	std::shared_ptr<GameUI> ui = std::make_shared<GameUI>();
	ui->Init();
	AddObject(ui);
	// プレイヤーを渡す
	ui->SetPlayer(player);
	// 保持
	m_wpGameUI = ui;

	// シーンを変える sprite描画する中では一番下に置く
	std::shared_ptr<SceneChange> sceneChange = std::make_shared<SceneChange>();
	sceneChange->Init();
	AddObject(sceneChange);
	// 保持
	m_wpSceneChange = sceneChange;

	// 地形作成
	std::shared_ptr<TerrainController> terrainController = std::make_shared<TerrainController>();
	std::string csvName = "Asset/Data/CSV/Terrain/Stage" + std::to_string(m_nowStage);
	// CSVファイルを指定する
	terrainController->SetCSV(csvName);

	// 敵作成
	std::shared_ptr<EnemyController> enemyController = std::make_shared<EnemyController>();
	csvName = "Asset/Data/CSV/Enemy/Stage" + std::to_string(m_nowStage) + ".csv";
	enemyController->SetCSV(csvName);
	// enemyControllerにプレイヤーを渡す
	enemyController->SetPlayer(player);
	// 敵にTerrainControllerを渡す
	enemyController->SetTerrainController(terrainController);
	enemyController->Init();
	AddObject(enemyController);

	// 運ぶオブジェクト作成
	std::shared_ptr<CarryObjectController> carryObjectController = std::make_shared<CarryObjectController>();
	// CSVファイルを指定する
	carryObjectController->SetCSV("Asset/Data/CSV/CarryObject/Stage" + std::to_string(m_nowStage) + ".csv");
	// Initより先に書く
	carryObjectController->SetTerrainController(terrainController);
	carryObjectController->SetPlayer(player);
	carryObjectController->Init();
	AddObject(carryObjectController);

	// EventObject作成
	std::shared_ptr<EventObjectController> eventObjectController = std::make_shared<EventObjectController>();
	// CSVファイルを指定する
	csvName = "Asset/Data/CSV/EventObject/Stage" + std::to_string(m_nowStage);
	eventObjectController->SetCSV(csvName);
	// Initより先に書く
	eventObjectController->SetPlayer(player);
	eventObjectController->SetCamera(tpsCamera);
	eventObjectController->Init();
	AddObject(eventObjectController);

	// ===========================================================================
	// 地形によって挙動が変わるオブジェクトの後に更新するように書く
	// ===========================================================================
	terrainController->Init();
	AddObject(terrainController);

	// デバッグウィンドウにオブジェクトコントローラーを渡す
	DebugWindow::Instance().SetTerrainController(terrainController);			// Terrain
	DebugWindow::Instance().SetEnemyController(enemyController);				// Enemy
	DebugWindow::Instance().SetCarryObjectController(carryObjectController);	// CarryObject
	DebugWindow::Instance().SetEventObjectController(eventObjectController);	// EventObject


	// オブジェクトコントローラーにカメラを渡す
	terrainController->SetCamera(tpsCamera);
	enemyController->SetCamera(tpsCamera);
	carryObjectController->SetCamera(tpsCamera);

	// プレイヤーにterrainControllerを渡す
	player->SetTerrainController(terrainController);
	player->SetCarryObjectContoller(carryObjectController);
	player->SetEventObjectController(eventObjectController);

	m_bgm = KdAudioManager::Instance().Play("Asset/Sounds/BGM/stageBGM.wav", true);
	m_bgm.lock()->SetVolume(m_vol);
}

void GameScene::StartGameScene()
{
	if (!m_wpSceneChange.expired())
	{
		m_wpSceneChange.lock()->StartScene(5);

		if (m_wpSceneChange.lock()->GetFinishFlg())
		{
			m_sceneStartFlg = true;
			m_wpSceneChange.lock()->Reset();
			// フェードインが終了したらプレイヤーを動けるようにする
			if (!m_wpPlayer.expired())
			{
				m_wpPlayer.lock()->SetStopFlg(false);
			}
			// ポーズもできるようにする
			if (!m_wpPause.expired())
			{
				m_wpPause.lock()->SetStopFlg(false);
			}
			// フェードインが終了したら"Stage Start"を消していく
			//if (!m_wpStart.expired())
			//{
			//	m_wpStart.lock()->SetEnd();
			//}
		}
		else
		{
			// 終わるまでプレイヤーを止める
			if (!m_wpPlayer.expired())
			{
				m_wpPlayer.lock()->SetStopFlg(true);
			}
			// ポーズも止める
			if (!m_wpPause.expired())
			{
				m_wpPause.lock()->SetStopFlg(true);
			}
		}
	}
}

void GameScene::GameEnd(int _stayCnt)
{
	if (!m_wpSceneChange.expired())
	{
		// シーン遷移を終えたらリセット
		if (m_wpSceneChange.lock()->GetFinishFlg())
		{
			// 敵、地形、運べるオブジェクトを全て削除する
			auto it = m_objList.begin();
			while (it != m_objList.end())
			{
				if ((*it)->GetBaseObjectType() == KdGameObject::BaseObjectType::Enemy
					//|| (*it)->GetBaseObjectType() == KdGameObject::BaseObjectType::Ground
					|| (*it)->GetBaseObjectType() == KdGameObject::BaseObjectType::CarryObject
					|| (*it)->GetBaseObjectType() == KdGameObject::BaseObjectType::Event)
				{
					(*it)->SetExpired(true);
					it = m_objList.erase(it);
				}
				else
				{
					it++;
				}
			}

			for (auto& obj : m_objList)
			{
				// リセット(敵、地形、運べるオブジェクトも作り直される)
				obj->Reset();
			}

			// 地形をリストの最後にする
			{
				std::list<std::shared_ptr<KdGameObject>> objList = m_objList;
				std::list<std::shared_ptr<KdGameObject>> terrainList;
				auto it = m_objList.begin();
				for (auto& obj : objList)
				{
					if (obj->GetBaseObjectType() == KdGameObject::BaseObjectType::Ground)
					{
						m_objList.push_back(obj);
						it = m_objList.erase(it);
					}
					else
					{
						it++;
					}
				}
			}

			m_resetFlg = true;
		}
		else
		{
			m_wpSceneChange.lock()->EndScene(_stayCnt);
			// フェードアウト、フェードインが終わるまでプレイヤーを止める
			if (!m_wpPlayer.expired())
			{
				m_wpPlayer.lock()->SetStopFlg(true);
			}
			// ポーズも止める
			if (!m_wpPause.expired())
			{
				m_wpPause.lock()->SetStopFlg(true);
			}
		}
	}
}

void GameScene::GameSceneReStart()
{
	if (!m_wpSceneChange.expired())
	{
		if (m_wpSceneChange.lock()->GetFinishFlg())
		{
			m_resetFlg = false;
			m_wpSceneChange.lock()->Reset();
			// フェードイン終了したらプレイヤーを動かせるようにする
			if (!m_wpPlayer.expired())
			{
				m_wpPlayer.lock()->SetStopFlg(false);
			}
			// ポーズも動かせるようにする
			if (!m_wpPause.expired())
			{
				m_wpPause.lock()->SetStopFlg(false);
			}
		}
		else
		{
			m_wpSceneChange.lock()->StartScene(5);
		}
	}
}

void GameScene::ResetCSV()
{
	{
		std::ifstream ifs("Asset/Data/CSV/Terrain/BaseStage" + std::to_string(m_nowStage) + ".csv");

		if (ifs.is_open())
		{
			std::ofstream ofs("Asset/Data/CSV/Terrain/Stage" + std::to_string(m_nowStage) + ".csv");

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

	{
		std::ifstream ifs("Asset/Data/CSV/EventObject/BaseStage" + std::to_string(m_nowStage) + ".csv");

		if (ifs.is_open())
		{
			std::ofstream ofs("Asset/Data/CSV/EventObject/Stage" + std::to_string(m_nowStage) + ".csv");

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
}