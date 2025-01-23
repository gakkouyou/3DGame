#include "Player.h"
#include "../../../Scene/SceneManager.h"
#include "../../Camera/CameraBase.h"

#include "../../Effect/Smoke/Smoke.h"
#include "../../UI/GameUI/GameUI.h"
#include "../../UI/Tutorial/Tutorial.h"

#include "../../Terrain/TerrainBase.h"
#include "../../../Tool/ObjectController/TerrainController/TerrainController.h"
#include "../../../Tool/ObjectController/CarryObjectController/CarryObjectController.h"
#include "../../Terrain/CarryObject/CarryObjectBase.h"
#include "../../../Tool/ObjectController/EventObjectController/EventObjectController.h"
#include "../../EventObject/EventObjectBase.h"

#include "../../Terrain/MoveObjectRideProcess/MoveObjectRideProcess.h"

#include "../../../main.h"

void Player::Update()
{
	// デバッグモード中は更新しない
	if (SceneManager::Instance().GetDebug()) return;
	// ポーズ画面中は更新しない
	if (m_pauseFlg == true)
	{
		// ポーズから戻ってきたときにアクションをしてしまう事を防止
		m_actionKeyFlg = true;
		return;
	}

	// 動く床に乗っていた時の処理
	if (m_moveGround.hitFlg == true)
	{
		// 一緒に動く
		if (m_wpHitTerrain.expired() == false)
		{
			m_pos = MoveObjectRideProcess::Instance().MoveGroundRide(m_mWorld, m_moveGround.transMat, m_wpHitTerrain.lock()->GetPos());
		}
	}

	// 運べるオブジェクトに乗っている時の処理(運べるオブジェクトが地面に触れている場合)
	if (m_carryObjectHitTerrain.hitFlg)
	{
		std::shared_ptr<TerrainBase> spHitTerrain = m_wpHitTerrain.lock();
		if (spHitTerrain)
		{
			switch (spHitTerrain->GetObjectType())
			{
				// 動く床の場合
				// 動く床に乗っている運べるオブジェクトに乗っている時に、一緒に動く
			case ObjectType::MoveGround:
			{
				m_pos = MoveObjectRideProcess::Instance().MoveGroundRide(m_mWorld, m_carryObjectHitTerrain.transMat, spHitTerrain->GetPos());
			}
			}
		}
	}

	if (m_situationType & SituationType::CarryAnimation)
	{
		m_carryAnimationCount++;
		if (m_carryAnimationCount > m_carryAnimationTime)
		{
			m_situationType &= (~SituationType::CarryAnimation);
			m_situationType |= SituationType::Carry;
			m_carryAnimationCount = 0;
			m_stopFlg = false;
		}
	}

	// 死んでいたら操作を受け付けない
	if (m_aliveFlg == false)
	{
		SetStopFlg(true);
	}

	// 無限ジャンプ
	if (GetAsyncKeyState('M') & 0x8000)
	{
		if (m_mugenJumpKeyFlg == false)
		{
			m_mugenJumpFlg = !m_mugenJumpFlg;
			m_mugenJumpKeyFlg = true;
		}
	}
	else
	{
		m_mugenJumpKeyFlg = false;
	}

	// 移動前の座標を保存
	m_oldPos = m_pos;
	// 更新前の状況を保存
	m_oldSituationType = m_situationType;

	// Idle以外のフラグが立っていたらIdleをおろす
	if (m_situationType ^ SituationType::Idle)
	{
		m_situationType &= (~SituationType::Idle);
	}
	// 何もフラグが立っていなかったらIdleにする
	if (m_situationType == 0)
	{
		m_situationType = SituationType::Idle;
	}
	// 入力前の移動ベクトルを保持
	Math::Vector3 oldMoveVec = m_moveVec;
	// 移動ベクトル初期化
	m_moveVec = Math::Vector3::Zero;

	if (m_stopFlg == false)
	{
		// ステージセレクトシーンなら操作ができる状態の時は常に操作説明を表示する
		if (SceneManager::Instance().GetNowScene() == SceneManager::SceneType::StageSelect)
		{
			if (m_wpTutorial.expired() == false)
			{
				m_wpTutorial.lock()->Display();
			}
		}

		// 動ける状態かつ 止まっている or 箱を持って止まっている状態なら操作説明を表示させる状態にする
		if((m_situationType ^ SituationType::Idle) == 0 || (m_situationType ^ SituationType::Carry) == 0)
		{
			if (m_wpTutorial.expired() == false)
			{
				m_wpTutorial.lock()->Display();
			}
		}

		// WASDで移動
		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			m_moveVec += Math::Vector3::Backward;
			// 運んでいる状態なら歩く
			if (m_situationType & SituationType::Carry)
			{
				m_situationType |= SituationType::Walk;
			}
			// 違うなら走る
			else
			{
				m_situationType |= SituationType::Run;
				m_situationType &= (~SituationType::Walk);
			}
		}
		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			m_moveVec += Math::Vector3::Left;
			// 運んでいる状態なら歩く
			if (m_situationType & SituationType::Carry)
			{
				m_situationType |= SituationType::Walk;
			}
			// 違うなら走る
			else
			{
				m_situationType |= SituationType::Run;
				m_situationType &= (~SituationType::Walk);
			}
		}
		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			m_moveVec += Math::Vector3::Forward;
			// 運んでいる状態なら歩く
			if (m_situationType & SituationType::Carry)
			{
				m_situationType |= SituationType::Walk;
			}
			// 違うなら走る
			else
			{
				m_situationType |= SituationType::Run;
				m_situationType &= (~SituationType::Walk);
			}
		}
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			m_moveVec += Math::Vector3::Right;
			// 運んでいる状態なら歩く
			if (m_situationType & SituationType::Carry)
			{
				m_situationType |= SituationType::Walk;
			}
			// 違うなら走る
			else
			{
				m_situationType |= SituationType::Run;
				m_situationType &= (~SituationType::Walk);
			}
		}

		// SHIFTキーを押すと、歩きになる
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			// 空中でないなら走り状態に移行できる
			if ((m_situationType & SituationType::Air) == 0)
			{
				m_situationType |= SituationType::Walk;
			}
			// 歩き状態なら走り状態を解除する
			if (m_situationType & SituationType::Walk)
			{
				m_situationType &= (~SituationType::Run);
			}
		}
	}

	// カメラの向きで移動方向を補正
	m_moveVec = m_moveVec.TransformNormal(m_moveVec, m_wpCamera.lock()->GetRotationYMatrix());
	m_moveVec.Normalize();

	// 動いていなかったら止まっている状態にする。
	if (m_moveVec.LengthSquared() == 0)
	{
		m_situationType &= (~SituationType::Walk);
		m_situationType &= (~SituationType::Run);

		// 音をなっていない状態にする
		//m_nowWalkSoundFlg = false;
	}
	// 動いていてかつ空中なら、空中に出る前の移動状態にする
	else if (m_situationType & SituationType::Air)
	{
		// 歩き状態の時
		if (m_lastGroundSituationType & SituationType::Walk)
		{
			m_situationType |= SituationType::Walk;
			m_situationType &= (~SituationType::Run);
		}
	}

	// 空中にいなくて動いている時、足元に煙を出す
	if ((m_situationType & SituationType::Air) == 0)
	{
		// 歩いている時
		if (m_situationType & SituationType::Walk)
		{
			// 一定時間置きに煙を出す
			if (m_smokeCount % m_walkSmokeTime == 0)
			{
				std::shared_ptr<Smoke> smoke = std::make_shared<Smoke>();
				smoke->Init();
				smoke->SetPos(m_pos);
				smoke->SetSmokeType(Smoke::SmokeType::WalkSmoke);
				SceneManager::Instance().AddObject(smoke);
				// 煙を出すカウントをリセット
				m_smokeCount = 0;
				m_nowWalkSEFlg = false;
			}
			// 煙を出すカウント
			m_smokeCount++;
		}
		// 走っている時
		else if (m_situationType & SituationType::Run)
		{
			// 一定時間置きに煙を出す
			if (m_smokeCount % m_runSmokeTime == 0)
			{
				// 煙を生み出す
				std::shared_ptr<Smoke> smoke = std::make_shared<Smoke>();
				smoke->Init();
				smoke->SetPos(m_pos);
				smoke->SetSmokeType(Smoke::SmokeType::WalkSmoke);
				SceneManager::Instance().AddObject(smoke);
				// 煙を出すカウントをリセット
				m_smokeCount = 0;
				// 歩く音をもう一度鳴らせるようにする
				m_nowWalkSEFlg = false;
			}
			// 煙を出すカウント
			m_smokeCount++;
		}
		// 移動していなかったらカウントをリセット
		else
		{
			m_smokeCount = 0;
		}
	}
	// 空中にいるならカウントをリセット
	else
	{
		m_smokeCount = 0;
	}

	if (m_stopFlg == false)
	{
		// ジャンプ
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			// 運んでいたらジャンプ出来ないようにする
			if ((m_situationType & SituationType::Carry) == 0)
			{
				// 空中じゃなければジャンプする
				if (!(m_situationType & SituationType::Air) || m_mugenJumpFlg == true)
				{
					m_situationType |= SituationType::Jump;
					m_gravity = -m_jumpPow;

					// ジャンプ音を鳴らす
					if (m_jumpSE.flg == false)
					{
						if (!m_jumpSE.wpSound.expired())
						{
							m_jumpSE.wpSound.lock()->Play();
							m_jumpSE.flg = true;
						}
					}
				}
			}
		}
	}
	// 移動
	// 歩き
	if (m_situationType & SituationType::Walk)
	{
		m_pos += m_moveVec * m_walkSpeed;
	}
		// 走り
	else if (m_situationType & SituationType::Run)
	{
		// 高速ダッシュ
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		{
			m_pos += m_moveVec * 1.0f;
		}
		m_pos += m_moveVec * m_runSpeed;
	}

	if (m_goalJumpFlg == false)
	{
		// 重力
		m_gravity += m_gravityPow;
		m_pos.y -= m_gravity;
		// 重力の制限
		if (m_gravity >= m_maxGravity)
		{
			m_gravity = m_maxGravity;
		}
	}

	// 落下死
	if (m_pos.y < m_underLine && m_aliveFlg == true)
	{
		// 万が一ステージセレクト画面で落ちたら、座標を初期座標に戻す
		if (SceneManager::Instance().GetNowScene() == SceneManager::SceneType::StageSelect)
		{
			m_pos = m_stageStartPos;
		}
		else
		{
			m_aliveFlg = false;

			std::shared_ptr<KdSoundInstance> dropSE = KdAudioManager::Instance().Play("Asset/Sounds/SE/drop.wav");
			dropSE->SetVolume(0.06f);
		}
	}

	// 移動中
	// 回転処理
	if (m_situationType & SituationType::Walk || m_situationType & SituationType::Run)
	{
		// 回転
		RotationCharacter(m_angle, m_moveVec, m_maxAngle);

		// 回転行列
		m_rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));
	}

	if (m_stopFlg == false)
	{
		if (GetAsyncKeyState('Z') & 0x8000)
		{
			if (m_actionKeyFlg == false)
			{
				// 運び状態を解除する
				if (m_situationType & SituationType::Carry)
				{
					if (m_wpCarryObject.expired() == false)
					{
						// 運ばれていない状態にする
						m_wpCarryObject.lock()->CarryFlg(false);
						m_wpCarryObject.reset();
					}
					// 状態の切り替え
					m_situationType &= (~SituationType::Carry);
					// 歩き状態の解除
					m_situationType &= (~SituationType::Walk);
					// キーフラグ
					m_actionKeyFlg = true;
					// 音を鳴らす
					if (m_letGoSE.wpSound.expired() == false)
					{
						m_letGoSE.wpSound.lock()->Play();
					}
				}
				// 運んでいなくて、持てる状態でないなら空ぶる
				else
				{
					// 箱を持てない状態、かつ空中ではない、かつステージに入れない状態の時、空ぶる
					if (m_holdFlg == false && !(m_situationType & SituationType::Air) && m_stageSelectObjectFlg == false)
					{
						m_stopFlg = true;
						m_missingShotFlg = true;
						SetAnimation("Carry", true);
						// キーフラグ
						m_actionKeyFlg = true;
					}
				}
			}
		}
		else
		{
			m_actionKeyFlg = false;
		}
	}

	if (m_missingShotFlg == true)
	{
		m_missingShotCount++;
		if (m_missingShotCount > m_missingShotTime)
		{
			m_missingShotCount = 0;
			m_stopFlg = false;
			m_missingShotFlg = false;
			SetAnimation("Idle", true);
		}
	}

	// ゴールの処理
	if (m_goalFlg)
	{
		GoalProcess();
	}

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);

	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_scale);

	m_mWorld = scaleMat * m_rotMat * transMat;
}

