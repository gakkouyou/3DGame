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
		StartScene();
	}
	// 演出しているオブジェクトがある場合
	if (m_wpFirstClearObject.expired() == false)
	{
		// カメラがアニメーションをし終わったら演出
		if (m_wpCamera.expired() == false)
		{
			if (m_wpCamera.lock()->IsFirstClearProcess() == true)
			{
				m_wpFirstClearObject.lock()->Active();
				// 演出が終わった時の処理
				if (m_wpFirstClearObject.lock()->IsActive() == false)
				{
					m_wpCamera.lock()->SetFirstClearFlg(false);

					if (m_wpCamera.lock()->IsFirstClearEndProcess() == true)
					{
						// ウィークポインタリセット
						m_wpFirstClearObject.reset();
						// プレイヤーを動けるようにする
						if (m_wpPlayer.expired() == false)
						{
							m_wpPlayer.lock()->SetStopFlg(false);
						}
						// 初クリア状態をクリア状態に変更
						SceneManager::Instance().WorkStageInfo()[SceneManager::Instance().GetNowStage() - 1] = SceneManager::StageInfo::Clear;
					}
				}
			}
		}
	}

	if (m_wpPlayer.expired() == false)
	{
		// シーンを変える(ゲームシーンへ)
		if (m_wpPlayer.lock()->GetBeginGameScene())
		{
			if (!m_wpSceneChange.expired())
			{
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
			// 違ったらプレイヤーを動けるようにする
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
							// カメラにターゲットをセット
							if (m_wpCamera.expired() == false)
							{
								m_wpCamera.lock()->SetTarget(obj.lock());
								m_wpCamera.lock()->SetFirstClearFlg(true);
							}
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
	}
}

void StageSelectScene::Init()
{
	KdShaderManager::Instance().WorkAmbientController().SetDirLight({ 1, -1, 1 }, { 3, 3, 3 });
	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight({ 0.3, 0.3, 0.3, 1.0 });
	KdShaderManager::Instance().WorkAmbientController().SetDirLightShadowArea({ 50,50 }, 50);

	int nowStage = SceneManager::Instance().GetNowStage();

	// タイトル→ステージセレクトシーンの場合はチェックしない
	if (nowStage != 0)
	{
		// 初クリアかどうか
		if (SceneManager::Instance().GetStageInfo()[SceneManager::Instance().GetNowStage() - 1] == SceneManager::StageInfo::FirstClear)
		{
			m_firstClearFlg = true;
		}
		else
		{
			m_firstClearFlg = false;
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

	// シーンを変える
	std::shared_ptr<SceneChange> sceneChange = std::make_shared<SceneChange>();
	sceneChange->Init();
	AddObject(sceneChange);
	// 保持
	m_wpSceneChange = sceneChange;

	// EventObjectController
	std::shared_ptr<EventObjectController> eventObjectController = std::make_shared<EventObjectController>();
	eventObjectController->SetCSV("Asset/Data/CSV/EventObject/StageSelect");	// CSVセット
	eventObjectController->SetCamera(camera);										// カメラセット
	eventObjectController->SetStageSelectUI(ui);									// UIセット
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
			if (obj.lock()->GetParam().modelNum == SceneManager::Instance().GetNowStage())
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
		if (m_wpEventObjectController.expired() == false)
		{
			int count = 0;
			for (auto& obj : m_wpEventObjectController.lock()->GetObjList())
			{
				if (obj.expired() == false)
				{
					if (obj.lock()->GetObjectType() == KdGameObject::ObjectType::FinalGoal)
					{
						m_wpEventObjectController.lock()->WorkCSVData()[count].modelNum = stageNum;
						EventObjectBase::Param param = obj.lock()->GetParam();
						param.modelNum = stageNum;
						obj.lock()->SetParam(param);
						m_wpEventObjectController.lock()->CSVWriter();
						break;
					}
				}
				count++;
			}
		}
	}

	// 音
	std::shared_ptr<KdSoundInstance> bgm = KdAudioManager::Instance().Play("Asset/Sounds/BGM/stageSelectBGM.wav", true);
	bgm->SetVolume(0.02f);
}