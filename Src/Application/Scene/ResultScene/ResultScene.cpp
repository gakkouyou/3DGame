#include "ResultScene.h"
#include "../SceneManager.h"
#include "../../GameObject/SceneChange/SceneChange.h"
#include "Application/main.h"
#include "../../GameObject/Title/House/House.h"
#include "../../GameObject/BackGround/TitleBackGround/TitleBackGround.h"
#include "../../GameObject/BackGround/ResultBackGround/ResultBackGround.h"
#include "../../GameObject/Camera/ResultCamera/ResultCamera.h"
#include "../../GameObject/Character/ResultPlayer/ResultPlayer.h"
#include "../../GameObject/UI/TitleUI/TitleUI.h"

void ResultScene::Event()
{
	if (m_sceneStartFlg == false)
	{
		if (m_wpSceneChange.expired() == false)
		{
			m_wpSceneChange.lock()->StartScene(0, true, { 1, 1, 1 });

			if (m_wpSceneChange.lock()->GetFinishFlg() == true)
			{
				if (m_wpPlayer.expired() == false)
				{
					m_wpPlayer.lock()->SetFirstAnimationStart();
				}
				m_sceneStartFlg = true;
				m_wpSceneChange.lock()->Reset();
			}
		}
	}

	if (m_wpPlayer.expired() == false)
	{
		if (m_wpPlayer.lock()->GetAnimationEnd() == false)
		{
			// プレイヤーの二つ目のアニメーションが終わった時の処理
			if (m_wpPlayer.lock()->GetSecondAnimationEnd())
			{
				if (m_wpBackGround.expired() == false)
				{
					// 空をオレンジにしていく
					m_wpBackGround.lock()->OrangeAnimation();

					// オレンジになりきったら、プレイヤーの三つ目のアニメーションを始める
					if (m_wpBackGround.lock()->GetOrangeAnimationEnd())
					{
						m_wpPlayer.lock()->SetThirdAnimationStart();
						// からすの鳴き声を鳴らす
						if (m_evening.sound.expired() == false)
						{
							if (m_evening.flg == false)
							{
								m_evening.sound.lock()->Play();
								m_evening.flg = true;
							}
						}
					}
				}
				
				// すずめの鳴き声をフェードアウト
				if (m_morning.sound.expired() == false)
				{
					m_morning.vol -= m_morning.sumVol;
					if (m_morning.vol < 0)
					{
						m_morning.vol = 0;
					}
					m_morning.sound.lock()->SetVolume(m_morning.vol);
				}
			}

			// ドアを開ける
			if (m_wpPlayer.lock()->GetOpenDoor())
			{
				if (m_wpHouse.expired() == false)
				{
					// ドアを開ける
					m_wpHouse.lock()->OpenDoor();
				}
			}

			// ドアを閉める
			if (m_wpPlayer.lock()->GetCloseDoor())
			{
				if (m_wpHouse.expired() == false)
				{
					// ドアを閉める
					m_wpHouse.lock()->CloseDoor();
				}
			}
		}
		// プレイヤーのアニメーションが完全に終了したときの処理
		else
		{
			// 夕方から夜にしていく
			if (m_wpBackGround.expired() == false)
			{
				m_wpBackGround.lock()->BlackAnimation();
			}
			// bgmをフェードアウトさせる
			if (m_bgm.sound.expired() == false)
			{
				m_bgm.vol -= m_bgm.sumVol;
				if (m_bgm.vol < 0)
				{
					m_bgm.vol = 0;
				}
				m_bgm.sound.lock()->SetVolume(m_bgm.vol);
			}
			// 烏の鳴き声をフェードアウトさせる
			if (m_evening.sound.expired() == false)
			{
				m_evening.vol -= m_evening.sumVol;
				if (m_evening.vol < 0)
				{
					m_evening.vol = 0;
				}
				m_evening.sound.lock()->SetVolume(m_evening.vol);
			}
		}
	}
}

void ResultScene::Init()
{
	KdShaderManager::Instance().WorkAmbientController().SetDirLightShadowArea({ 50, 50 }, 50);

	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight({ 0.6f, 0.6f, 0.6f, 1.0f });
	KdShaderManager::Instance().WorkAmbientController().SetDirLight({ 0.0f, -1.0f, 1.0f }, { 0.7f, 0.7f, 0.7f });

	// シーンチェンジ
	std::shared_ptr<SceneChange> sceneChange = std::make_shared<SceneChange>();
	sceneChange->Init();
	AddObject(sceneChange);
	// 保持
	m_wpSceneChange = sceneChange;

	// タイトルUI
	std::shared_ptr<TitleUI> ui = std::make_shared<TitleUI>();
	ui->Init();
	//AddObject(ui);

	// 家
	std::shared_ptr<House> house = std::make_shared<House>();
	house->Init();
	AddObject(house);
	// 保持
	m_wpHouse = house;

	// 背景
	std::shared_ptr<ResultBackGround> backGround = std::make_shared<ResultBackGround>();
	backGround->Init();
	AddObject(backGround);
	m_wpBackGround = backGround;

	// プレイヤー
	std::shared_ptr<ResultPlayer> player = std::make_shared<ResultPlayer>();
	player->Init();
	AddObject(player);
	// 保持
	m_wpPlayer = player;

	// カメラ
	std::shared_ptr<ResultCamera> camera = std::make_shared<ResultCamera>();
	camera->Init();
	AddObject(camera);

	// BGM
	m_bgm.sound = KdAudioManager::Instance().Play("Asset/Sounds/BGM/resultBGM.wav");
	if (m_bgm.sound.expired() == false)
	{
		m_bgm.vol = 0.06f;
		m_bgm.sumVol = m_bgm.vol / 180.0f;
		m_bgm.sound.lock()->SetVolume(m_bgm.vol);
	}
	// すずめの鳴き声
	m_morning.sound = KdAudioManager::Instance().Play("Asset/Sounds/SE/morning.wav");
	if (m_morning.sound.expired() == false)
	{
		m_morning.vol = 0.06f;
		m_morning.sumVol = m_morning.vol / 60.0f;
		m_morning.sound.lock()->SetVolume(m_morning.vol);
	}
	// からすの鳴き声
	m_evening.sound = KdAudioManager::Instance().Play("Asset/Sounds/SE/evening.wav");
	if (m_evening.sound.expired() == false)
	{
		m_evening.vol = 0.06f;
		m_evening.sumVol = m_evening.vol / 60.0f;
		m_evening.sound.lock()->SetVolume(m_evening.vol);
		m_evening.sound.lock()->Stop();
	}
}