void Player::PostUpdate()
{
	// ポーズ画面中は更新しない
	if (m_pauseFlg == true) return;

	// 当たり判定
	HitJudge();

	// 空中になった瞬間の状態を保持
	if (m_situationType & SituationType::Air)
	{
		if ((m_oldSituationType & SituationType::Air) == 0)
		{
			m_lastGroundSituationType = m_situationType;
		}
	}

	// 着地した瞬間
	if ((m_situationType & SituationType::Air) == 0)
	{
		if (m_oldSituationType & SituationType::Air)
		{
			// ジャンプ音のフラグをfalseに
			m_jumpSE.flg = false;
			m_nowWalkSEFlg = false;
		}
	}

	// アニメーションを切り替える
	if (m_oldSituationType != m_situationType)
	{
		ChangeAnimation();
	}

	// 何の地面に乗っているかによって、音を変える
	// 運べるオブジェクトの場合の音
	if (m_carryObject.hitFlg == true)
	{
		m_walkSEType = WalkSEType::Tile;
	}
	else if (!m_wpHitTerrain.expired())
	{
		ObjectType type = m_wpHitTerrain.lock()->GetObjectType();
		switch (type)
		{
			// 草の音
		case ObjectType::NormalGround:
		case ObjectType::SlopeGround:
		case ObjectType::RotationGround:
			m_walkSEType = WalkSEType::Grass;
			break;

		case ObjectType::BoundGround:
			break;

			// コツコツ見たいな音
		default:
			m_walkSEType = WalkSEType::Tile;
			break;
		}
	}

	// 空中にいなかったら歩く音を鳴らすようにする
	if (((m_situationType & Air) == 0))
	{
		if (m_nowWalkSEFlg == false)
		{
			// シーン開始後の着地音防止
			if (m_firstLandFlg == false)
			{
				m_firstLandFlg = true;
				m_nowWalkSEFlg = true;
			}
			else
			{
				if (m_wpWalkSE[m_walkSEType].expired() == false)
				{
					m_wpWalkSE[m_walkSEType].lock()->Play();
					m_nowWalkSEFlg = true;
				}
			}
		}
	}
	// 止める
	else
	{
		if (m_wpWalkSE[m_walkSEType].expired() == false)
		{
			m_wpWalkSE[m_walkSEType].lock()->Stop();
		}
	}

	Math::Matrix transMat = Math::Matrix::Identity;

	// 回る床に当たっていた時の処理
	if (m_rotationGround.hitFlg)
	{
		// 当たった地面
		std::shared_ptr<TerrainBase> spHitTerrain = m_wpHitTerrain.lock();
		// 無かったら終了
		if (!spHitTerrain) return;
		Math::Vector3 terrainPos = spHitTerrain->GetPos();
		// 回転床が回転する角度
		Math::Vector3 rotDegAng = spHitTerrain->GetParam().degAng;

		// Z軸回転の場合
		if (rotDegAng.z != 0)
		{
			// 回転床に乗った時の処理
			MoveObjectRideProcess::Instance().RotationGroundRide(m_pos, terrainPos, rotDegAng);
		}
		// Y軸回転の場合
		else if (rotDegAng.y != 0)
		{
			// プロペラに乗った時の処理
			MoveObjectRideProcess::Instance().PropellerRide(m_pos, terrainPos, rotDegAng, m_angle);

			// 回転行列
			m_rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));
		}
	}

	// 運べるオブジェクトに乗っている時の処理(地面に触れている場合)
	if (m_carryObjectHitTerrain.hitFlg)
	{
		std::shared_ptr<TerrainBase> spHitTerrain = m_wpHitTerrain.lock();
		if (spHitTerrain)
		{
			switch (spHitTerrain->GetObjectType())
			{
				// 回る床の場合(Z軸)
				case ObjectType::RotationGround:
				{
					Math::Vector3 terrainPos = spHitTerrain->GetPos();
					// 回転床が回転する角度
					Math::Vector3 rotDegAng = spHitTerrain->GetParam().degAng;

					// 回転床に乗った時の処理
					MoveObjectRideProcess::Instance().RotationGroundRide(m_pos, terrainPos, rotDegAng);
					break;
				}
				case ObjectType::Propeller:
				{
					Math::Vector3 terrainPos = spHitTerrain->GetPos();
					// 回転床が回転する角度
					Math::Vector3 rotDegAng = spHitTerrain->GetParam().degAng;
					
					// プロペラに乗った時の処理
					MoveObjectRideProcess::Instance().PropellerRide(m_pos, terrainPos, rotDegAng, m_angle);

					// 回転行列
					m_rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));
				}
			}
		}
	}

	transMat = Math::Matrix::CreateTranslation(m_pos);
	
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_scale);

	m_mWorld = scaleMat * m_rotMat * transMat;

	if (m_aliveFlg == true)
	{
		// アニメーションの更新
		m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
		m_spModel->CalcNodeMatrices();
	}

	// 3D音のリスナーにプレイヤーを設定
	KdAudioManager::Instance().SetListnerMatrix(m_mWorld);
}

