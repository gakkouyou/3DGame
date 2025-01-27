﻿#include "StageSelectScene.h"
#include "../SceneManager.h"

#include "../../Tool/DebugWindow/DebugWindow.h"
#include "../../Tool/ObjectController/TerrainController/TerrainController.h"
#include "../../Tool/ObjectController/EventObjectController/EventObjectController.h"
#include "../../Tool/MouseClickHit/MouseClickHit.h"

#include "../../GameObject/EventObject/EventObjectBase.h"
#include "../../GameObject/Terrain/TerrainBase.h"

#include "../../GameObject/EventObject/FinalGoal/FinalGoal.h"

#include "../../GameObject/SceneChange/SceneChange.h"
#include "../../GameObject/Camera/TPSCamera/TPSCamera.h"
#include "../../GameObject/BackGround/BackGround.h"
#include "../../GameObject/Character/Player/Player.h"
#include "../../GameObject/UI/StageSelectUI/StageSelectUI.h"
#include "../../GameObject/UI/Tutorial/Tutorial.h"

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
	// 最終ゴールの演出
	if (m_finalGoalEffectFlg == true)
	{
		FinalGoalEffectProcess();
	}
	// 坂の演出
	if (m_slopeEffectFlg == true)
	{
		SlopeEffectProcess();
	}

	// カメラが元に戻ったらプレイヤーを動けるようにする
	if (m_wpFirstClearObject.expired() == false)
	{
		if (m_wpCamera.expired() == false)
		{
			if (m_wpCamera.lock()->GetFirstClearProcessEnd() == true)
			{
				// ウィークポインタリセット
				m_wpFirstClearObject.reset();
				// プレイヤーを動けるようにする
				if (m_wpPlayer.expired() == false)
				{
					m_wpPlayer.lock()->SetStopFlg(false);
				}
			}
		}
	}

	if (m_wpPlayer.expired() == false)
	{
		// ゲームシーンへ移行(プレイヤーから取得)
		if (m_wpPlayer.lock()->GetBeginGameScene())
		{
			if (!m_wpSceneChange.expired())
			{
				// フェード後
				m_wpSceneChange.lock()->EndScene();

				// フェードアウト終了後シーンをゲームシーンに
				if (m_wpSceneChange.lock()->GetFinishFlg())
				{
					SceneManager::Instance().SetNextScene(SceneManager::SceneType::Game);
					// 全ての音を止める
					KdAudioManager::Instance().StopAllSound();
				}
			}
		}

		// シーンを変える(最終ゴール後)
		if (m_wpPlayer.lock()->GetGoalFlg())
		{
			if (!m_wpSceneChange.expired())
			{
				// 白フェードアウト
				m_wpSceneChange.lock()->EndScene(0, true, { 1, 1, 1 });

				// フェードアウト終了後シーンをタイトルシーンに
				if (m_wpSceneChange.lock()->GetFinishFlg())
				{
					SceneManager::Instance().SetNextScene(SceneManager::SceneType::Result);
					// 全ての音を止める
					KdAudioManager::Instance().StopAllSound();
				}
			}
		}
	}
}

