#include "TitleScene.h"
#include "../SceneManager.h"
#include "../../GameObject/SceneChange/SceneChange.h"
#include "Application/main.h"
#include "../../GameObject/Title/House/House.h"
#include "../../GameObject/BackGround/TitleBackGround/TitleBackGround.h"
#include "../../GameObject/Camera/TitleCamera/TitleCamera.h"
#include "../../GameObject/Title/TitlePlayer/TitlePlayer.h"

void TitleScene::Event()
{
	//if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	//{
	//	if (!m_wpSceneChange.expired())
	//	{
	//		m_wpSceneChange.lock()->EndScene();
	//	}
	//}

	//if (!m_wpSceneChange.expired())
	//{
	//	if (m_wpSceneChange.lock()->GetFinishFlg())
	//	{
	//		SceneManager::Instance().SetNextScene
	//		(
	//			SceneManager::SceneType::StageSelect
	//		);
	//	}
	//}

	
}

void TitleScene::Init()
{
	KdShaderManager::Instance().WorkAmbientController().SetDirLightShadowArea({ 50, 50 }, 50);

	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight({ 0.0, 0.0, 0.0, 1.0 });
	KdShaderManager::Instance().WorkAmbientController().SetDirLight({ 0, -1, 1 }, { 0.7, 0.7, 0.7 });

	// シーンチェンジ
	std::shared_ptr<SceneChange> sceneChange = std::make_shared<SceneChange>();
	sceneChange->Init();
	AddObject(sceneChange);
	// 保持
	m_wpSceneChange = sceneChange;

	// 家
	std::shared_ptr<House> house = std::make_shared<House>();
	house->Init();
	AddObject(house);

	// 背景
	std::shared_ptr<TitleBackGround> backGround = std::make_shared<TitleBackGround>();
	backGround->Init();
	AddObject(backGround);

	// TPSカメラ
	std::shared_ptr<TitleCamera> titleCamera = std::make_shared<TitleCamera>();
	// TPSカメラにターゲットをセットする
	//tpsCamera->SetTarget(house);
	titleCamera->Init();
	AddObject(titleCamera);
	// カメラの情報を保持する
	//m_wpCamera = tpsCamera;

	// プレイヤー
	std::shared_ptr<TitlePlayer> player = std::make_shared<TitlePlayer>();
	player->SetBedPos(house->GetBedPos());
	player->Init();
	AddObject(player);
}