void Player::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().WorkAmbientController().SetDirLight({ 0, -1, 0 }, { 3, 3, 3 });
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
		KdShaderManager::Instance().WorkAmbientController().SetDirLight({ 1, -1, 1 }, { 3, 3, 3 });
	}
}

void Player::DrawLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void Player::DrawUnLit()
{
	//if (m_landingEffectFlg == false) return;
	//Math::Matrix mat = Math::Matrix::CreateTranslation(m_landingEffectPos);
	//if (m_spEffectModel)
	//{
	//	KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
	//	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spEffectModel, mat);
	//	KdShaderManager::Instance().UndoRasterizerState();
	//}
}

void Player::DrawBright()
{
	if (m_goalFlg == true) return;

	Math::Color color = { 0.0f, 0.0f, 1.0f, m_landingEffectAlpha };
	if (m_landingEffectFlg == false) return;
	Math::Matrix mat = Math::Matrix::CreateTranslation(m_landingEffectPos);
	if (m_spEffectModel)
	{
		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spEffectModel, mat, color);
		KdShaderManager::Instance().UndoRasterizerState();
	}
}

void Player::Init()
{
	CharacterBase::Init();

	DataLoad();

	if (!m_spModel)
	{
		// モデル
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Character/Player/player.gltf");

		// 初期のアニメーション
		m_spAnimator = std::make_shared<KdAnimator>();
		SetAnimation("Idle", true);
	}

	// エフェクト
	m_spEffectModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Effect/LandingPosEffect/Out/out.gltf");

	// オブジェクトのタイプ
	m_objectType = ObjectType::Player;

	m_baseObjectType = BaseObjectType::Player;

	m_pos = m_stageStartPos;
	m_respawnPos = m_pos;

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	// コライダー
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Player", m_spModel, KdCollider::TypeDamageLine);

	// 移動が出来なくなるフラグ
	m_stopFlg = true;

	// 音
	// 草の上を歩いた音
	SoundLoad(m_wpWalkSE[WalkSEType::Grass], "Asset/Sounds/SE/grassWalk.wav", 0.3f);
	// かたい地面を歩いた音
	SoundLoad(m_wpWalkSE[WalkSEType::Tile], "Asset/Sounds/SE/tileWalk.wav", 0.15f);
	// 歩いているSEを鳴らしているかのフラグ
	m_nowWalkSEFlg = false;

	// ジャンプ音
	SoundLoad(m_jumpSE.wpSound, "Asset/Sounds/SE/jump.wav", 0.03f);
	m_jumpSE.flg = false;

	// 敵を踏んだ時の音
	SoundLoad(m_stampSE.wpSound, "Asset/Sounds/SE/stamp.wav", 0.03f);
	m_stampSE.flg = false;

	// キノコではねた時の音
	SoundLoad(m_boundSE.wpSound, "Asset/Sounds/SE/bound.wav", 0.06f);
	m_boundSE.flg = false;

	// 落下死したときの音
	SoundLoad(m_dropSE.wpSound, "Asset/Sounds/SE/drop.wav", 0.06f);

	// ゴールした時のSE
	SoundLoad(m_goalSE.wpSound, "Asset/Sounds/SE/goal.wav", 0.06f);
	m_goalSE.flg = false;

	// ゴールした時のBGM
	SoundLoad(m_goalBGM.wpSound, "Asset/Sounds/BGM/clear.wav", 0.03f);

	// ステージを選択したときの音
	SoundLoad(m_stageSelectSE.wpSound, "Asset/Sounds/SE/select.wav", 0.2f);

	// 箱を持った時の音
	SoundLoad(m_holdSE.wpSound, "Asset/Sounds/SE/hold.wav", 0.03f);

	// 箱を離した時の音
	SoundLoad(m_letGoSE.wpSound, "Asset/Sounds/SE/letGo.wav", 0.03f);
}

