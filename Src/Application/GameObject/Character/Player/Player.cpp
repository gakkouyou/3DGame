#include "Player.h"
#include "../../../Scene/SceneManager.h"
#include "../../Camera/CameraBase.h"

#include "../../Effect/Smoke/Smoke.h"
#include "../../UI/GameUI/GameUI.h"

#include "../../Terrain/TerrainBase.h"
#include "../../../Tool/ObjectController/TerrainController/TerrainController.h"
#include "../../../Tool/ObjectController/CarryObjectController/CarryObjectController.h"
#include "../../Terrain/CarryObject/CarryObjectBase.h"
#include "../../../Tool/ObjectController/EnemyController/EnemyController.h"
#include "../Enemy/EnemyBase.h"
#include "../../../Tool/ObjectController/EventObjectController/EventObjectController.h"
#include "../../EventObject/EventObjectBase.h"

#include "../../../main.h"

#include <../Library/tinygltf/json.hpp>

void Player::Update()
{
	// デバッグモード中は更新しない
	if (SceneManager::Instance().GetDebug()) return;
	// ポーズ画面中は更新しない
	if (m_pauseFlg == true) return;

	// 死んでいたら操作を受け付けない
	if (m_aliveFlg == false)
	{
		m_stopFlg = true;
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

	// 更新前のプレイヤーの状態を保持
	UINT oldSituationType = m_situationType;

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
	// 移動初期化
	m_moveVec = Math::Vector3::Zero;

	if (m_stopFlg == false)
	{
		// WASDで移動
		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			m_moveVec.z += 1.0f;
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
			m_moveVec.x -= 1.0f;
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
			m_moveVec.z -= 1.0f;
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
			m_moveVec.x += 1.0f;
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
		m_nowWalkSoundFlg = false;
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
				m_nowWalkSoundFlg = false;
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
				m_nowWalkSoundFlg = false;
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
					if (m_jumpSound.flg == false)
					{
						if (!m_jumpSound.wpSound.expired())
						{
							m_jumpSound.wpSound.lock()->Play();
							m_jumpSound.flg = true;
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
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		{
			m_pos += m_moveVec * 1.0f;
		}
		else
		{
			m_pos += m_moveVec * m_runSpeed;
		}
	}

	// 重力
	m_gravity += m_gravityPow;
	m_pos.y -= m_gravity;
	// 重力の制限
	if (m_gravity >= m_maxGravity)
	{
		m_gravity = m_maxGravity;
	}

	// 落下死
	if (m_pos.y < -15.0f && m_aliveFlg == true)
	{
		m_aliveFlg = false;
		
		std::shared_ptr<KdSoundInstance> dropSE = KdAudioManager::Instance().Play("Asset/Sounds/SE/drop.wav");
		dropSE->SetVolume(0.06f);
	}

	// 移動中
	// 回転処理
	if (m_situationType & SituationType::Walk || m_situationType & SituationType::Run)
	{
		// 回転
		RotationCharacter(m_angle, m_moveVec);

		// 回転行列
		m_rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));
	}

	// エフェクシア座標追尾===========================================================
	if (!m_wpEffekseer.expired())
	{
		m_wpEffekseer.lock()->SetPos(m_pos);
		if (!m_wpEffekseer.lock()->IsPlaying() == true)
		{
			m_wpEffekseer.reset();
		}
	}
	// ===============================================================================

	// 運び状態を解除する
	if (GetAsyncKeyState('F') & 0x8000)
	{
		if (m_situationType & SituationType::Carry)
		{
			if (m_actionKeyFlg == false)
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
			}
		}
	}
	else
	{
		m_actionKeyFlg = false;
	}

	// 当たり判定
	HitJudge();

	// 空中になった瞬間の状態を保持
	if (m_situationType & SituationType::Air)
	{
		if ((oldSituationType & SituationType::Air) == 0)
		{
			m_lastGroundSituationType = m_situationType;
		}
	}

	// 着地した瞬間
	if ((m_situationType & SituationType::Air) == 0)
	{
		if (oldSituationType & SituationType::Air)
		{
			// ジャンプ音のフラグをfalseに
			m_jumpSound.flg = false;
		}
	}



	// アニメーションを切り替える
	if (oldSituationType != m_situationType)
	{
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
					if ((oldSituationType & SituationType::Jump) == 0 || oldSituationType & SituationType::Carry)
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

	// 何の地面に乗っているかによって、音を変える
	if (!m_wpHitTerrain.expired())
	{
		ObjectType type = m_wpHitTerrain.lock()->GetObjectType();
		switch (type)
		{
		// 草の音
		case ObjectType::NormalGround:
		case ObjectType::SlopeGround:
		case ObjectType::RotationGround:
			m_walkSoundType = WalkSoundType::Grass;
			break;

		case ObjectType::BoundGround:
			break;

		// コツコツ見たいな音
		default:
			m_walkSoundType = WalkSoundType::Tile;
			break;
		}
	}

	// Walk or Run 状態の時に空中にいなかったら歩く音を鳴らすようにする
	if(((m_situationType & Walk) || (m_situationType & Run)) && ((m_situationType & Air) == 0))
	{
		if (m_nowWalkSoundFlg == false)
		{
			if (m_wpWalkSound[m_walkSoundType].expired() == false)
			{
				m_wpWalkSound[m_walkSoundType].lock()->Play();
				m_nowWalkSoundFlg = true;
			}
		}
	}
	// 止める
	else
	{
		if (m_wpWalkSound[m_walkSoundType].expired() == false)
		{
			m_wpWalkSound[m_walkSoundType].lock()->Stop();
		}
	}

	// ゴールの処理
	if (m_goalFlg)
	{
		GoalProcess();
	}
}

void Player::PostUpdate()
{
	// ポーズ画面中は更新しない
	if (m_pauseFlg == true) return;

	Math::Matrix transMat = Math::Matrix::Identity;
	// 動く床に当たっていた時の処理
	if (m_moveGround.hitFlg)
	{
		// 動く床の動く前の逆行列
		Math::Matrix inverseMatrix = DirectX::XMMatrixInverse(nullptr, m_moveGround.transMat);
		// 動く床から見たプレイヤーの座標行列
		Math::Matrix playerMat = Math::Matrix::CreateTranslation(m_pos) * inverseMatrix;

		// 動く床の場合
		std::shared_ptr<TerrainBase> spHitObject = m_wpHitTerrain.lock();
		if (spHitObject)
		{
			// 動く床の動いた後の行列
			Math::Matrix afterMoveGroundMat = Math::Matrix::CreateTranslation(spHitObject->GetPos());

			// 座標を確定
			m_pos = afterMoveGroundMat.Translation() + playerMat.Translation();
		}
	}

	// 回る床に当たっていた時の処理
	if (m_rotationGround.hitFlg)
	{
		// 当たった地面
		std::shared_ptr<TerrainBase> spHitTerrain = m_wpHitTerrain.lock();
		// 無かったら終了
		if (!spHitTerrain) return;
		Math::Vector3 terrainPos = spHitTerrain->GetPos();
		// プレイヤーから回転床までの距離
		Math::Vector3 vec = m_pos - spHitTerrain->GetPos();
		// 回転床が回転する角度
		Math::Vector3 lotDegAng = spHitTerrain->GetParam().degAng;

		// Z軸回転の場合
		if (lotDegAng.z != 0)
		{
			vec.z = 0;
			float length = vec.Length();
			// 移動する前の回転床から見たプレイヤーの角度
			float degAng = DirectX::XMConvertToDegrees(atan2(vec.x, vec.y));

			if (degAng <= 90 && degAng >= -90)
			{
				degAng -= 90;
				if (degAng < 0)
				{
					degAng += 360;
				}
				degAng = 360 - degAng;

				degAng += lotDegAng.z;
				m_pos.x = spHitTerrain->GetPos().x + length * cos(DirectX::XMConvertToRadians(degAng));
				m_pos.y = spHitTerrain->GetPos().y + length * sin(DirectX::XMConvertToRadians(degAng));
			}
		}
		// Y軸回転の場合
		else if (lotDegAng.y != 0)
		{
			vec = m_pos - spHitTerrain->GetPos();
			vec.y = 0;
			float length = vec.Length();
			// 移動する前の回転床から見たプレイヤーの角度
			float degAng = DirectX::XMConvertToDegrees(atan2(vec.x, vec.z));

			//if (degAng <= 90 && degAng >= -90)
			{
				degAng -= 90;
				if (degAng < 0)
				{
					degAng += 360;
				}
				if (degAng >= 360)
				{
					degAng -= 360;
				}
				degAng = 360 - degAng;

				// 回転床が回転する角度
				degAng -= lotDegAng.y;
				m_pos.x = spHitTerrain->GetPos().x + length * cos(DirectX::XMConvertToRadians(degAng));
				m_pos.z = spHitTerrain->GetPos().z + length * sin(DirectX::XMConvertToRadians(degAng));

				// プレイヤーの角度を変える
				m_angle += lotDegAng.y;
				// 回転行列
				m_rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));
			}
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
					// 動く床の場合
				case ObjectType::MoveGround:
				{
					// 動く床の動く前の逆行列
					Math::Matrix inverseMatrix = m_carryObjectHitTerrain.transMat.Invert();
					// 動く床から見たプレイヤーの座標行列
					Math::Matrix playerMat = Math::Matrix::CreateTranslation(m_pos) * inverseMatrix;

					// 動く床の動いた後の行列
					Math::Matrix afterMoveGroundMat = Math::Matrix::CreateTranslation(spHitTerrain->GetMatrix().Translation());

					// 座標を確定
					m_pos = afterMoveGroundMat.Translation() + playerMat.Translation();
					break;
				}
				// 回る床の場合(Z軸)
				case ObjectType::RotationGround:
				{
					Math::Vector3 terrainPos = spHitTerrain->GetPos();
					// プレイヤーから回転床までの距離
					Math::Vector3 vec = m_pos - spHitTerrain->GetPos();
					// 回転床が回転する角度
					Math::Vector3 lotDegAng = spHitTerrain->GetParam().degAng;

					// Z軸回転の場合
					if (lotDegAng.z != 0)
					{
						vec.z = 0;
						float length = vec.Length();
						// 移動する前の回転床から見たプレイヤーの角度
						float degAng = DirectX::XMConvertToDegrees(atan2(vec.x, vec.y));

						if (degAng <= 90 && degAng >= -90)
						{
							degAng -= 90;
							if (degAng < 0)
							{
								degAng += 360;
							}
							degAng = 360 - degAng;

							degAng += lotDegAng.z;
							m_pos.x = spHitTerrain->GetPos().x + length * cos(DirectX::XMConvertToRadians(degAng));
							m_pos.y = spHitTerrain->GetPos().y + length * sin(DirectX::XMConvertToRadians(degAng));
						}
					}
					break;
				}
				case ObjectType::Propeller:
				{
					Math::Vector3 terrainPos = spHitTerrain->GetPos();
					// プレイヤーから回転床までの距離
					Math::Vector3 vec = m_pos - spHitTerrain->GetPos();
					// 回転床が回転する角度
					Math::Vector3 lotDegAng = spHitTerrain->GetParam().degAng;
					// Y軸回転の場合
					if (lotDegAng.y != 0)
					{
						vec = m_pos - spHitTerrain->GetPos();
						vec.y = 0;
						float length = vec.Length();
						// 移動する前の回転床から見たプレイヤーの角度
						float degAng = DirectX::XMConvertToDegrees(atan2(vec.x, vec.z));

						//if (degAng <= 90 && degAng >= -90)
						{
							degAng -= 90;
							if (degAng < 0)
							{
								degAng += 360;
							}
							if (degAng >= 360)
							{
								degAng -= 360;
							}
							degAng = 360 - degAng;

							// 回転床が回転する角度
							degAng -= lotDegAng.y;
							m_pos.x = spHitTerrain->GetPos().x + length * cos(DirectX::XMConvertToRadians(degAng));
							m_pos.z = spHitTerrain->GetPos().z + length * sin(DirectX::XMConvertToRadians(degAng));

							// プレイヤーの角度を変える
							m_angle += lotDegAng.y;
							// 回転行列
							m_rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));
						}
					}
				}
			}
		}
	}
	// 運べるオブジェクトに乗っている時の処理(地面に触れていない場合)
	else if (m_carryObject.hitFlg)
	{
		// 動く前の逆行列
		Math::Matrix inverseMatrix = m_carryObject.transMat.Invert();
		// 動く床から見たプレイヤーの座標行列
		Math::Matrix playerMat = Math::Matrix::CreateTranslation(m_pos) * inverseMatrix;

		// 動く床の場合
		std::shared_ptr<KdGameObject> spHitObject = m_wpHitCarryObject.lock();
		if (spHitObject)
		{
			// 動く床の動いた後の行列
			Math::Matrix afterMoveGroundMat = Math::Matrix::CreateTranslation(spHitObject->GetMatrix().Translation());

			// 座標を確定
			m_pos = afterMoveGroundMat.Translation() + playerMat.Translation();
		}
	}

	transMat = Math::Matrix::CreateTranslation(m_pos);
	
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_scale);

	m_mWorld = scaleMat * m_rotMat * transMat;

	// アニメーションの更新
	// 止まっていたらアニメーションしない
	m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
	m_spModel->CalcNodeMatrices();
}

