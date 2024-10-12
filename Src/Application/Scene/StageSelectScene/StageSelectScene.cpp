#include "StageSelectScene.h"
#include "../SceneManager.h"

#include "../../Tool/DebugWindow/DebugWindow.h"
#include "../../Tool/ObjectController/TerrainController/TerrainController.h"
#include "../../Tool/ObjectController/EventObjectController/EventObjectController.h"

#include "../../GameObject/EventObject/EventObjectBase.h"
#include "../../GameObject/Terrain/TerrainBase.h"

#include "../../GameObject/SceneChange/SceneChange.h"
#include "../../GameObject/Camera/TPSCamera/TPSCamera.h"
#include "../../GameObject/BackGround/BackGround.h"
#include "../../GameObject/Character/Player/Player.h"

void StageSelectScene::Event()
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
		StartScene();
	}
	// 開始し終わって、ステージ初クリアの時の演出
	else
	{
		
	}

	// シーンを変える
	if (m_wpPlayer.lock()->GetBeginGameScene())
	{
		if (!m_wpSceneChange.expired())
		{
			m_wpSceneChange.lock()->EndScene();
		}
	}

	if (!m_wpSceneChange.expired())
	{
		// フェードアウト終了後シーンをゲームシーンに
		if (m_wpSceneChange.lock()->GetFinishFlg())
		{
			SceneManager::Instance().SetNextScene(SceneManager::SceneType::Game);
		}
	}
}

void StageSelectScene::StartScene()
{
	if (!m_wpSceneChange.expired())
	{
		m_wpSceneChange.lock()->StartScene(5);

		if (m_wpSceneChange.lock()->GetFinishFlg())
		{
			// ステージ初クリアの際の処理
			if (SceneManager::Instance().GetFirstClearFlg() == true)
			{
				FirstClearProcess();
			}

			m_sceneStartFlg = true;
			m_wpSceneChange.lock()->Reset();
			// フェードインが終了したらプレイヤーを動けるようにする
			if (!m_wpPlayer.expired())
			{
				m_wpPlayer.lock()->SetStopFlg(false);
			}
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

void StageSelectScene::FirstClearProcess()
{
	if (m_wpTerrainController.expired() == false)
	{
		// 坂を出す演出をする
		// ターゲットの坂を探すための文字列を作成
		std::string string = "Stage" + SceneManager::Instance().GetNowStage();
		for (auto& obj : m_wpTerrainController.lock()->GetObjList())
		{
			if (obj.expired() == false)
			{
				if (obj.lock()->GetParam().targetName == string)
				{
					obj.lock()->Active();
				}
			}
		}
	}
}

void StageSelectScene::Init()
{
	KdShaderManager::Instance().WorkAmbientController().SetDirLight({ 1, -1, 1 }, { 3, 3, 3 });
	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight({ 0.3, 0.3, 0.3, 1.0 });
	KdShaderManager::Instance().WorkAmbientController().SetDirLightShadowArea({ 50,50 }, 50);

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
	std::shared_ptr<TPSCamera> camera = std::make_shared<TPSCamera>();
	// カメラにターゲットをセット
	camera->SetPlayer(player);
	AddObject(camera);
	// カメラの情報を保持する
	m_wpCamera = camera;

	player->SetCamera(camera);

	// オブジェクトコントローラーにカメラを渡す
	//objectController->SetCamera(camera);

	//// ステージの画像
	//std::shared_ptr<StageSelectTexture> stageSelectTexture = std::make_shared<StageSelectTexture>();
	//stageSelectTexture->Init();
	//AddObject(stageSelectTexture);
	//// 保持
	//m_wpStageSelectTexture = stageSelectTexture;

	//// UI
	//std::shared_ptr<StageSelectUI> ui = std::make_shared<StageSelectUI>();
	//ui->Init();
	//AddObject(ui);
	//// 保持
	//m_wpUI = ui;
	//// ステージ数とかもってるクラスをセット
	//ui->SetStageSelectTexture(stageSelectTexture);

	// シーンを変える
	std::shared_ptr<SceneChange> sceneChange = std::make_shared<SceneChange>();
	sceneChange->Init();
	AddObject(sceneChange);
	// 保持
	m_wpSceneChange = sceneChange;

	// EventObjectController
	std::shared_ptr<EventObjectController> eventObjectController = std::make_shared<EventObjectController>();
	eventObjectController->SetCSV("Asset/Data/CSV/EventObject/StageSelect.csv");
	eventObjectController->SetCamera(camera);
	eventObjectController->Init();
	AddObject(eventObjectController);

	// マップエディタ的な
	std::shared_ptr<TerrainController> objectController = std::make_shared<TerrainController>();
	objectController->SetCSV("Asset/Data/CSV/Terrain/StageSelect.csv");
	objectController->Init();
	AddObject(objectController);
	m_wpTerrainController = objectController;

	objectController->SetCamera(camera);


	// プレイヤーにTerrainControllerを渡す
	player->SetTerrainController(objectController);

	// デバッグウィンドウにオブジェクトコントローラーを渡す
	DebugWindow::Instance().SetTerrainController(objectController);	// Terrain
	DebugWindow::Instance().SetEventObjectController(eventObjectController);	// EventObject

	// プレイヤーのスポーン位置を設定する
	for (auto& obj : eventObjectController->GetObjList())
	{
		if (obj.lock()->GetObjectType() == KdGameObject::ObjectType::StageSelectObject)
		{
			if (obj.lock()->GetParam().modelNum == SceneManager::Instance().GetNowStage())
			{
				Math::Vector3 pos = obj.lock()->GetPos();
				pos.z -= 3.0f;
				player->SetPos(pos);
			}
		}
	}

	// プレイヤーの座標が確定してから初期化する
	camera->Init();
}