void Player::Reset()
{
	CharacterBase::Reset();
	
	// リスポーン座標からやり直す
	m_pos = m_respawnPos;
	m_oldPos = Math::Vector3::Zero;

	// 生存フラグ
	m_aliveFlg = true;

	// 状態
	m_situationType = SituationType::Idle;

	// 角度
	m_angle = 0;
	m_rotMat = Math::Matrix::CreateRotationY(m_angle);

	m_stopFlg = true;

	// アニメーションをIdleに
	SetAnimation("Idle", true);

	// 煙カウント
	m_smokeCount = 0;

	m_jumpSE.flg = false;

	m_firstLandFlg = false;
	m_pauseFlg = false;

	m_holdFlg = false;
	m_missingShotCount = 0;
	m_missingShotFlg = false;
}

void Player::BackPos()
{
	m_pos.x = m_oldPos.x;
	m_pos.z = m_oldPos.z;
}

void Player::OnHit()
{
	m_aliveFlg = false;
}

void Player::SetStopFlg(const bool _stopFlg)
{
	if (m_stopFlg == _stopFlg) return;
	m_stopFlg = _stopFlg;

	// UIにも渡す
	if (m_wpGameUI.expired() == false)
	{
		m_wpGameUI.lock()->SetStopFlg(_stopFlg);
	}
}

const bool Player::IsCameraTracking() const
{
	// 当たった運ぶオブジェクトがある時
	if (m_wpHitCarryObject.expired() == false)
	{
		// チェック
		return m_wpHitCarryObject.lock()->IsCameraTracking();
	}

	// 当たった地面がある時
	if (m_wpHitTerrain.expired() == false)
	{
		// チェック
		return m_wpHitTerrain.lock()->IsCameraTracking();
	}

	return false;
}

// 当たり判定
void Player::HitJudge()
{
	if (SceneManager::Instance().GetDebug()) return;

	// 当たったオブジェクトリストをリセット
	m_wpHitObjectList.clear();

	// 運べるオブジェクトとの当たり判定
	HitJudgeCarryObject();

	// 地面との当たり判定
	HitJudgeGround();

	// 触れたらイベントが発生する
	HitJudgeEvent();

	// 敵との当たり判定
	HitJudgeEnemy();
}

