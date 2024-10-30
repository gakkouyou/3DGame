#include "TitleScene.h"
#include "../SceneManager.h"
#include "../../GameObject/SceneChange/SceneChange.h"
#include "Application/main.h"
#include "../../GameObject/Title/House/House.h"
#include "../../GameObject/BackGround/TitleBackGround/TitleBackGround.h"
#include "../../GameObject/Camera/TitleCamera/TitleCamera.h"
#include "../../GameObject/Title/TitlePlayer/TitlePlayer.h"
#include "../../GameObject/UI/TitleUI/TitleUI.h"

void TitleScene::Event()
{
	if (m_wpCamera.expired() == false)
	{
		if (m_wpCamera.lock()->GetAnimationFinishFlg())
		{
			m_wpSceneChange.lock()->EndScene(0, true);
			if (m_wpSceneChange.lock()->GetFinishFlg())
			{
				SceneManager::Instance().SetNextScene(SceneManager::SceneType::StageSelect);
				// 音を止める
				KdAudioManager::Instance().StopAllSound();
			}
		}
	}
}

void TitleScene::Init()
{
	// とりあえずリセット
	SceneManager::Instance().SetNowStage(0);
	std::vector<UINT>& stageInfoList = SceneManager::Instance().WorkStageInfo();
	for (int i = 0; i < stageInfoList.size(); i++)
	{
		stageInfoList[i] = 0;
	}
	SceneManager::Instance().CSVReset();

	// 影の範囲
	KdShaderManager::Instance().WorkAmbientController().SetDirLightShadowArea({ 50, 50 }, 50);

	// 環境光
	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight({ 0.0, 0.0, 0.0, 1.0 });
	// 平行光
	KdShaderManager::Instance().WorkAmbientController().SetDirLight({ 0, -1, 1 }, { 0.7, 0.7, 0.7 });

	// シーンチェンジ
	std::shared_ptr<SceneChange> sceneChange = std::make_shared<SceneChange>();
	sceneChange->Init();
	AddObject(sceneChange);
	// 保持
	m_wpSceneChange = sceneChange;

	// タイトルUI
	std::shared_ptr<TitleUI> ui = std::make_shared<TitleUI>();
	ui->Init();
	AddObject(ui);

	// 家
	std::shared_ptr<House> house = std::make_shared<House>();
	house->Init();
	AddObject(house);

	// 背景
	std::shared_ptr<TitleBackGround> backGround = std::make_shared<TitleBackGround>();
	backGround->Init();
	AddObject(backGround);

	// プレイヤー
	std::shared_ptr<TitlePlayer> player = std::make_shared<TitlePlayer>();
	player->SetBedPos(house->GetBedPos());
	player->Init();
	AddObject(player);

	// TPSカメラ
	std::shared_ptr<TitleCamera> titleCamera = std::make_shared<TitleCamera>();
	// カメラにターゲットをセットする
	titleCamera->SetTarget(player);
	// TPSカメラにターゲットをセットする
	titleCamera->Init();
	AddObject(titleCamera);
	m_wpCamera = titleCamera;

	
	std::shared_ptr<KdSoundInstance> bgm = KdAudioManager::Instance().Play("Asset/Sounds/BGM/nightBGM.wav", true);
	bgm->SetVolume(0.18f);
}