void Player::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void Player::DrawLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);

	}
}

void Player::Init()
{
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

	// オブジェクトのタイプ
	m_objectType = ObjectType::Player;

	m_baseObjectType = BaseObjectType::Player;

	m_pos = { 0, 0.25f, -5.0f };
	m_respawnPos = m_pos;

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	// 移動が出来なくなるフラグ
	m_stopFlg = true;

	// 音
	// 草の上を歩いた音
	m_wpWalkSound[WalkSoundType::Grass] = KdAudioManager::Instance().Play("Asset/Sounds/SE/grassWalk.wav", false);
	if (!m_wpWalkSound[WalkSoundType::Grass].expired())
	{
		m_wpWalkSound[WalkSoundType::Grass].lock()->SetVolume(0.15f);
		m_wpWalkSound[WalkSoundType::Grass].lock()->Stop();
	}
	m_nowWalkSoundFlg = false;
	// かたい地面を歩いた音
	m_wpWalkSound[WalkSoundType::Tile] = KdAudioManager::Instance().Play("Asset/Sounds/SE/tileWalk.wav", false);
	if (!m_wpWalkSound[WalkSoundType::Tile].expired())
	{
		m_wpWalkSound[WalkSoundType::Tile].lock()->SetVolume(0.15f);
		m_wpWalkSound[WalkSoundType::Tile].lock()->Stop();
	}

	// ジャンプ音
	m_jumpSound.wpSound = KdAudioManager::Instance().Play("Asset/Sounds/SE/jump.wav", false);
	if (!m_jumpSound.wpSound.expired())
	{
		m_jumpSound.wpSound.lock()->SetVolume(0.06f);
		m_jumpSound.wpSound.lock()->Stop();
	}
	m_jumpSound.flg = false;

	// 敵を踏んだ時の音
	m_stampSound.wpSound = KdAudioManager::Instance().Play("Asset/Sounds/SE/stamp.wav", false);
	if (!m_stampSound.wpSound.expired())
	{
		m_stampSound.wpSound.lock()->SetVolume(0.06f);
		m_stampSound.wpSound.lock()->Stop();
	}
	m_stampSound.flg = false;

	// キノコではねた時の音
	m_boundSound.wpSound = KdAudioManager::Instance().Play("Asset/Sounds/SE/bound.wav", false);
	if (!m_boundSound.wpSound.expired())
	{
		m_boundSound.wpSound.lock()->SetVolume(0.06f);
		m_boundSound.wpSound.lock()->Stop();
	}
	m_boundSound.flg = false;
}