void Player::HitJudgeGround()
{
	// 動く床関連をリセット
	// 動く床
	m_moveGround.transMat = Math::Matrix::Identity;
	m_moveGround.hitFlg = false;
	// 回る床
	m_rotationGround.transMat = Math::Matrix::Identity;
	m_rotationGround.hitFlg = false;

	// 当たった地面をリセット
	//m_wpHitTerrain.reset();

	// 地面とのレイ当たり判定
	if (m_carryObject.hitFlg == false)
	{
		// 当たった座標
		Math::Vector3 hitPos = Math::Vector3::Zero;
		// ノードの座標
		Math::Vector3 nodePos = Math::Vector3::Zero;
		// 当たったかどうか
		bool hitFlg = false;

		// レイの情報
		KdCollider::RayInfo rayInfo;
		// レイの長さ
		rayInfo.m_range = m_gravity + m_enableStepHeight;
		// レイの座標(キャラの中心)
		rayInfo.m_pos.y = m_enableStepHeight;
		rayInfo.m_pos += m_pos;
		// レイの方向
		rayInfo.m_dir = Math::Vector3::Down;
		// レイのタイプ
		rayInfo.m_type = KdCollider::TypeGround;

		// レイ判定
		hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitTerrain);
		// 着地場所チェック
		if (m_landingEffectFlg == false)
		{
			LandingPosEffectPos(rayInfo);
		}

		// 当たっていなかったら右足からのレイ判定をする
		if (hitFlg == false)
		{
			// 右足の場所
			nodePos = m_spModel->FindNode("Right")->m_worldTransform.Translation();
			// 回転処理
			rayInfo.m_pos.x = nodePos.x * cos(DirectX::XMConvertToRadians(-m_angle)) + m_pos.x;
			rayInfo.m_pos.z = nodePos.x * sin(DirectX::XMConvertToRadians(-m_angle)) + m_pos.z;

			// レイ判定
			hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitTerrain);

			// 着地場所チェック
			if (m_landingEffectFlg == false)
			{
				LandingPosEffectPos(rayInfo);
			}
		}

		// 当たっていなかったら左足からのレイ判定をする
		if (hitFlg == false)
		{
			// 左足の場所
			nodePos = m_spModel->FindNode("Left")->m_worldTransform.Translation();
			// 回転処理
			rayInfo.m_pos.x = nodePos.x * cos(DirectX::XMConvertToRadians(m_angle)) + m_pos.x;
			rayInfo.m_pos.z = nodePos.x * sin(DirectX::XMConvertToRadians(m_angle)) + m_pos.z;

			// レイ判定
			hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitTerrain);

			// 着地場所チェック
			if (m_landingEffectFlg == false)
			{
				LandingPosEffectPos(rayInfo);
			}
		}

		// 当たっていた時の処理
		if (hitFlg)
		{
			// 当たったオブジェクト
			std::shared_ptr<TerrainBase> spHitObject = m_wpHitTerrain.lock();

			// 音のフラグをリセットする
			m_boundSE.flg = false;	// きのこで跳ねた時の音

			if (spHitObject)
			{
				spHitObject->OnHit();

				switch (spHitObject->GetObjectType())
				{
					// バウンドする床に乗った場合
				case ObjectType::BoundGround:
					// 座標
					m_pos.y = hitPos.y;
					// 重力
					m_gravity = -m_boundJumpPow;

					// ジャンプ状態にする
					m_situationType |= SituationType::Jump;

					// 跳ねた時の音を鳴らす
					if (m_boundSE.flg == false)
					{
						if (!m_boundSE.wpSound.expired())
						{
							m_boundSE.wpSound.lock()->Play();
							m_boundSE.flg = true;
						}
					}
					break;

					// 動く床に乗った場合
				case ObjectType::MoveGround:
				case ObjectType::DropGround:
				case ObjectType::Switch:
					// 座標
					m_pos.y = hitPos.y;
					// 重力
					m_gravity = 0;
					// 空中にいない
					m_situationType &= (~SituationType::Air);
					m_situationType &= (~SituationType::Jump);
					// 動く床の動く前の行列
					m_moveGround.transMat = Math::Matrix::CreateTranslation(spHitObject->GetPos());
					// 動く床に当たったかどうかのフラグ
					m_moveGround.hitFlg = true;
					break;

					// 回る床に乗った場合
				case ObjectType::RotationGround:
				case ObjectType::Propeller:
					// 座標
					m_pos.y = hitPos.y;
					// 重力
					m_gravity = 0;
					// 空中にいない
					m_situationType &= (~SituationType::Air);
					m_situationType &= (~SituationType::Jump);
					// 当たったかどうか
					m_rotationGround.hitFlg = true;
					break;

					// 坂
				case ObjectType::SlopeGround:
					m_pos.y = hitPos.y;
					// 重力
					m_gravity = 0.03f;
					// 空中にいない
					m_situationType &= (~SituationType::Air);
					m_situationType &= (~SituationType::Jump);
					break;

				default:
					// 座標
					m_pos.y = hitPos.y;
					// 重力
					m_gravity = 0;
					// 空中にいない
					m_situationType &= (~SituationType::Air);
					m_situationType &= (~SituationType::Jump);
					break;
				}
			}
		}
		else
		{
			// 空中にいる
			m_situationType |= SituationType::Air;
		}
	}

	// 地面(壁)とのスフィア判定
	// BOX当たってたらにするかも
	{
		// ボックス判定
		KdCollider::BoxInfo boxInfo;
		// 右前上のノード
		Math::Vector3 rightBackUpPos = m_spModel->FindNode("RightBackUp")->m_worldTransform.Translation();
		// 左後上のノード
		Math::Vector3 leftFrontUpPos = m_spModel->FindNode("LeftFrontUp")->m_worldTransform.Translation();
		// キャラクターの高さ
		float charaHighLength = rightBackUpPos.y;
		float radius = 0.35f;
		//float radius = abs(rightBackUpPos.z - leftFrontUpPos.z);
		// 中心座標(キャラクターの真ん中)
		boxInfo.m_Abox.Center = m_pos;
		boxInfo.m_Abox.Center.y += charaHighLength / 2.0f;
		boxInfo.m_Abox.Extents = { radius, charaHighLength / 2, radius };
		// 当たり判定をするタイプ
		boxInfo.m_type = KdCollider::TypeGround;

		Math::Matrix mat = Math::Matrix::CreateTranslation(boxInfo.m_Abox.Center);
		// 当たり判定の結果格納リスト
		std::list<KdCollider::CollisionResult> resultList;

		bool boxHitFlg = false;

		bool fineHitFlg = false;

		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			if (obj->Intersects(boxInfo, nullptr))
			{
				boxHitFlg = true;
				// 当たったオブジェクトが細かく当たり判定をしないといけないオブジェクトだった場合フラグを立てる
				if (obj->CheckFineHitJudge() == true)
				{
					fineHitFlg = true;
				}
				break;
			}
		}

		if (boxHitFlg == true)
		{
			// スフィアの情報
			KdCollider::SphereInfo sphereInfo;

			// 動いた量
			Math::Vector3 movePos = m_pos - m_oldPos;

			// スフィアの中心座標
			sphereInfo.m_sphere.Center = m_pos;
			// スフィアの半径
			sphereInfo.m_sphere.Radius = radius;
			sphereInfo.m_sphere.Center.y += sphereInfo.m_sphere.Radius - 0.1f;
			// スフィアのタイプ
			sphereInfo.m_type = KdCollider::TypeGround;

			// 当たったかどうかのフラグ
			bool hitFlg = false;
			// 当たった結果
			std::list<KdCollider::CollisionResult> collisionResultList;

			if (fineHitFlg == false)
			{
				// 当たり判定の回数
				const int checkNum = 3;
				// スフィアのY座標を上げていく
				const float upY = 0.5f;
				if (m_situationType & SituationType::Air)
				{
					hitFlg = SphereHitJudge(sphereInfo, collisionResultList);
				}
				if (hitFlg == false)
				{
					for (int i = 1; i <= checkNum; i++)
					{
						sphereInfo.m_sphere.Center.y += upY;
						hitFlg = SphereHitJudge(sphereInfo, collisionResultList);
						if (hitFlg == true) break;
					}
				}
			}
			// 細かく当たり判定をする
			else
			{
				// 細かく判定をする回数
				const int fineNum = 3;
				for (int i = 0; i < fineNum; i++)
				{
					sphereInfo.m_sphere.Center = m_oldPos + (movePos / 2.0f) * (float)i;
					sphereInfo.m_sphere.Center.y += sphereInfo.m_sphere.Radius;

					// 当たり判定の回数
					const int checkNum = 6;
					// スフィアのY座標を上げていく
					const float upY = 0.25f;

					if (m_situationType & SituationType::Air)
					{
						hitFlg = SphereHitJudge(sphereInfo, collisionResultList);
					}
					if (hitFlg == false)
					{
						for (int j = 1; j <= checkNum; j++)
						{
							sphereInfo.m_sphere.Center.y += upY;
							hitFlg = SphereHitJudge(sphereInfo, collisionResultList);
							if (hitFlg == true) break;
						}
					}
					if (hitFlg == true) break;
				}
			}

			// 押し戻し
			if (hitFlg)
			{
				Math::Vector3 adjustVec;
				for (auto& result : collisionResultList)
				{
					result.m_hitDir.Normalize();
					adjustVec += result.m_hitDir * result.m_overlapDistance;
				}

				m_pos.x = sphereInfo.m_sphere.Center.x + adjustVec.x;
				m_pos.z = sphereInfo.m_sphere.Center.z + adjustVec.z;
			}

		}
		// 頭判定
		bool hitFlg = false;
		KdCollider::SphereInfo sphereInfo;
		sphereInfo.m_sphere.Center = m_pos;
		sphereInfo.m_sphere.Radius = radius;
		sphereInfo.m_sphere.Center.y += 1.75f;
		sphereInfo.m_type = KdCollider::TypeGround;
		std::list<KdCollider::CollisionResult> collisionResult;

		hitFlg = SphereHitJudge(sphereInfo, collisionResult);
		if (hitFlg == true)
		{
			KdCollider::CollisionResult result = collisionResult.front();
			// ひっかかり防止
			if (result.m_hitDir.y > 0)
			{
				result.m_hitDir *= -1;
			}
			result.m_hitDir.Normalize();
			Math::Vector3 pos = result.m_overlapDistance * result.m_hitDir;
			m_pos.y += pos.y;
		}
	}
}

