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
			m_wpSceneChange.lock()->StartScene();

			if (m_wpSceneChange.lock()->GetFinishFlg())
			{
				m_sceneStartFlg = true;
				m_wpSceneChange.lock()->Reset();
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
				if (!m_wpSceneChange.expired())
				{
					// シーン遷移を終えたらリセット
					if (m_wpSceneChange.lock()->GetFinishFlg())
					{
						for (auto& obj : m_objList)
						{
							if (obj->GetBaseObjectType() == KdGameObject::BaseObjectType::Enemy || obj->GetBaseObjectType() == KdGameObject::BaseObjectType::Ground)
							{
								obj->SetExpired(true);
							}

							obj->Reset();
							m_resetFlg = true;
						}
					}
					else
					{
						m_wpSceneChange.lock()->EndScene();
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
			}
			else
			{
				m_wpSceneChange.lock()->StartScene();
			}
		}
	}

	// ゴール処理
	if(!m_wpGoal.expired())
	{
		if (m_wpGoal.lock()->GetGoalFlg())
		{
			if (!m_wpCamera.expired())
			{
				m_wpCamera.lock()->SetGoalFlg(true);

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
	// マップエディタ的な
	std::shared_ptr<TerrainController> objectController = std::make_shared<TerrainController>();
	// CSVファイルを指定する
	objectController->SetCSV("Asset/Data/CSV/Terrain.csv");
	objectController->Init();
	AddObject(objectController);

	// 敵エディタ的な
	std::shared_ptr<EnemyController> enemyController = std::make_shared<EnemyController>();
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



	// シーンを変える 絶対一番下
	std::shared_ptr<SceneChange> sceneChange = std::make_shared<SceneChange>();
	sceneChange->Init();
	AddObject(sceneChange);
	// 保持
	m_wpSceneChange = sceneChange;
}