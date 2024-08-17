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

	// プレイヤーが死んだときの処理
	std::shared_ptr<Player> spPlayer = m_wpPlayer.lock();
	if (spPlayer)
	{
		if (spPlayer->GetAlive() == false)
		{
			
		}
	}

}

void GameScene::Init()
{
	// マップエディタ的な
	std::shared_ptr<TerrainController> objectController = std::make_shared<TerrainController>();
	objectController->Init();
	AddObject(objectController);

	// 敵エディタ的な
	std::shared_ptr<EnemyController> enemyController = std::make_shared<EnemyController>();
	enemyController->Init();
	AddObject(enemyController);

	// デバッグウィンドウにオブジェクトコントローラーを渡す
	DebugWindow::Instance().SetTerrainController(objectController);	// Terrain
	DebugWindow::Instance().SetEnemyController(enemyController);	// Enemy

	// シーンを変える
	std::shared_ptr<SceneChange> sceneChange = std::make_shared<SceneChange>();
	sceneChange->Init();
	AddObject(sceneChange);
	sceneChange->EndScene();

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
	goal->Init();
	goal->SetPos({ 0, 0, 10 });
	AddObject(goal);
}