void Player::HitJudgeEvent()
{
	if (m_wpEventObjectController.expired() == true) return;

	// ボックス判定
	{
		// ボックス判定
		KdCollider::BoxInfo boxInfo;
		// 右前上のノード
		Math::Vector3 rightBackUpPos = m_spModel->FindNode("RightBackUp")->m_worldTransform.Translation();
		// 左後上のノード
		Math::Vector3 leftFrontUpPos = m_spModel->FindNode("LeftFrontUp")->m_worldTransform.Translation();
		// キャラクターの高さ
		float charaHighLength = rightBackUpPos.y;
		// 中心座標(キャラクターの真ん中)
		boxInfo.m_Abox.Center = m_pos;
		boxInfo.m_Abox.Center.y += charaHighLength / 2.0f;
		// キャラクターの横幅(半径)
		float charaSideRadius = abs(rightBackUpPos.x);
		// キャラクターの奥行幅(直径)
		float charaZLength = abs(rightBackUpPos.z - leftFrontUpPos.z);
		// ボックスのサイズ
		//boxInfo.m_Abox.Extents = { 5, 5, 5 };
		boxInfo.m_Abox.Extents = { charaSideRadius, charaHighLength / 2, charaZLength / 2 };
		// 当たり判定をするタイプ
		boxInfo.m_type |= KdCollider::TypeEvent;


		// 当たり判定の結果格納リスト
		std::list<KdCollider::CollisionResult> resultList;

		bool hitFlg = false;

		// 当たったオブジェクト
		std::shared_ptr<EventObjectBase> spHitObject;

		for (auto& obj : m_wpEventObjectController.lock()->GetObjList())
		{
			if (obj.expired() == true) continue;
			if (obj.lock()->Intersects(boxInfo, &resultList))
			{
				spHitObject = obj.lock();
				hitFlg = true;
				break;
			}
		}

		if (hitFlg)
		{
			if (spHitObject)
			{
				spHitObject->OnHit();
				switch (spHitObject->GetObjectType())
				{
					// ゴール
				case ObjectType::Goal:
				{
					// ゴールしたフラグを立てる
					m_goalFlg = true;
					// 操作を止める
					SetStopFlg(true);

					// ゴールした時のSEを鳴らす
					if (m_goalSE.flg == false)
					{
						if (m_goalSE.wpSound.expired() == false)
						{
							m_goalSE.wpSound.lock()->Play();
						}
						m_goalSE.flg = true;
					}
					break;
				}

					// セーブポイント
				case ObjectType::SavePoint:
					// リスポーン地点を更新
					m_respawnPos = spHitObject->GetPos();
					m_respawnPos.z -= 1.0f;
					// セーブポイントの旗を立っている状態にする
					for (auto& data : m_wpEventObjectController.lock()->WorkCSVData())
					{
						if (data.name == spHitObject->GetObjectName())
						{
							data.modelNum = 1;
							break;
						}
					}
					break;

					// ワープポイント
				case ObjectType::WarpPoint:
					// リスポーン座標にワープ
					if (GetAsyncKeyState('Z') & 0x8000)
					{
						m_pos = m_respawnPos;
					}
					// UIを出す
					if (m_wpGameUI.expired() == false)
					{
						m_wpGameUI.lock()->SetDrawType(GameUI::DrawType::Warp);
					}
					break;

					// 最終ゴール
				case ObjectType::FinalGoal:
					// 操作を止める
					SetStopFlg(true);
					m_finalGoalFlg = true;
					break;
				}
			}
		}
	}


	// フラグリセット
	m_stageSelectObjectFlg = false;
	std::shared_ptr<KdGameObject> spHitObject = nullptr;
	// ステージセレクトのオブジェクトとの視野角判定
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		// ステージセレクトのオブジェクトでなければ飛ばす
		if (obj->GetObjectType() != ObjectType::StageSelectObject) continue;

		// プレイヤーとオブジェクトの距離
		Math::Vector3 vec = obj->GetPos() - m_pos;
		// 一番近いオブジェクトのみ処理をするために、すでに判定をしたオブジェクトより遠かったらスキップする
		if (spHitObject)
		{
			Math::Vector3 maxVec = spHitObject->GetPos() - m_pos;
			if (vec.Length() < maxVec.Length())
			{
				continue;
			}
		}
		// プレイヤーの正面
		Math::Vector3 forwardVec = m_mWorld.Backward();

		// 設定されている距離より短かったら視野角判定
		if (vec.Length() < 4.0f)
		{
			// 視野角判定
			vec.Normalize();
			float dot = forwardVec.Dot(vec);
			dot = std::clamp(dot, -1.0f, 1.0f);
			float deg = DirectX::XMConvertToDegrees(acos(dot));
			// 視野角内ならステージに入れるようにする
			if (deg < 90)
			{
				spHitObject = obj;
			}

			m_stageSelectObjectFlg = true;
		}
	}
	// もし当たっていたら一番近いオブジェクトだけ処理をする
	if (spHitObject)
	{
		spHitObject->OnHit();
		if (GetAsyncKeyState('Z') & 0x8000)
		{
			if (m_actionKeyFlg == false && m_stopFlg == false)
			{
				// 操作を受け付けなくする
				SetStopFlg(true);
				m_beginGameSceneFlg = true;

				// キーフラグ
				m_actionKeyFlg = true;

				if (!m_stageSelectSE.wpSound.expired())
				{
					m_stageSelectSE.wpSound.lock()->Play();
				}
			}
		}
		else
		{
			m_actionKeyFlg = false;
		}
	}
}

void Player::HitJudgeEnemy()
{
	// ボックス判定
	{
		// ボックス判定
		KdCollider::BoxInfo boxInfo;
		// 右前上のノード
		Math::Vector3 rightBackUpPos = m_spModel->FindNode("RightBackUp")->m_worldTransform.Translation();
		// 左後上のノード
		Math::Vector3 leftFrontUpPos = m_spModel->FindNode("LeftFrontUp")->m_worldTransform.Translation();
		// キャラクターの高さ
		float charaHighLength = rightBackUpPos.y;
		// 中心座標(キャラクターの真ん中)
		boxInfo.m_Abox.Center = m_pos;
		boxInfo.m_Abox.Center.y += charaHighLength / 2.0f;
		// キャラクターの横幅(半径)
		float charaSideRadius = abs(rightBackUpPos.x);
		// キャラクターの奥行幅(直径)
		float charaZLength = abs(rightBackUpPos.z - leftFrontUpPos.z);
		// ボックスのサイズ
		//boxInfo.m_Abox.Extents = { 5, 5, 5 };
		boxInfo.m_Abox.Extents = { charaSideRadius, charaHighLength / 2, charaZLength / 2 };
		// 当たり判定をするタイプ
		boxInfo.m_type |= KdCollider::TypeDamage;


		// 当たり判定の結果格納リスト
		std::list<KdCollider::CollisionResult> resultList;

		bool hitFlg = false;

		std::shared_ptr<KdGameObject> spHitObject = nullptr;

		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			if (obj->Intersects(boxInfo, &resultList))
			{
				m_wpHitObjectList.push_back(obj);
				spHitObject = obj;
				hitFlg = true;
			}
		}

		// 当たっていた時の処理
		if (hitFlg)
		{
			// 音のフラグをリセット
			m_stampSE.flg = false;	// 敵を踏んだ時の音
			if (spHitObject)
			{
				// 敵の倒せる座標
				Math::Vector3 enemyPos = spHitObject->GetPos();
				// 上なら倒す
				if (m_pos.y >= enemyPos.y)
				{
					// 煙を生み出す
					std::shared_ptr<Smoke> smoke = std::make_shared<Smoke>();
					smoke->Init();
					smoke->SetPos(m_pos);
					smoke->SetSmokeType(Smoke::SmokeType::StampSmoke);
					SceneManager::Instance().AddObject(smoke);

					// 敵を踏んだ時の処理
					spHitObject->OnHit();
					m_gravity = -m_enemyJumpPow;

					// もしジャンプ状態じゃないならジャンプ状態にする
					if ((m_situationType & SituationType::Jump) == 0)
					{
						m_situationType |= SituationType::Jump;
					}

					// 敵を踏んだ時の音
					if (m_stampSE.flg == false)
					{
						if (!m_stampSE.wpSound.expired())
						{
							m_stampSE.wpSound.lock()->Play();
							m_stampSE.flg = true;
						}
					}
				}
				// 下半分なら死亡
				else
				{
					m_aliveFlg = false;
					m_pauseFlg = true;

					std::shared_ptr<KdSoundInstance> spDeathSound =  KdAudioManager::Instance().Play("Asset/Sounds/SE/death.wav", false);
					spDeathSound->SetVolume(0.1f);
				}
			}
		}
	}
}

