#include "StageSelectScene.h"
#include "../SceneManager.h"

#include "../../Tool/DebugWindow/DebugWindow.h"
#include "../../Tool/ObjectController/TerrainController/TerrainController.h"

#include "../../GameObject/SceneChange/SceneChange.h"
#include "../../GameObject/Camera/StageSelectCamera/StageSelectCamera.h"
#include "../../GameObject/Character/StageSelectPlayer/StageSelectPlayer.h"

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

	if (GetAsyncKeyState(VK_RSHIFT) & 0x8000)
	{
		if (!m_wpSceneChange.expired())
		{
			m_wpSceneChange.lock()->EndScene();
		}
	}

	if (!m_wpSceneChange.expired())
	{
		if (m_wpSceneChange.lock()->GetFinishFlg())
		{
			SceneManager::Instance().SetNextScene
			(
				SceneManager::SceneType::Game
			);
		}
	}
}

void StageSelectScene::Init()
{
	// マップエディタ的な
	std::shared_ptr<TerrainController> objectController = std::make_shared<TerrainController>();
	objectController->SetCSV("Asset/Data/StageSelectTerrain");
	objectController->Init();
	AddObject(objectController);

	// デバッグウィンドウにオブジェクトコントローラーを渡す
	DebugWindow::Instance().SetTerrainController(objectController);	// Terrain

	// シーンを変える
	std::shared_ptr<SceneChange> sceneChange = std::make_shared<SceneChange>();
	sceneChange->Init();
	AddObject(sceneChange);
	// 保持
	m_wpSceneChange = sceneChange;

	// プレイヤー
	std::shared_ptr<StageSelectPlayer> player = std::make_shared<StageSelectPlayer>();
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
	objectController->SetCamera(camera);
}