void StageSelectScene::Init()
{
	KdShaderManager::Instance().WorkAmbientController().SetDirLight({ 1, -1, 1 }, { 3, 3, 3 });
	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight({ 0.3f, 0.3f, 0.3f, 1.0f });
	KdShaderManager::Instance().WorkAmbientController().SetDirLightShadowArea({ 50,50 }, 50);

	int nowStage = SceneManager::Instance().GetNowStage();

	// タイトル→ステージセレクトシーンの場合はチェックしない
	if (nowStage != 0)
	{
		// 初クリアかどうか
		if (SceneManager::Instance().GetStageInfo()[nowStage - 1] == SceneManager::StageInfo::FirstClear)
		{
			m_firstClearFlg = true;		// 初クリア
		}
		else
		{
			m_firstClearFlg = false;	// 初クリアでない
		}
	}

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

	// UI
	std::shared_ptr<StageSelectUI> ui = std::make_shared<StageSelectUI>();
	ui->Init();
	AddObject(ui);

	// 操作説明
	std::shared_ptr<Tutorial> tutorial = std::make_shared<Tutorial>();
	tutorial->Init();
	AddObject(tutorial);
	// プレイヤーにセット
	player->SetTutorial(tutorial);

	// シーンを変える
	std::shared_ptr<SceneChange> sceneChange = std::make_shared<SceneChange>();
	sceneChange->Init();
	AddObject(sceneChange);
	// 保持
	m_wpSceneChange = sceneChange;

	// EventObjectController
	std::shared_ptr<EventObjectController> eventObjectController = std::make_shared<EventObjectController>();
	eventObjectController->SetStageSelectScene(this);
	eventObjectController->SetCSV("Asset/Data/CSV/EventObject/StageSelect");	// CSVセット
	eventObjectController->SetCamera(camera);									// カメラセット
	eventObjectController->SetStageSelectUI(ui);								// UIセット
	eventObjectController->Init();
	AddObject(eventObjectController);
	// 保持
	m_wpEventObjectController = eventObjectController;

	// マップエディタ的な
	std::shared_ptr<TerrainController> objectController = std::make_shared<TerrainController>();
	objectController->SetCSV("Asset/Data/CSV/Terrain/StageSelect");
	objectController->Init();
	AddObject(objectController);
	m_wpTerrainController = objectController;

	objectController->SetCamera(camera);


	// プレイヤーにTerrainControllerを渡す
	player->SetTerrainController(objectController);
	player->SetEventObjectController(eventObjectController);

	// デバッグウィンドウにオブジェクトコントローラーを渡す
	DebugWindow::Instance().SetTerrainController(objectController);	// Terrain
	DebugWindow::Instance().SetEventObjectController(eventObjectController);	// EventObject

	// プレイヤーのスポーン位置を設定する
	for (auto& obj : eventObjectController->GetObjList())
	{
		if (obj.lock()->GetObjectType() == KdGameObject::ObjectType::StageSelectObject)
		{
			if (obj.lock()->GetParam().modelNum == nowStage)
			{
				Math::Vector3 pos = obj.lock()->GetPos();
				pos.z -= 4.0f;
				player->SetPos(pos);
			}
		}
	}

	// プレイヤーの座標が確定してから初期化する
	camera->Init();

	// 初クリアなら最終ゴールのモデルを変更
	if (m_firstClearFlg == true)
	{
		int stageNum = SceneManager::Instance().GetNowStage();
		int count = 0;
		for (auto& obj : eventObjectController->GetObjList())
		{
			if (obj.expired() == false)
			{
				if (obj.lock()->GetObjectType() == KdGameObject::ObjectType::FinalGoal)
				{
					// CSVに書き込む用のデータを書き換える
					eventObjectController->WorkCSVData()[count].modelNum = stageNum;
					EventObjectBase::Param param = obj.lock()->GetParam();
					param.modelNum = stageNum;
					// モデルを変更
					obj.lock()->SetParam(param);
					// CSVに書き込む
					eventObjectController->CSVWriter();
					break;
				}
			}
			count++;
		}
	}

	// 最終ゴールに初クリアかどうかを伝える
	if (m_wpFinalGoal.expired() == false)
	{
		m_wpFinalGoal.lock()->SetFirstClearFlg(m_firstClearFlg);
	}

	// ツール用マウスクリックで当たり判定をするクラス
	std::shared_ptr<MouseClickHit> mouseClickHit = std::make_shared<MouseClickHit>();
	AddObject(mouseClickHit);
	mouseClickHit->SetTerrainController(objectController);
	mouseClickHit->SetEventController(eventObjectController);
	mouseClickHit->SetCamera(camera);

	// 音
	std::shared_ptr<KdSoundInstance> bgm = KdAudioManager::Instance().Play("Asset/Sounds/BGM/stageSelectBGM.wav", true);
	bgm->SetVolume(0.06f);
}