void Player::HitJudgeCarryObject()
{
	m_wpHitTerrain.reset();
	m_wpHitCarryObject.reset();
	// 当たったオブジェクト系をリセット
	m_carryObjectHitTerrain.hitFlg = false;
	m_carryObjectHitTerrain.transMat = Math::Matrix::Identity;
	// 着地エフェクトフラグリセット
	m_landingEffectFlg = false;

	m_carryObject.hitFlg = false;
	m_carryObject.transMat = Math::Matrix::Identity;
	// 運んでいる状態なら当たり判定しない
	if (m_situationType & SituationType::Carry) return;
	// オブジェクトリストを入手できない状態なら当たり判定しない
	if (m_wpCarryObjectController.expired()) return;

	// 当たった座標
	Math::Vector3 hitPos = Math::Vector3::Zero;
	// ノードの座標
	Math::Vector3 nodePos = Math::Vector3::Zero;
	// 当たったかどうか
	bool hitFlg = false;

	// レイの情報
	KdCollider::RayInfo rayInfo;
	// レイの長さ
	rayInfo.m_range = m_gravity + 0.5f + m_enableStepHeight;
	// レイの座標(キャラの中心)
	rayInfo.m_pos.y = m_enableStepHeight + 0.5f;
	rayInfo.m_pos += m_pos;
	// レイの方向
	rayInfo.m_dir = Math::Vector3::Down;
	// レイのタイプ
	rayInfo.m_type = KdCollider::TypeGround;

	// レイ判定
	hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitCarryObject);

	// 着地エフェクトフラグリセット
	m_landingEffectFlg = false;

	// 着地場所チェック
	LandingPosEffectPos(rayInfo);

	// 当たっていなかったら右足からのレイ判定をする
	if (hitFlg == false)
	{
		// 右足の場所
		nodePos = m_spModel->FindNode("Right")->m_worldTransform.Translation();
		// 回転処理
		rayInfo.m_pos.x = nodePos.x * cos(DirectX::XMConvertToRadians(-m_angle)) + m_pos.x;
		rayInfo.m_pos.z = nodePos.x * sin(DirectX::XMConvertToRadians(-m_angle)) + m_pos.z;
		// レイ判定
		hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitCarryObject);

		// 着地場所チェック
		if (m_landingEffectFlg == false)
		{
			LandingPosEffectPos(rayInfo);
		}
	}

	// 当たっていなかったら左足からのレイ判定をする
	if (hitFlg == false)
	{
		// 左足の場所
		nodePos = m_spModel->FindNode("Left")->m_worldTransform.Translation();
		// 回転処理
		rayInfo.m_pos.x = nodePos.x * cos(DirectX::XMConvertToRadians(m_angle)) + m_pos.x;
		rayInfo.m_pos.z = nodePos.x * sin(DirectX::XMConvertToRadians(m_angle)) + m_pos.z;
		// レイ判定
		hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitCarryObject);

		// 着地場所チェック
		if (m_landingEffectFlg == false)
		{
			LandingPosEffectPos(rayInfo);
		}
	}

	// 当たっていた時の処理
	if (hitFlg)
	{
		// 当たったオブジェクト
		std::shared_ptr<CarryObjectBase> spHitObject = m_wpHitCarryObject.lock();

		if (spHitObject)
		{
			spHitObject->OnHit();

			// 箱状態の時の処理
			if (spHitObject->IsCarryCheck() == true)
			{
				switch (spHitObject->GetObjectType())
				{
				case ObjectType::Box:
				case ObjectType::BoxEnemy:
					// 座標
					m_pos.y = hitPos.y;
					// 重力
					m_gravity = 0;
					// 空中にいない
					m_situationType &= (~SituationType::Air);
					m_situationType &= (~SituationType::Jump);
					// 動く床の動く前の行列
					m_carryObject.transMat = Math::Matrix::CreateTranslation(spHitObject->GetPos());
					// 動く床に当たったかどうかのフラグ
					m_carryObject.hitFlg = true;
					if (spHitObject->GetHitTerrain().expired() == false)
					{
						m_wpHitTerrain = spHitObject->GetHitTerrain();
						m_carryObjectHitTerrain.transMat = Math::Matrix::CreateTranslation(m_wpHitTerrain.lock()->GetPos());
						m_carryObjectHitTerrain.hitFlg = true;
					}
					break;
				}
			}
		}
	}

	// フラグリセット
	m_holdFlg = false;
	// 物を持てるかどうか
	for (auto& obj : m_wpCarryObjectController.lock()->GetObjList())
	{
		if (obj.expired() == true) continue;
		// 敵状態なら飛ばす
		if (obj.lock()->IsCarryCheck() == false) continue;
		// 持てるか持てないかの判定
		// プレイヤーとオブジェクトの距離
		Math::Vector3 vec = obj.lock()->GetPos() - m_pos;
		// プレイヤーの正面
		Math::Vector3 forwardVec = m_mWorld.Backward();

		// 設定されている距離より短かったら視野角判定
		if (vec.Length() < obj.lock()->GetParam().area)
		{
			// 視野角判定
			vec.Normalize();
			float dot = forwardVec.Dot(vec);
			dot = std::clamp(dot, -1.0f, 1.0f);
			float deg = DirectX::XMConvertToDegrees(acos(dot));
			// 視野角内なら持てる用にする
			if (deg < 90)
			{
				// 物を持てる状態にする
				m_holdFlg = true;

				obj.lock()->SetSelectWhite(true);
				if (m_wpGameUI.expired() == false)
				{
					m_wpGameUI.lock()->SetDrawType(GameUI::DrawType::HoldBox);
				}
				if (m_stopFlg == false)
				{
					if (GetAsyncKeyState('Z') & 0x8000)
					{
						// 運んでいる状態なら処理しない
						if ((m_situationType & SituationType::Carry) == 0)
						{
							if (m_actionKeyFlg == false)
							{
								// 運ばれている状態にする
								obj.lock()->CarryFlg(true);
								// オブジェクトを保持
								m_wpCarryObject = obj;
								// 状態の切り替え
								m_situationType |= SituationType::CarryAnimation;
								// 走り状態を解除
								m_situationType &= (~SituationType::Run);
								// キーフラグ
								m_actionKeyFlg = true;
								// 音を鳴らす
								if (m_holdSE.wpSound.expired() == false)
								{
									m_holdSE.wpSound.lock()->Play();
								}
								m_stopFlg = true;

								// キャラクターの角度を箱の方に向ける
								// 向きたい方向
								Math::Vector3 objVec = obj.lock()->GetPos() - m_pos;
								objVec.y = 0;
								objVec.Normalize();

								RotationCharacter(m_angle, objVec);
								m_rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));
							}
						}
					}
					else
					{
						m_actionKeyFlg = false;
					}
				}
			}
		}
	}
}

