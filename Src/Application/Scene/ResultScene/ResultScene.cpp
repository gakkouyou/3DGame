#include "ResultScene.h"
#include "../SceneManager.h"
#include "../../GameObject/SceneChange/SceneChange.h"
#include "Application/main.h"
#include "../../GameObject/Title/House/House.h"
#include "../../GameObject/BackGround/TitleBackGround/TitleBackGround.h"
#include "../../GameObject/BackGround/ResultBackGround/ResultBackGround.h"
#include "../../GameObject/Camera/ResultCamera/ResultCamera.h"
#include "../../GameObject/Title/ResultPlayer/ResultPlayer.h"
#include "../../GameObject/UI/TitleUI/TitleUI.h"

void ResultScene::Event()
{
	// 状態毎の更新
	if (m_nowAction)
	{
		m_nowAction->Update(*this);
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

	ChangeActionState(std::make_shared<SceneStart>());
}

void ResultScene::ChangeActionState(std::shared_ptr<StateBase> _nextState)
{
	if (m_nowAction)m_nowAction->Exit(*this);
	m_nowAction = _nextState;
	m_nowAction->Enter(*this);
}

void ResultScene::SceneStart::Enter(ResultScene& _owner)
{
	// シーンチェンジが存在しないなら終了
	if (_owner.m_wpSceneChange.expired() == true) return;
	// 白画面フェードイン
	_owner.m_wpSceneChange.lock()->StartScene(0, true, { 1, 1, 1 });
}

void ResultScene::SceneStart::Update(ResultScene& _owner)
{
	// シーンチェンジが存在しないなら終了
	if (_owner.m_wpSceneChange.expired() == true) return;
	// プレイヤーが存在しないなら終了
	if (_owner.m_wpPlayer.expired() == true) return;

	// フェードインがまだ終了していないなら終了
	if (_owner.m_wpSceneChange.lock()->GetFinishFlg() == false) return;

	// フェードインが終了したら朝のアニメーションに移行
	_owner.ChangeActionState(std::make_shared<Morning>());
}

void ResultScene::SceneStart::Exit(ResultScene& _owner)
{
	// プレイヤーの朝のアニメーションスタート
	_owner.m_wpPlayer.lock()->MorningAnimationStart();
	// シーンチェンジリセット
	_owner.m_wpSceneChange.lock()->Reset();
}

void ResultScene::Morning::Update(ResultScene& _owner)
{
	// プレイヤーが存在しないなら終了
	if (_owner.m_wpPlayer.expired() == true) return;

	// ドアを開ける
	if (_owner.m_wpPlayer.lock()->GetOpenDoor())
	{
		if (_owner.m_wpHouse.expired() == false)
		{
			// ドアを開ける
			_owner.m_wpHouse.lock()->OpenDoor();
		}
	}

	// ドアを閉める
	if (_owner.m_wpPlayer.lock()->GetCloseDoor())
	{
		if (_owner.m_wpHouse.expired() == false)
		{
			// ドアを閉める
			_owner.m_wpHouse.lock()->CloseDoor();
		}
	}

	// プレイヤーの朝のアニメーションが終了していなければ終了
	if (_owner.m_wpPlayer.lock()->GetMorningAnimationEnd() == false) return;

	// すずめの鳴き声をフェードアウト
	if (_owner.m_morning.sound.expired() == false)
	{
		_owner.m_morning.vol -= _owner.m_morning.sumVol;
		if (_owner.m_morning.vol < 0)
		{
			_owner.m_morning.vol = 0;
		}
		_owner.m_morning.sound.lock()->SetVolume(_owner.m_morning.vol);
	}

	// 背景が存在していなければ終了
	if (_owner.m_wpBackGround.expired() == true) return;
	// 空をオレンジにしていく
	_owner.m_wpBackGround.lock()->OrangeAnimation();

	// オレンジになりきったら夕方へ移行
	if (_owner.m_wpBackGround.lock()->GetOrangeAnimationEnd() == true)
	{
		_owner.ChangeActionState(std::make_shared<Evening>());
	}
}

void ResultScene::Morning::Exit(ResultScene& _owner)
{
	// すずめの声を消す
	_owner.m_morning.sound.lock()->SetVolume(0);
}

void ResultScene::Evening::Enter(ResultScene& _owner)
{
	// プレイヤーの夕方のアニメーションを開始する
	if (_owner.m_wpPlayer.expired() == false)
	{
		_owner.m_wpPlayer.lock()->EveningAnimationStart();
	}
	// からすの鳴き声を鳴らす
	if (_owner.m_evening.sound.expired() == false)
	{
		if (_owner.m_evening.flg == false)
		{
			_owner.m_evening.sound.lock()->Play();
			_owner.m_evening.flg = true;
		}
	}
}

void ResultScene::Evening::Update(ResultScene& _owner)
{
	// プレイヤーが存在しなければ終了
	if (_owner.m_wpPlayer.expired() == true) return;

	// ドアを開ける
	if (_owner.m_wpPlayer.lock()->GetOpenDoor())
	{
		if (_owner.m_wpHouse.expired() == false)
		{
			// ドアを開ける
			_owner.m_wpHouse.lock()->OpenDoor();
		}
	}

	// ドアを閉める
	if (_owner.m_wpPlayer.lock()->GetCloseDoor())
	{
		if (_owner.m_wpHouse.expired() == false)
		{
			// ドアを閉める
			_owner.m_wpHouse.lock()->CloseDoor();
		}
	}

	// プレイヤーの夕方のアニメーションが終了したら夜に移行
	if (_owner.m_wpPlayer.lock()->GetEveningAnimationEnd() == false) return;
	_owner.ChangeActionState(std::make_shared<Night>());
}

void ResultScene::Night::Enter(ResultScene& _owner)
{
	if (_owner.m_wpBackGround.expired() == true) return;
	// 夕方から夜にしていく
	_owner.m_wpBackGround.lock()->BlackAnimation();
}

void ResultScene::Night::Update(ResultScene& _owner)
{
	// bgmをフェードアウトさせる
	if (_owner.m_bgm.sound.expired() == false)
	{
		_owner.m_bgm.vol -= _owner.m_bgm.sumVol;
		if (_owner.m_bgm.vol < 0)
		{
			_owner.m_bgm.vol = 0;
		}
		_owner.m_bgm.sound.lock()->SetVolume(_owner.m_bgm.vol);
	}
	// 烏の鳴き声をフェードアウトさせる
	if (_owner.m_evening.sound.expired() == false)
	{
		_owner.m_evening.vol -= _owner.m_evening.sumVol;
		if (_owner.m_evening.vol < 0)
		{
			_owner.m_evening.vol = 0;
		}
		_owner.m_evening.sound.lock()->SetVolume(_owner.m_evening.vol);
	}
}