// シーン開始時の処理
void StageSelectScene::StartScene()
{
	if (!m_wpSceneChange.expired())
	{
		m_wpSceneChange.lock()->StartScene(5);

		if (m_wpSceneChange.lock()->GetFinishFlg())
		{
			// ステージ初クリアの際の処理
			if (m_firstClearFlg)
			{
				FirstClearProcess();

				// もし初クリアだが演出がない場合はプレイヤーを動けるようにする
				if (m_wpFirstClearObject.expired() == true)
				{
					// プレイヤーを動けるようにする
					if (m_wpPlayer.expired() == false)
					{
						m_wpPlayer.lock()->SetStopFlg(false);
					}
				}
			}
			// 初クリアでなかったらプレイヤーを動けるようにする
			else
			{
				if (m_wpPlayer.expired() == false)
				{
					m_wpPlayer.lock()->SetStopFlg(false);
				}
			}
			m_sceneStartFlg = true;
			m_wpSceneChange.lock()->Reset();
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

// 初クリアしたときの処理
void StageSelectScene::FirstClearProcess()
{
	// オブジェクトをまだ見つけていない時のみ実行
	if (m_wpFirstClearObject.expired() == true)
	{
		if (m_wpTerrainController.expired() == false)
		{
			// 坂を出す演出をする
			// ターゲットの坂を探すための文字列を作成
			std::string string = "Stage" + std::to_string(SceneManager::Instance().GetNowStage());
			int count = 0;
			for (auto& obj : m_wpTerrainController.lock()->GetObjList())
			{
				if (obj.expired() == false)
				{
					if (obj.lock()->GetParam().targetName == string)
					{
						// 坂なら保持する
						if (obj.lock()->GetObjectType() == KdGameObject::ObjectType::SlopeGround)
						{
							// 保持
							m_wpFirstClearObject = obj;
						}
						// 坂以外
						else
						{
							// アクティブにする
							obj.lock()->Active();
						}

						// CSVの中身を書き換える
						// 坂がもう出た状態に書き換える
						std::vector<TerrainController::Data>& dataList = m_wpTerrainController.lock()->WorkCSVData();
						dataList[count].yetActive = 1;
						m_wpTerrainController.lock()->CSVWriter();
					}
				}
				count++;
			}
		}
		// カメラのアニメーション開始
		m_finalGoalEffectFlg = true;
		if (m_wpCamera.expired() == false)
		{
			// ターゲット設定
			m_wpCamera.lock()->SetTarget(m_wpFinalGoal.lock());
			m_wpCamera.lock()->SetFinalGoalProcess();
		}
		// 初クリア状態をクリア状態に変更
		SceneManager::Instance().WorkStageInfo()[SceneManager::Instance().GetNowStage() - 1] = SceneManager::StageInfo::Clear;
	}
}

void StageSelectScene::SlopeEffectProcess()
{
	std::shared_ptr<TerrainBase> spSlope = m_wpFirstClearObject.lock();
	// 坂がないなら終了
	if (!spSlope) return;

	std::shared_ptr<TPSCamera> spCamera = m_wpCamera.lock();
	// カメラがないなら終了
	if (!spCamera) return;

	// まだ坂に向かっている途中なら終了
	if (spCamera->GetSlopeProcessEnd() == false) return;

	// カメラが坂に到着したら最終ゴールの演出をする
	spSlope->Active();

	// 終了したらカメラを元に戻す
	if (spSlope->IsActive() == false)
	{
		m_slopeEffectFlg = false;

		spCamera->SetBackProcess();
	}
}

void StageSelectScene::FinalGoalEffectProcess()
{
	std::shared_ptr<FinalGoal> spFinalGoal = m_wpFinalGoal.lock();
	// 最終ゴールがないなら終了
	if (!spFinalGoal) return;

	std::shared_ptr<TPSCamera> spCamera = m_wpCamera.lock();
	// カメラがないなら終了
	if (!spCamera) return;

	// まだ最終ゴールに向かっている途中なら終了
	if (spCamera->GetFinalGoalProcessEnd() == false) return;

	// カメラが最終ゴールに到着したら最終ゴールの演出を始める
	spFinalGoal->FirstClearProcess();

	// 終了したら坂の演出を開始する
	if (spFinalGoal->IsFirstClearProcess() == true)
	{
		m_finalGoalEffectFlg = false;
		m_slopeEffectFlg = true;

		// ターゲット変更
		spCamera->SetTarget(m_wpFirstClearObject.lock());
		spCamera->SetSlopeProcess();
	}
}