void Player::ChangeAnimation()
{
	if (m_missingShotFlg == true) return;

	// 持つ時のアニメーション
	if (m_situationType & SituationType::CarryAnimation)
	{
		SetAnimation("CarryAnimation", false);
		return;
	}

	// 運び状態の場合
	if (m_situationType & SituationType::Carry)
	{
		// 運び状態のみの場合
		if ((m_situationType ^ SituationType::Carry) == 0)
		{
			SetAnimation("Carry", true);
		}
		// 歩きの場合
		if (m_situationType & SituationType::Walk)
		{
			SetAnimation("CarryWalk", true);
		}
		// 空中の場合
		if (m_situationType & SituationType::Air)
		{
			SetAnimation("Carry", true);
		}
	}
	// 運び状態じゃない場合
	else
	{
		// Idleのみの場合
		if ((m_situationType ^ SituationType::Idle) == 0)
		{
			SetAnimation("Idle", true);
		}
		else
		{
			// ジャンプの場合
			if (m_situationType & SituationType::Jump)
			{
				// ジャンプをしていなかった、または、運ばれている状態が解除された時にジャンプアニメーションを再生する
				if ((m_oldSituationType & SituationType::Jump) == 0 || m_oldSituationType & SituationType::Carry)
				{
					SetAnimation("Jump", false);
				}
			}
			// ジャンプではないが空中の場合
			else if (m_situationType & SituationType::Air)
			{
				SetAnimation("Idle", true);
			}
			// 歩きの場合
			else if (m_situationType & SituationType::Walk)
			{
				SetAnimation("Walk", true);
			}
			// 走りの場合
			else if (m_situationType & SituationType::Run)
			{
				SetAnimation("Run", true);
			}
		}
	}
}

void Player::DataLoad()
{
	// JSONファイルを読み込む
	std::ifstream file(m_path.data());
	if (!file.is_open()) return;

	nlohmann::json data;
	file >> data;

	// JSONデータを格納していく
	m_runSpeed			= data["Player"]["m_runSpeed"];			// 走る速度
	m_walkSpeed			= data["Player"]["m_walkSpeed"];		// 歩く速度
	m_jumpPow			= data["Player"]["m_jumpPow"];			// ジャンプ力
	m_boundJumpPow		= data["Player"]["m_boundJumpPow"];		// 跳ねる床を踏んだ時のジャンプ力
	m_enemyJumpPow		= data["Player"]["m_enemyJumpPow"];		// 敵を踏んだ時のジャンプ力
	m_stageStartPos.x	= data["Player"]["m_stageStartPos.x"];	// 初期座標
	m_stageStartPos.y	= data["Player"]["m_stageStartPos.y"];
	m_stageStartPos.z	= data["Player"]["m_stageStartPos.z"];
	m_enableStepHeight	= data["Player"]["m_enableStepHeight"];	// 段差許容範囲
	m_scale				= data["Player"]["m_scale"];			// 拡縮
	m_walkSmokeTime		= data["Player"]["m_walkSmokeTime"];	// 歩く時の煙や音が出る間隔
	m_runSmokeTime		= data["Player"]["m_runSmokeTime"];		// 走る時の煙や音が出る間隔
	m_underLine			= data["Player"]["m_underLine"];		// これ以上下に行くと死ぬライン
	m_maxAngle			= data["Player"]["m_maxAngle"];			// 回転角度制限
}

void Player::GoalProcess()
{
	m_missingShotFlg = false;

	m_goalStayCount++;
	if (m_goalStayCount > m_goalStayTime)
	{
		// Y座標以外ゴールの座標にする
		m_pos.x = m_goalPos.x;
		m_pos.z = m_goalPos.z;

		// もし１ステージ目なら少し前に出す(土台がでかくて埋まってしまうので)
		if (SceneManager::Instance().GetNowStage() == 1)
		{
			m_pos.z -= 1.0f;
		}

		// カメラ側に向かせる
		m_angle = 180;
		m_rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));

		// ゴールのBGMを鳴らす
		if (m_goalBGM.flg == false)
		{
			if (m_goalBGM.wpSound.expired() == false)
			{
				m_goalBGM.wpSound.lock()->Play();
			}
			m_goalBGM.flg = true;
		}
	}

	if (m_goalJumpFlg == true)
	{
		if (m_oldGoalJumpFlg == false)
		{
			// ゴールしてカメラが動き終わった際に、ジャンプする
			SetAnimation("Jump", false);
			m_situationType |= Jump;
			m_gravity = -m_jumpPow;
			m_oldGoalJumpFlg = true;
		}

		// 重力
		m_gravity += m_gravityPow;

		if (m_gravity >= 0)
		{
			m_gravity = 0;
		}
		m_pos.y -= m_gravity;
	}
}

void Player::LandingPosEffectPos(KdCollider::RayInfo _rayInfo)
{
	// 着地場所チェック
	KdCollider::RayInfo landingRayInfo = _rayInfo;
	// 長さを長くする
	landingRayInfo.m_range = 100;
	// レイ判定
	m_landingEffectFlg = RayHitJudge(landingRayInfo, m_landingEffectPos);
	// 当たっていたら、x,z座標を現在の位置にする。y座標のみ当たった座標
	if (m_landingEffectFlg)
	{
		m_landingEffectPos.x = m_pos.x;
		m_landingEffectPos.z = m_pos.z;
	}

	float length = m_pos.y - m_landingEffectPos.y;
	if (length > m_maxLandingEffectLength)
	{
		m_landingEffectAlpha = 1.0f;
	}
	else
	{
		m_landingEffectAlpha = length / m_maxLandingEffectLength;
	}
}

void Player::SoundLoad(std::weak_ptr<KdSoundInstance>& _wpSound, std::string _path, float _vol)
{
	_wpSound = KdAudioManager::Instance().Play(_path);
	if (!_wpSound.expired())
	{
		_wpSound.lock()->SetVolume(_vol);
		_wpSound.lock()->Stop();
	}
}
