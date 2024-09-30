#include "Player.h"
#include "../../../Scene/SceneManager.h"
#include "../../Camera/CameraBase.h"
#include "../../Terrain/TerrainBase.h"
#include "../../../Tool/ObjectController/TerrainController/TerrainController.h"

#include "../../Effect/PlayerSmoke/PlayerSmoke.h"

#include "../../../Tool/ObjectController/CarryObjectController/CarryObjectController.h"
#include "../../Terrain/CarryObject/CarryObjectBase.h"
#include "../../../Tool/ObjectController/EnemyController/EnemyController.h"
#include "../Enemy/EnemyBase.h"

#include "../../../main.h"

void Player::Update()
{
	// デバッグモード中は更新しない
	if (SceneManager::Instance().GetDebug()) return;
	// ポーズ画面中は更新しない
	if (m_pauseFlg == true) return;

	// 移動前の座標を保存
	m_oldPos = m_pos;

	// 更新前のプレイヤーの状態を保持
	UINT oldSituationType = m_situationType;

	// Idle以外のフラグが立っていたらStopをおろす
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

	{
		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			if (flg[0] == false)
			{
				m_moveVec.z += 1.0f;
				m_situationType |= SituationType::Walk;
				flg[0] = true;
			}
		}
		else
		{
			flg[0] = false;
		}
		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			if (flg[1] == false)
			{
				m_moveVec.x -= 1.0f;
				m_situationType |= SituationType::Walk;
				flg[1] = true;
			}
		}
		else
		{
			flg[1] = false;
		}
		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			if (flg[2] == false)
			{
				m_moveVec.z -= 1.0f;
				m_situationType |= SituationType::Walk;
				flg[2] = true;
			}
		}
		else
		{
			flg[2] = false;
		}
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			if (flg[3] == false)
			{
				m_moveVec.x += 1.0f;
				m_situationType |= SituationType::Walk;
				flg[3] = true;
			}
		}
		else
		{
			flg[3] = false;
		}
	}

	if (m_stopFlg == false)
	{
		// WASDで移動
		if (GetAsyncKeyState('W') & 0x8000)
		{
			m_moveVec.z += 1.0f;
			m_situationType |= SituationType::Walk;
		}
		if (GetAsyncKeyState('A') & 0x8000)
		{
			m_moveVec.x -= 1.0f;
			m_situationType |= SituationType::Walk;
		}
		if (GetAsyncKeyState('S') & 0x8000)
		{
			m_moveVec.z -= 1.0f;
			m_situationType |= SituationType::Walk;
		}
		if (GetAsyncKeyState('D') & 0x8000)
		{
			m_moveVec.x += 1.0f;
			m_situationType |= SituationType::Walk;
		}

		// SHIFTキーを押すと、ダッシュになる
		// 運んでいる状態ならダッシュをできないようにする
		if ((m_situationType & SituationType::Carry) == 0)
		{
			if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
			{
				// 空中でないなら走り状態に移行できる
				if ((m_situationType & SituationType::Air) == 0)
				{
					m_situationType |= SituationType::Run;
				}
				// 走り状態なら歩き状態を解除する
				if (m_situationType & SituationType::Run)
				{
					m_situationType &= (~SituationType::Walk);
				}
			}
			else
			{
				m_situationType &= (~SituationType::Run);
			}
		}
		// 運んでいる状態なら走り状態を解除する
		else
		{
			m_situationType &= (~SituationType::Run);
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
		// 走り状態の時
		if (m_lastGroundSituationType & SituationType::Run)
		{
			m_situationType |= SituationType::Run;
			m_situationType &= (~SituationType::Walk);
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
				std::shared_ptr<PlayerSmoke> smoke = std::make_shared<PlayerSmoke>();
				smoke->Init();
				smoke->SetPos(m_pos);
				smoke->SetSmokeType(PlayerSmoke::SmokeType::WalkSmoke);
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
				std::shared_ptr<PlayerSmoke> smoke = std::make_shared<PlayerSmoke>();
				smoke->Init();
				smoke->SetPos(m_pos);
				smoke->SetSmokeType(PlayerSmoke::SmokeType::WalkSmoke);
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
				//if (!(m_situationType & SituationType::Air))
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
		m_pos += m_moveVec * m_moveSpeed;
	}
	// 走り
	else if (m_situationType & SituationType::Run)
	{
		m_pos += m_moveVec * m_runSpeed;
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
	if (m_pos.y < -15.0f)
	{
		m_aliveFlg = false;
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

	//Application::Instance().m_log.Clear();
	//Application::Instance().m_log.AddLog("x:%.4f y:%.4f z:%.4f", m_pos.x, m_pos.y, m_pos.z);

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

	// ダメージを受けた時の処理
	if (m_damage.nowDamageFlg == true)
	{
		DamageProcess();
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

	// 運び状態を解除する
	if (GetAsyncKeyState('F') & 0x8000)
	{
		if (m_situationType & SituationType::Carry)
		{
			if (m_carryKeyFlg == false)
			{
				if (m_wpCarryObject.expired() == false)
				{
					// 運ばれている状態にする
					m_wpCarryObject.lock()->CarryFlg(false);
					m_wpCarryObject.reset();
				}
				// 状態の切り替え
				m_situationType &= (~SituationType::Carry);
				// キーフラグ
				m_carryKeyFlg = true;
			}
		}
	}
	else
	{
		m_carryKeyFlg = false;
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
					// もしジャンプをすでにしている場合変えない
					if ((oldSituationType & SituationType::Jump) == 0)
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
			m_walkSoundType = WalkSoundType::Grass;
			break;

		// コツコツ見たいな音
		case ObjectType::FenceBar:
		case ObjectType::FencePillar:
		case ObjectType::MoveGround:
		case ObjectType::DropGround:
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

	Application::Instance().m_log.AddLog("%.2f, %.2f, %.2f\n", m_pos.x, m_pos.y, m_pos.z);
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
		if (m_damage.drawFlg || m_aliveFlg == false)
		{
			KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
		}
	}
}

void Player::DrawLit()
{
	if (m_spModel)
	{
		if (m_damage.drawFlg || m_aliveFlg == false)
		{
			KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
		}
	}
}

void Player::Init()
{
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

	// 移動速度
	m_moveSpeed = 0.06f;

	m_pos = { 0, 0.25f, -5.0f };
	m_respawnPos = m_pos;

	// ジャンプ力
	m_jumpPow = 0.125f;

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	// 移動が出来なくなるフラグ
	m_stopFlg = true;
	
	// ライフ
	m_life = m_maxLife;














	// 音
	// 草の上を歩いた音
	m_wpWalkSound[WalkSoundType::Grass] = KdAudioManager::Instance().Play("Asset/Sounds/SE/grassWalk.wav", false);
	if (!m_wpWalkSound[WalkSoundType::Grass].expired())
	{
		m_wpWalkSound[WalkSoundType::Grass].lock()->SetVolume(0.1f);
		m_wpWalkSound[WalkSoundType::Grass].lock()->Stop();
	}
	m_nowWalkSoundFlg = false;
	// かたい地面を歩いた音
	m_wpWalkSound[WalkSoundType::Tile] = KdAudioManager::Instance().Play("Asset/Sounds/SE/tileWalk.wav", false);
	if (!m_wpWalkSound[WalkSoundType::Tile].expired())
	{
		m_wpWalkSound[WalkSoundType::Tile].lock()->SetVolume(0.1f);
		m_wpWalkSound[WalkSoundType::Tile].lock()->Stop();
	}

	// ジャンプ音
	m_jumpSound.wpSound = KdAudioManager::Instance().Play("Asset/Sounds/SE/jump.wav", false);
	if (!m_jumpSound.wpSound.expired())
	{
		m_jumpSound.wpSound.lock()->SetVolume(0.02f);
		m_jumpSound.wpSound.lock()->Stop();
	}
	m_jumpSound.flg = false;

	// 敵を踏んだ時の音
	m_stampSound.wpSound = KdAudioManager::Instance().Play("Asset/Sounds/SE/stamp.wav", false);
	if (!m_stampSound.wpSound.expired())
	{
		m_stampSound.wpSound.lock()->SetVolume(0.02f);
		m_stampSound.wpSound.lock()->Stop();
	}
	m_stampSound.flg = false;

	// キノコではねた時の音
	m_boundSound.wpSound = KdAudioManager::Instance().Play("Asset/Sounds/SE/bound.wav", false);
	if (!m_boundSound.wpSound.expired())
	{
		m_boundSound.wpSound.lock()->SetVolume(0.02f);
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

	// ライフ
	m_life = m_maxLife;

	// ダメージ受けた時の変数をリセット
	m_damage.nowDamageFlg = false;
	m_damage.damageCount = 0;
	m_damage.drawFlg = true;

	// アニメーションをIdleに
	SetAnimation("Idle", true);

	// 煙カウント
	m_smokeCount = 0;
}

void Player::BackPos()
{
	m_pos.x = m_oldPos.x;
	m_pos.z = m_oldPos.z;
}

const bool Player::IsCameraTracking() const
{
	// 当たった地面がある時
	if (m_wpHitTerrain.expired() == false)
	{
		// チェック
		return m_wpHitTerrain.lock()->IsCameraTracking();
	}
	
	// 当たった運ぶオブジェクトがある時
	if (m_wpHitCarryObject.expired() == false)
	{
		// チェック
		return m_wpHitCarryObject.lock()->IsCameraTracking();
	}

	return false;
}

// 当たり判定
void Player::HitJudge()
{
	// 当たったオブジェクトリストをリセット
	m_wpHitObjectList.clear();

	// 地面との当たり判定
	HitJudgeGround();

	if (m_stopFlg == false)
	{
		// 触れたらイベントが発生する
		HitJudgeEvent();

		// 敵との当たり判定
		HitJudgeEnemy();

		// 運べるオブジェクトとの当たり判定
		HitJudgeCarryObject();
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
		rayInfo.m_range = m_gravity + 1.0f + m_enableStepHeight;
		// レイの座標(キャラの中心)
		rayInfo.m_pos.y = m_enableStepHeight + 1.0f;
		rayInfo.m_pos += m_pos;
		// レイの方向
		rayInfo.m_dir = Math::Vector3::Down;
		// レイのタイプ
		rayInfo.m_type = KdCollider::TypeGround;

		// レイ判定
		hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitTerrain, true);

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
					m_gravity = -0.25f;

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
	{
		// スフィアの情報
		KdCollider::SphereInfo sphereInfo;
		// スフィアの中心座標
		sphereInfo.m_sphere.Center = m_pos;
		// スフィアの半径
		sphereInfo.m_sphere.Radius = 0.25f;
		sphereInfo.m_sphere.Center.y += sphereInfo.m_sphere.Radius + 0.1f;
		// スフィアのタイプ
		sphereInfo.m_type = KdCollider::TypeGround;

		// 当たったかどうかのフラグ
		bool hitFlg = false;
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
			//hitFlg = SphereHitJudge(sphereInfo, collisionResult, multiHitFlg, true);
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
		sphereInfo.m_sphere.Center.y = m_pos.y + 2.0f;
		hitFlg = SphereHitJudge(sphereInfo, collisionResult, multiHitFlg, true);
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

	// ボックス判定
	{
		//// ボックス判定
		//KdCollider::BoxInfo boxInfo;
		//// 右前上のノード
		//Math::Vector3 rightBackUpPos = m_spModel->FindNode("RightBackUp")->m_worldTransform.Translation();
		//// 左後上のノード
		//Math::Vector3 leftFrontUpPos = m_spModel->FindNode("LeftFrontUp")->m_worldTransform.Translation();
		//// キャラクターの高さ
		//float charaHighLength = rightBackUpPos.y;
		//// 中心座標(キャラクターの真ん中)
		//boxInfo.m_Abox.Center = m_pos;
		//boxInfo.m_Abox.Center.y += charaHighLength / 2.0f + 0.1f;
		//// キャラクターの横幅(半径)
		//float charaSideRadius = abs(rightBackUpPos.x);
		//// キャラクターの奥行幅(直径)
		//float charaZLength = abs(rightBackUpPos.z - leftFrontUpPos.z);
		//// ボックスのサイズ
		////boxInfo.m_Abox.Extents = { 5, 5, 5 };
		//boxInfo.m_Abox.Extents = { charaSideRadius, charaHighLength / 2, charaZLength / 2 };
		//// 当たり判定をするタイプ
		//boxInfo.m_type |= KdCollider::TypeGround;

		////m_pDebugWire->AddDebugSphere(rightBackUpPos + m_pos, 0.1f);
		////m_pDebugWire->AddDebugSphere(leftFrontUpPos + m_pos, 0.1f);
		//m_pDebugWire->AddDebugBox(Math::Matrix::CreateTranslation(boxInfo.m_Abox.Center), boxInfo.m_Abox.Extents);

		//// 当たり判定の結果格納リスト
		//std::list<KdCollider::CollisionResult> resultList;

		//for (auto& obj : m_wpTerrainController.lock()->GetObjList())
		//{
		//	if (obj.expired() == false)
		//	{
		//		if (obj.lock()->Intersects(boxInfo, &resultList))
		//		{
		//			m_wpHitObjectList.push_back(obj);
		//		}
		//	}
		//}

		//float maxOverLap = 0;
		//Math::Vector3 hitDir;
		//bool hitFlg = false;

		//for (auto& result : resultList)
		//{
		//	if (result.m_overlapDistance > maxOverLap)
		//	{
		//		maxOverLap = result.m_overlapDistance;
		//		hitDir = result.m_hitDir;
		//		hitFlg = true;
		//	}
		//}

		//Application::Instance().m_log.AddLog("\nmaxOverLap:%.4f\n", maxOverLap);
		//Application::Instance().m_log.AddLog("hitDir.x:%.4f y:%.4f z:%.4f", hitDir.x, hitDir.y, hitDir.z);

		//if (hitFlg)
		//{
		//	hitDir.y = 0;
		//	hitDir.Normalize();

		//	m_pos += hitDir * maxOverLap;
		//}
	}
}

void Player::HitJudgeEvent()
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
		boxInfo.m_type |= KdCollider::TypeEvent;


		// 当たり判定の結果格納リスト
		std::list<KdCollider::CollisionResult> resultList;

		bool hitFlg = false;

		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			if (obj->Intersects(boxInfo, &resultList))
			{
				m_wpHitObjectList.push_back(obj);
				hitFlg = true;
			}
		}

		if (hitFlg)
		{
			// 当たったオブジェクト
			std::shared_ptr<KdGameObject> spHitObject;

			// 探す
			for (auto& hitObject : m_wpHitObjectList)
			{
				if (!hitObject.expired())
				{
					if (hitObject.lock()->GetBaseObjectType() == BaseObjectType::Event)
					{
						spHitObject = hitObject.lock();
						break;
					}
				}
			}
			if (spHitObject)
			{
				spHitObject->OnHit();
				switch (spHitObject->GetObjectType())
				{
					// ゴール
				case ObjectType::Goal:
					m_goalFlg = true;
					m_stopFlg = true;
					break;

					// 回復アイテム
				case ObjectType::HealItem:
					m_life += 1;
					// 回復エフェクト
					if (!m_effectFlg)
					{
						//m_effectFlg = true;
						m_wpEffekseer = KdEffekseerManager::GetInstance().Play("Heal/heal.efkefc", m_pos, { 0.0f, 0.0f, 0 }, 0.3f, 1.0f, false);
					}
					break;

					// セーブポイント
				case ObjectType::SavePoint:
					m_respawnPos = spHitObject->GetPos();
					break;
				}
			}
		}
	}
}

void Player::HitJudgeEnemy()
{
	// スフィアの情報
	//KdCollider::SphereInfo sphereInfo;
	//// スフィアの中心座標
	//sphereInfo.m_sphere.Center = m_pos;
	//// スフィアの半径
	//sphereInfo.m_sphere.Radius = 0.25f;
	//sphereInfo.m_sphere.Center.y += sphereInfo.m_sphere.Radius;
	//// スフィアのタイプ
	//sphereInfo.m_type = KdCollider::TypeDamage;

	//// 当たったかどうかのフラグ
	//bool hitFlg = false;

	//hitFlg = SphereHitJudge(sphereInfo, true);

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

		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			if (obj->Intersects(boxInfo, &resultList))
			{
				m_wpHitObjectList.push_back(obj);
				hitFlg = true;
			}
		}

		// 当たっていた時の処理
		if (hitFlg)
		{
			// 当たったオブジェクト
			std::shared_ptr<KdGameObject> spHitObject;

			// 音のフラグをリセット
			m_stampSound.flg = false;	// 敵を踏んだ時の音

			// 敵を探す
			for (auto& hitObject : m_wpHitObjectList)
			{
				if (!hitObject.expired())
				{
					if (hitObject.lock()->GetBaseObjectType() == BaseObjectType::Enemy)
					{
						spHitObject = hitObject.lock();
						break;
					}
				}
			}

			if (spHitObject)
			{
				// 敵の座標
				Math::Vector3 enemyPos = spHitObject->GetPos();
				enemyPos.y += 0.5f;
				// 敵からプレイヤーのベクトル
				Math::Vector3 vec = m_pos - enemyPos;
				vec.z = 0;
				// 敵から見たプレイヤーの角度
				float degAng = DirectX::XMConvertToDegrees(atan2(vec.x, vec.y));
				// 上半分なら倒す
				if (degAng < 90 && degAng > -90)
				{
					// 煙を生み出す
					std::shared_ptr<PlayerSmoke> smoke = std::make_shared<PlayerSmoke>();
					smoke->Init();
					smoke->SetPos(m_pos);
					smoke->SetSmokeType(PlayerSmoke::SmokeType::StampSmoke);
					SceneManager::Instance().AddObject(smoke);

					// 敵を踏んだ時の処理
					spHitObject->OnHit();
					m_gravity = -0.15f;

					// もしジャンプ状態ならジャンプのアニメーションをする
					if (m_situationType & SituationType::Jump)
					{
						//m_spAnimator->SetAnimation(m_spModel->GetAnimation("Jump"), false);
					}
					// 違うならジャンプ状態にする
					else
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
				// 下半分ならダメージを受ける
				else
				{
					if (m_damage.nowDamageFlg == false)
					{
						// ダメージ処理をする
						m_damage.nowDamageFlg = true;
						// ライフを減らす
						m_life -= 1;
						// ライフが０以下になったら死亡
						if (m_life <= 0)
						{
							m_aliveFlg = false;
						}
					}
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
	rayInfo.m_range = m_gravity + 1.0f + m_enableStepHeight;
	// レイの座標(キャラの中心)
	rayInfo.m_pos.y = m_enableStepHeight + 1.0f;
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
					m_moveGround.transMat = Math::Matrix::CreateTranslation(spHitObject->GetPos());
					// 動く床に当たったかどうかのフラグ
					m_moveGround.hitFlg = true;
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
						if (m_carryKeyFlg == false)
						{
							// 運ばれている状態にする
							obj.lock()->CarryFlg(true);
							// オブジェクトを保持
							m_wpCarryObject = obj;
							// 状態の切り替え
							m_situationType |= SituationType::Carry;
							// キーフラグ
							m_carryKeyFlg = true;
						}
					}
				}
				else
				{
					m_carryKeyFlg == false;
				}
			}
		}
	}
}

void Player::DamageProcess()
{
	// 生きている時だけ
	if (m_aliveFlg == true)
	{
		m_damage.damageCount++;
		if (m_damage.damageCount % m_damage.blinkingTime == 0)
		{
			m_damage.drawFlg = !m_damage.drawFlg;
		}

		if (m_damage.damageCount > m_damage.damageTime)
		{
			m_damage.nowDamageFlg = false;
			m_damage.damageCount = 0;
			m_damage.drawFlg = true;
		}
	}
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
		//m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("Goal"), true);
	}
}