void Player::Reset()
{
	CharacterBase::Reset();

	// 座標
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

	m_jumpSound.flg = false;
}

void Player::BackPos()
{
	m_pos.x = m_oldPos.x;
	m_pos.z = m_oldPos.z;
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
	// 当たったオブジェクトリストをリセット
	m_wpHitObjectList.clear();

	// 運べるオブジェクトとの当たり判定
	HitJudgeCarryObject();

	// 地面との当たり判定
	HitJudgeGround();

	if (m_stopFlg == false || (SceneManager::Instance().GetNowScene() == SceneManager::SceneType::StageSelect))
	{
		// 触れたらイベントが発生する
		HitJudgeEvent();

		// 敵との当たり判定
		HitJudgeEnemy();
	}
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
	m_wpHitTerrain.reset();

	// 地面とのレイ当たり判定
	if(m_carryObject.hitFlg == false)
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
		}

		// 当たっていた時の処理
		if (hitFlg)
		{
			// 当たったオブジェクト
			std::shared_ptr<TerrainBase> spHitObject = m_wpHitTerrain.lock();

			// 音のフラグをリセットする
			m_boundSound.flg = false;	// きのこで跳ねた時の音

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
					if (m_boundSound.flg == false)
					{
						if (!m_boundSound.wpSound.expired())
						{
							m_boundSound.wpSound.lock()->Play();
							m_boundSound.flg = true;
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
		const float radius = 0.35f;

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
		boxInfo.m_Abox.Extents = { radius, charaHighLength / 2, radius };
		// 当たり判定をするタイプ
		boxInfo.m_type |= KdCollider::TypeGround;

		// 当たり判定の結果格納リスト
		std::list<KdCollider::CollisionResult> resultList;

		bool hitFlg = false;

		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			if (obj->Intersects(boxInfo, nullptr))
			{
				hitFlg = true;
				break;
			}
		}

		if (hitFlg == false) return;

		// スフィアの情報
		KdCollider::SphereInfo sphereInfo;
		// スフィアの中心座標
		sphereInfo.m_sphere.Center = m_pos;
		// スフィアの半径
		sphereInfo.m_sphere.Radius = radius;
		sphereInfo.m_sphere.Center.y += sphereInfo.m_sphere.Radius + 0.1f;
		// スフィアのタイプ
		sphereInfo.m_type = KdCollider::TypeGround;

		// 当たったかどうかのフラグ
		hitFlg = false;
		// 当たった結果
		KdCollider::CollisionResult collisionResult;
		// 複数に当たったかどうかのフラグ
		bool multiHitFlg = false;

		hitFlg = SphereHitJudge(sphereInfo, collisionResult, multiHitFlg);
		if (hitFlg == false)
		{
			sphereInfo.m_sphere.Center.y += 0.5f;
			hitFlg = SphereHitJudge(sphereInfo, collisionResult, multiHitFlg);
		}
		if (hitFlg == false)
		{
			sphereInfo.m_sphere.Center.y += 0.5f;
			hitFlg = SphereHitJudge(sphereInfo, collisionResult, multiHitFlg);
		}
		if (hitFlg == false)
		{
			sphereInfo.m_sphere.Center.y += 0.5f;
			hitFlg = SphereHitJudge(sphereInfo, collisionResult, multiHitFlg);
		}

		// 複数のオブジェクトに当たっていた場合
		if (multiHitFlg == true)
		{
			// Y座標以外、更新前の座標に戻す
			m_pos.x = m_oldPos.x;
			m_pos.z = m_oldPos.z;
		}
		// 一つのオブジェクトに当たった場合
		else if (hitFlg)
		{
			// Y軸の補正はなし
			collisionResult.m_hitDir.y = 0;
			collisionResult.m_hitDir.Normalize();
			m_pos += collisionResult.m_hitDir * collisionResult.m_overlapDistance;
		}

		hitFlg = false;
		sphereInfo.m_sphere.Center = m_pos;
		sphereInfo.m_sphere.Radius -= 0.05f;
		sphereInfo.m_sphere.Center.y += 2.2f;
		hitFlg = SphereHitJudge(sphereInfo, collisionResult, multiHitFlg);
		// 複数のオブジェクトに当たっていた場合
		if (multiHitFlg == true)
		{
			// Y座標のみ、更新前の座標に戻す
			m_pos.y = m_oldPos.y;
		}
		// 一つのオブジェクトに当たった場合
		else if (hitFlg == true)
		{
			m_pos.y -= collisionResult.m_overlapDistance;
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
					m_goalFlg = true;
					m_stopFlg = true;

					if (m_goalSEFlg == false)
					{
						std::shared_ptr<KdSoundInstance> se = KdAudioManager::Instance().Play("Asset/Sounds/SE/goal.wav", false);
						se->SetVolume(0.06f);
						m_goalSEFlg = true;
					}
					break;
				}

					// セーブポイント
				case ObjectType::SavePoint:
					m_respawnPos = spHitObject->GetPos();
					m_respawnPos.z -= 1.0f;
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
					if (GetAsyncKeyState('F') & 0x8000)
					{
						m_pos = spHitObject->GetPos();
					}
					if (m_wpGameUI.expired() == false)
					{
						m_wpGameUI.lock()->SetDrawType(GameUI::DrawType::Warp);
					}
					break;

					// 最終ゴール
				case ObjectType::FinalGoal:
					m_stopFlg = true;
					m_finalGoalFlg = true;
					break;
				}
			}
		}
	}


	std::shared_ptr<KdGameObject> spHitObject = nullptr;
	// ステージセレクトのオブジェクトとの視野角判定
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		if (obj->GetObjectType() == ObjectType::StageSelectObject)
		{
			if (SceneManager::Instance().GetDebug() == true)
			{
				m_pDebugWire->AddDebugSphere(obj->GetPos(), 4.0f);
			}

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
			}
		}
	}
	// もし当たっていたら一番近いオブジェクトだけ処理をする
	if (spHitObject)
	{
		spHitObject->OnHit();
		if (GetAsyncKeyState('F') & 0x8000)
		{
			if (m_actionKeyFlg == false)
			{
				m_stopFlg = true;
				m_beginGameSceneFlg = true;

				// キーフラグ
				m_actionKeyFlg = true;
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
			m_stampSound.flg = false;	// 敵を踏んだ時の音
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
					if (m_stampSound.flg == false)
					{
						if (!m_stampSound.wpSound.expired())
						{
							m_stampSound.wpSound.lock()->Play();
							m_stampSound.flg = true;
						}
					}
				}
				// 下半分なら死亡
				else
				{
					m_aliveFlg = false;
				}
			}
		}
	}
}

