#include "GameScene.h"
#include "../SceneManager.h"

#include "../../GameObject/Camera/TPSCamera/TPSCamera.h"
#include "../../GameObject/Character/Player/Player.h"
#include "../../GameObject/Terrain/Ground/NormalGround/NormalGround.h"
#include "../../GameObject/Terrain/Ground/BoundGround/BoundGround.h"
#include "../../GameObject/BackGround/BackGround.h"

#include "../../Tool/MousePosBox/MousePosBox.h"
#include "../../Tool/DebugWindow/DebugWindow.h"
#include "../../Tool/ObjectController/ObjectController.h"

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
}

void GameScene::Init()
{
	// マップエディタ的な
	std::shared_ptr<ObjectController> objectController = std::make_shared<ObjectController>();
	objectController->Init();
	AddObject(objectController);

	// デバッグウィンドウにオブジェクトコントローラーを渡す
	DebugWindow::Instance().SetObjectController(objectController);

	// 背景
	std::shared_ptr<BackGround> backGround = std::make_shared<BackGround>();
	backGround->Init();
	//AddObject(backGround);

	// プレイヤー
	std::shared_ptr<Player> player = std::make_shared<Player>();
	player->Init();
	AddObject(player);

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
}