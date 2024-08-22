#include "GameScene.h"
#include "../SceneManager.h"

#include "../../GameObject/Camera/TPSCamera/TPSCamera.h"
#include "../../GameObject/Character/Player/Player.h"
#include "../../GameObject/Terrain/Ground/NormalGround/NormalGround.h"
#include "../../GameObject/Terrain/Ground/BoundGround/BoundGround.h"
#include "../../GameObject/BackGround/BackGround.h"
#include "../../GameObject/Terrain/Ground/RotationGround/RotationGround.h"
#include "../../GameObject/EventObject/Goal/Goal.h"
#include "../../GameObject/Character/Enemy/NormalEnemy/NormalEnemy.h"
#include "../../GameObject/SceneChange/SceneChange.h"
#include "../../GameObject/Terrain/Object/Fence/Fence.h"
#include "../../GameObject/Result/Result.h"
#include "../../GameObject/StageStart/StageStart.h"
#include "../../GameObject/Terrain/Ground/DropGround/DropGround.h"

#include "../../Tool/DebugWindow/DebugWindow.h"
#include "../../Tool/ObjectController/TerrainController/TerrainController.h"
#include "../../Tool/ObjectController/EnemyController/EnemyController.h"

#include "../../main.h"

void GameScene::Event()
{
	// デバッグ用　ENTERを押すと、マップを配置できるデバッグモードになる
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		if (!m_debugKeyFlg)
		{
			m_debugFlg = !m_debugFlg;
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
			}
		}
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

				if (!m_wpSceneChange.expired())
				{
					// シーン遷移を終えたらリセット
					if (m_wpSceneChange.lock()->GetFinishFlg())
					{
						for (auto& obj : m_objList)
						{
							// 敵、地形を全消去
							if (obj->GetBaseObjectType() == KdGameObject::BaseObjectType::Enemy || obj->GetBaseObjectType() == KdGameObject::BaseObjectType::Ground)
							{
								obj->SetExpired(true);
							}
							// リセット(敵、地形も作り直される)
							obj->Reset();
							m_resetFlg = true;
						}
					}
					else
					{
						m_wpSceneChange.lock()->EndScene(60);
						// フェードアウト、フェードインが終わるまでプレイヤーを止める
						if (!m_wpPlayer.expired())
						{
							m_wpPlayer.lock()->SetStopFlg(true);
						}
					}
				}
			}
		}
	}

	// リセットする
	if (m_resetFlg)
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

	// クリアした時の処理
	if(!m_wpGoal.expired())
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
			if (!m_wpPlayer.expired())
			{
				m_wpPlayer.lock()->SetGoalFlg(true);
				m_wpPlayer.lock()->SetStopFlg(true);
			}
		}
	}
}

void GameScene::Init()
{
	// ステージをゲット
	m_nowStage = SceneManager::Instance().GetNowStage();

	// CSVを読み込む
	CSVLoader();

	// マップエディタ的な
	std::shared_ptr<TerrainController> objectController = std::make_shared<TerrainController>();
	// CSVファイルを指定する
	objectController->SetCSV("Asset/Data/CSV/Terrain/Stage" + std::to_string(m_nowStage + 1) + ".csv");
	objectController->Init();
	AddObject(objectController);

	// 敵エディタ的な
	std::shared_ptr<EnemyController> enemyController = std::make_shared<EnemyController>();
	enemyController->SetCSV("Asset/Data/CSV/Enemy/Stage" + std::to_string(m_nowStage + 1) + ".csv");
	enemyController->Init();
	AddObject(enemyController);

	// デバッグウィンドウにオブジェクトコントローラーを渡す
	DebugWindow::Instance().SetTerrainController(objectController);	// Terrain
	DebugWindow::Instance().SetEnemyController(enemyController);	// Enemy

	// 背景
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

	// オブジェクトコントローラーにカメラを渡す
	objectController->SetCamera(tpsCamera);
	enemyController->SetCamera(tpsCamera);

	// enemyControllerにプレイヤーを渡す
	enemyController->SetPlayer(player);

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

	std::shared_ptr<DropGround> ground = std::make_shared<DropGround>();
	ground->Init();
	AddObject(ground);



	// シーンを変える 絶対一番下
	std::shared_ptr<SceneChange> sceneChange = std::make_shared<SceneChange>();
	sceneChange->Init();
	AddObject(sceneChange);
	// 保持
	m_wpSceneChange = sceneChange;
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