void Player::HitJudgeCarryObject()
{
	m_wpHitCarryObject.reset();
	// 当たったオブジェクト系をリセット
	m_carryObjectHitTerrain.hitFlg = false;
	m_carryObjectHitTerrain.transMat = Math::Matrix::Identity;

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
				obj.lock()->SetSelectWhite(true);
				if (GetAsyncKeyState('F') & 0x8000)
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
							m_situationType |= SituationType::Carry;
							// 走り状態を解除
							m_situationType &= (~SituationType::Run);
							// キーフラグ
							m_actionKeyFlg = true;
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

void Player::DataLoad()
{
	// JSONファイルを読み込む
	std::ifstream file(m_path.data());
	if (!file.is_open()) return;

	nlohmann::json data;
	file >> data;

	// JSONデータを格納していく
	for (const auto& objData : data["GameObject"])
	{
		// 走る速度
		m_runSpeed = objData["m_runSpeed"];
		// 歩く速度
		m_walkSpeed = objData["m_walkSpeed"];
		// ジャンプ力
		m_jumpPow	= objData["m_jumpPow"];
		// 跳ねる床を踏んだ時のジャンプ力
		m_boundJumpPow = objData["m_boundJumpPow"];
		// 敵を踏んだ時のジャンプ力
		m_enemyJumpPow = objData["m_enemyJumpPow"];
	}
}

void Player::DataSave()
{
	nlohmann::json objData;

	// リストごと
	nlohmann::json objStat;
	objStat["m_runSpeed"]		= m_runSpeed;
	objStat["m_walkSpeed"]		= m_walkSpeed;
	objStat["m_jumpPow"]		= m_jumpPow;
	objStat["m_boundJumpPow"]	= m_boundJumpPow;
	objStat["m_enemyJumpPow"]	= m_enemyJumpPow;
	objStat["name"] = m_name.data();

	// ゲームオブジェクトに追加
	objData["GameObject"][m_name.data()] = objStat;

	// ファイルに書き込む
	std::ofstream file(m_path.data());
	if (!file.is_open()) return;

	// JSONデータをファイルに書き込む
	file << std::setw(4) << objData << std::endl;	//Pretty print with 4-space indent
	file.close();
}

void Player::GoalProcess()
{
	m_goalStayCount++;
	if (m_goalStayCount > m_goalStayTime)
	{
		m_pos.x = m_goalPos.x;
		m_pos.z = m_goalPos.z;
		m_angle = 180;
		m_rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));

		if (m_goalBGMFlg == false)
		{
			std::shared_ptr<KdSoundInstance> se = KdAudioManager::Instance().Play("Asset/Sounds/BGM/clear.wav", false);
			se->SetVolume(0.06f);
		}
		m_goalBGMFlg = true;
	}
}