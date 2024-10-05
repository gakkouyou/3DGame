#include "StageSelectScene.h"
#include "../SceneManager.h"

#include "../../Tool/DebugWindow/DebugWindow.h"
#include "../../Tool/ObjectController/TerrainController/TerrainController.h"
#include "../../Tool/ObjectController/EventObjectController/EventObjectController.h"

#include "../../GameObject/SceneChange/SceneChange.h"
#include "../../GameObject/Camera/StageSelectCamera/StageSelectCamera.h"
#include "../../GameObject/Character/StageSelectPlayer/StageSelectPlayer.h"
#include "../../GameObject/BackGround/BackGround.h"
#include "../../GameObject/Character/Player/Player.h"

#include "../../GameObject/StageSelectTexture/StageSelectTexture.h"
#include "../../GameObject/UI/StageSelectUI/StageSelectUI.h"

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

	// シーンを変える
	if (GetAsyncKeyState('L') & 0x8000)
	{
		if (!m_wpSceneChange.expired())
		{
			m_wpSceneChange.lock()->EndScene();
		}
	}

	if (!m_wpSceneChange.expired())
	{
		// もしフェードアウト、イン中ならステージを変えれないようにする
		if (m_wpSceneChange.lock()->GetNowSceneChange())
		{
			if (!m_wpStageSelectTexture.expired())
			{
				m_wpStageSelectTexture.lock()->SetStopFlg(true);
			}
		}
		else
		{
			if (!m_wpStageSelectTexture.expired())
			{
				m_wpStageSelectTexture.lock()->SetStopFlg(false);
			}
		}

		// フェードアウト終了後シーンをゲームシーンに
		if (m_wpSceneChange.lock()->GetFinishFlg())
		{
			// ステージをセット
			if (!m_wpStageSelectTexture.expired())
			{
				SceneManager::Instance().SetNowStage(m_wpStageSelectTexture.lock()->GetNowStage());
			}
			SceneManager::Instance().SetNextScene(SceneManager::SceneType::Game);
		}
	}
}

void StageSelectScene::Init()
{
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
	std::shared_ptr<StageSelectCamera> camera = std::make_shared<StageSelectCamera>();
	camera->Init();
	AddObject(camera);
	// カメラの情報を保持する
	m_wpCamera = camera;

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
	eventObjectController->Init();
	AddObject(eventObjectController);

	// マップエディタ的な
	std::shared_ptr<TerrainController> objectController = std::make_shared<TerrainController>();
	objectController->SetCSV("Asset/Data/CSV/Terrain/StageSelect.csv");
	objectController->Init();
	AddObject(objectController);

	objectController->SetCamera(camera);

	// デバッグウィンドウにオブジェクトコントローラーを渡す
	DebugWindow::Instance().SetTerrainController(objectController);	// Terrain
	DebugWindow::Instance().SetEventObjectController(eventObjectController);	// EventObject
}