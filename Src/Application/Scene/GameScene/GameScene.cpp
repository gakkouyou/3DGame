#include "GameScene.h"
#include "../SceneManager.h"

#include "../../GameObject/Camera/TPSCamera/TPSCamera.h"
#include "../../GameObject/Character/Player/Player.h"
#include "../../GameObject/BackGround/BackGround.h"
#include "../../GameObject/EventObject/Goal/Goal.h"
#include "../../GameObject/SceneChange/SceneChange.h"
#include "../../GameObject/Result/Result.h"
#include "../../GameObject/StageStart/StageStart.h"
#include "../../GameObject/Effect/PlayerSmoke/PlayerSmoke.h"
#include "../../GameObject/Pause/Pause.h"
#include "../../GameObject/UI/GameUI/GameUI.h"

#include "../../Tool/DebugWindow/DebugWindow.h"
#include "../../Tool/ObjectController/TerrainController/TerrainController.h"
#include "../../Tool/ObjectController/EnemyController/EnemyController.h"

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
			if (spPlayer->GetAlive() == false)
			{
				if (!m_wpResult.expired())
				{
					m_wpResult.lock()->GameOver();
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
	if (!m_wpGoal.expired())
	{
		if (m_wpGoal.lock()->GetGoalFlg())
		{
			if (!m_wpCamera.expired())
			{
				m_wpCamera.lock()->SetGoalFlg(true);
				// ステージをクリアにする
				m_stageInfoList[m_nowStage] = 2;
				// 次のステージを挑戦可能状態にする
				m_stageInfoList[m_nowStage + 1] = 1;

				// 動き終わった後の処理
				if (m_wpCamera.lock()->GetGoalProcessFinish())
				{
					if (!m_wpResult.expired())
					{
						m_wpResult.lock()->StageClear();

						if (m_wpResult.lock()->GetClearFinish())
						{
							if (!m_wpSceneChange.expired())
							{
								m_wpSceneChange.lock()->EndScene();

								if (m_wpSceneChange.lock()->GetFinishFlg())
								{
									// CSVに書き込む
									CSVWriter();
									SceneManager::Instance().SetNextScene(SceneManager::SceneType::StageSelect);
								}
							}
						}
					}
				}
			}
			// プレイヤーにゴールしたことを伝え、止める
			if (!m_wpPlayer.expired())
			{
				m_wpPlayer.lock()->SetGoalFlg(true);
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
void GameScene::Init()
{
	KdShaderManager::Instance().WorkAmbientController().SetDirLight({ 0, -1, 1 }, { 3, 3, 3 });

	// ②フォグ(霧)                                                 ↓距離 ↓高さ
	KdShaderManager::Instance().WorkAmbientController().SetFogEnable(false, true);
	// 高さフォグ                                                    ↓色       ↓上↓下↓距離
	KdShaderManager::Instance().WorkAmbientController().SetheightFog({ 1, 1, 1 }, 0, -2, 0.1f);

	// ステージをゲット
	m_nowStage = SceneManager::Instance().GetNowStage();

	// CSVを読み込む
	CSVLoader();

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
	tpsCamera->Init();
	AddObject(tpsCamera);
	// カメラの情報を保持する
	m_wpCamera = tpsCamera;

	// プレイヤーにTPSカメラをセットする
	player->SetCamera(tpsCamera);

	// TPSカメラにターゲットをセットする
	tpsCamera->SetTarget(player);

	// ゴール
	std::shared_ptr<Goal> goal = std::make_shared<Goal>();
	goal->SetPos({ 220, 30, 50 });
	//goal->SetPos({ 20, 12, -10 });
	goal->Init();
	AddObject(goal);
	// 保持
	m_wpGoal = goal;

	// カメラにゴールの座標をセットする
	tpsCamera->SetGoalPos(goal->GetPos());
	// プレイヤーにゴールの座標をセットする
	player->SetGoalPos(goal->GetPos());

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









	// シーンを変える sprite描画する中では一番下に置く
	std::shared_ptr<SceneChange> sceneChange = std::make_shared<SceneChange>();
	sceneChange->Init();
	AddObject(sceneChange);
	// 保持
	m_wpSceneChange = sceneChange;

	// マップエディタ的な
	std::shared_ptr<TerrainController> terrainController = std::make_shared<TerrainController>();
	// CSVファイルを指定する
	terrainController->SetCSV("Asset/Data/CSV/Terrain/Stage" + std::to_string(m_nowStage + 1) + ".csv");
	terrainController->Init();
	AddObject(terrainController);

	// 敵エディタ的な
	std::shared_ptr<EnemyController> enemyController = std::make_shared<EnemyController>();
	enemyController->SetCSV("Asset/Data/CSV/Enemy/Stage" + std::to_string(m_nowStage + 1) + ".csv");
	// enemyControllerにプレイヤーを渡す
	enemyController->SetPlayer(player);
	enemyController->Init();
	AddObject(enemyController);

	// デバッグウィンドウにオブジェクトコントローラーを渡す
	DebugWindow::Instance().SetTerrainController(terrainController);// Terrain
	DebugWindow::Instance().SetEnemyController(enemyController);	// Enemy


	// オブジェクトコントローラーにカメラを渡す
	terrainController->SetCamera(tpsCamera);
	enemyController->SetCamera(tpsCamera);

	// プレイヤーにterrainControllerを渡す
	player->SetTerrainController(terrainController);
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
			// 敵、地形を全て削除する
			auto it = m_objList.begin();
			while (it != m_objList.end())
			{
				if ((*it)->GetBaseObjectType() == KdGameObject::BaseObjectType::Enemy || (*it)->GetBaseObjectType() == KdGameObject::BaseObjectType::Ground)
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
				// リセット(敵、地形も作り直される)
				obj->Reset();
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

			// フェードインが終了したら"Stage Start"を消していく
			//if (!m_wpStart.expired())
			//{
			//	m_wpStart.lock()->SetEnd();
			//}
		}
		else
		{
			m_wpSceneChange.lock()->StartScene(5);
		}
	}
}


void GameScene::CSVLoader()
{
	std::ifstream ifs("Asset/Data/CSV/StageInfo.csv");

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
			m_stageInfoList.push_back(stoi(commaString));
		}
	}
	ifs.close();
}

void GameScene::CSVWriter()
{
	std::ofstream ofs("Asset/Data/CSV/StageInfo.csv");

	for (int i=0; i < m_stageInfoList.size(); i++)
	{
		ofs << m_stageInfoList[i];
		
		if (i != m_stageInfoList.size() - 1)
		{
			ofs << ",";
		}
	}
	ofs << std::endl;
}