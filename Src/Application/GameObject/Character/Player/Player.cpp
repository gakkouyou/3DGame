﻿#include "Player.h"
#include "../../../Scene/SceneManager.h"
#include "../../Camera/CameraBase.h"
#include "../../Terrain/TerrainBase.h"

#include "../../Effect/PlayerSmoke/PlayerSmoke.h"

#include "../../../main.h"

void Player::Update()
{
	// デバッグモード中は更新しない
	if (SceneManager::Instance().GetDebug()) return;
	// ポーズ画面中は更新しない
	if (m_pauseFlg == true) return;

	// Stop以外のフラグが立っていたらStopをおろす
	if ((m_situationType & (~SituationType::Stop)) && (m_situationType & SituationType::Stop))
	{
		m_situationType &= (~SituationType::Stop);
	}
	// 何もフラグが立っていなかったらStopにする
	if (m_situationType == 0)
	{
		m_situationType = SituationType::Stop;
	}

	// 移動初期化
	m_moveVec = Math::Vector3::Zero;

	// 更新前のプレイヤーの状態を保持
	UINT oldSituationType = m_situationType;

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

		// 歩き状態の時にSHIFTキーを押すと、ダッシュになる
		if (m_situationType & SituationType::Walk)
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
	}

	// カメラの向きで移動方向を補正
	m_moveVec = m_moveVec.TransformNormal(m_moveVec, m_wpCamera.lock()->GetRotationYMatrix());
	m_moveVec.Normalize();

	// 動いていなかったら止まっている状態にする。
	if (m_moveVec.LengthSquared() == 0)
	{
		m_situationType &= (~SituationType::Walk);
		m_situationType &= (~SituationType::Run);
	}

	// 空中にいなくて動いている時、足元に煙を出す
	if ((m_situationType & SituationType::Air) == 0)
	{
		// 歩いている時
		if (m_situationType & SituationType::Walk)
		{
			if (m_smokeCount % m_walkSmokeTime == 0)
			{
				std::shared_ptr<PlayerSmoke> smoke = std::make_shared<PlayerSmoke>();
				smoke->Init();
				smoke->SetPos(m_pos);
				SceneManager::Instance().AddObject(smoke);
				m_smokeCount = 0;
			}
			m_smokeCount++;
		}
		// 走っている時
		else if (m_situationType & SituationType::Run)
		{
			if (m_smokeCount % m_runSmokeTime == 0)
			{
				std::shared_ptr<PlayerSmoke> smoke = std::make_shared<PlayerSmoke>();
				smoke->Init();
				smoke->SetPos(m_pos);
				SceneManager::Instance().AddObject(smoke);
				m_smokeCount = 0;
			}
			m_smokeCount++;
		}
	}
	else
	{
		m_smokeCount = 0;
	}

	if (m_stopFlg == false)
	{
		// ジャンプ
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			if (!(m_situationType & SituationType::Air))
			{
				m_situationType |= SituationType::Jump;
				m_gravity = -m_jumpPow;
				m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("Jump"), false);
			}
		}
	}

	if (m_situationType != oldSituationType)
	{
		// 止まっている状態→歩きor走り
		if ((m_situationType & SituationType::Walk) || (m_situationType & SituationType::Run))
		{
			if (oldSituationType & SituationType::Stop)
			{
				// 空中ではアニメーションしない
				if (!(m_situationType & SituationType::Air))
				{
					m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("RightWalk"), false);
				}
			}
		}
	}

	if (m_spAnimator->IsAnimationEnd())
	{
		// 歩いている状態or走っている状態の時
		// 空中にいたらアニメーションしない
		if (((m_situationType & SituationType::Walk) || (m_situationType & SituationType::Run)) && !(m_situationType & SituationType::Air))
		{
			// 右足を出す
			if (m_walkAnimeDirFlg == true)
			{
				m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("RightWalk"), false);
				m_walkAnimeDirFlg = false;
			}
			// 左足を出す
			else
			{
				m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("LeftWalk"), false);
				m_walkAnimeDirFlg = true;
			}
		}
		else
		{
			// 歩いていなかったら歩きアニメーションの方向フラグをfalseにする
			m_walkAnimeDirFlg = false;
		}

		// ジャンプ状態の時
		if (m_situationType & SituationType::Jump)
		{
			m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("Jump"), false);
		}

		// アニメーションが終わった時に止まっていたら止まっているアニメーションをする
		if (m_situationType & SituationType::Stop)
		{
			m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("Stop"), true);
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
	if (m_pos.y < -60.0f)
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

	//if (!m_effectFlg)
	//{
	//	m_effectFlg = true;
	//	KdEffekseerManager::GetInstance().Play("test.efkefc", { 0, 3, 0 }, { 90.0f, 90.0f, 0 }, 0.1f, 0.5f, true);
	//}

	// 当たり判定
	HitJudge();

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

		// 動く床を探す
		std::shared_ptr<TerrainBase> spHitObject	= m_wpHitTerrain.lock();

		if (spHitObject)
		{
			// 動く床の動いた後の行列
			Math::Matrix afterMoveGroundMat = Math::Matrix::CreateTranslation(spHitObject->GetMatrix().Translation());

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
		vec.z = 0;
		float length = vec.Length();
		// 移動する前の回転床から見たプレイヤーの角度
		float degAng = DirectX::XMConvertToDegrees(atan2(vec.x, vec.y));
		degAng -= 90;
		if (degAng < 0)
		{
			degAng += 360;
		}
		degAng = 360 - degAng;

		// 回転床が回転する角度
		float lotDegAng = spHitTerrain->GetParam().degAng.z;
		degAng += lotDegAng;
		m_pos.x = spHitTerrain->GetPos().x + length * cos(DirectX::XMConvertToRadians(degAng));
 		m_pos.y = spHitTerrain->GetPos().y + length * sin(DirectX::XMConvertToRadians(degAng));
	}

	transMat = Math::Matrix::CreateTranslation(m_pos);

	m_mWorld = m_rotMat * transMat;

	// アニメーションの更新
	// 止まっていたらアニメーションしない
	if (m_situationType == SituationType::Run)
	{
		m_spAnimator->AdvanceTime(m_spModel->WorkNodes(), 2.0f);
	}
	else
	{
		m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
	}
	m_spModel->CalcNodeMatrices();
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
		//m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("Stop"));
	}

	// オブジェクトのタイプ
	m_objectType = ObjectType::Player;

	m_baseObjectType = BaseObjectType::Player;

	// 移動速度
	m_moveSpeed = 0.3f;

	// ジャンプ力
	m_jumpPow = 0.5f;

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	m_pos = { 30.0f, 1.0f, -40.0f };

	m_stopFlg = true;
}

void Player::Reset()
{
	CharacterBase::Reset();

	// 座標
	m_pos = { 30.0f, 1.0f, -40.0f };
	
	// 生存フラグ
	m_aliveFlg = true;

	// 状態
	m_situationType = SituationType::Stop;

	// 角度
	m_angle = 0;
	m_rotMat = Math::Matrix::CreateRotationY(m_angle);

	// アニメーションフラグ
	m_walkAnimeDirFlg = false;

	m_stopFlg = true;


}

// 当たり判定
void Player::HitJudge()
{
	// 当たったオブジェクトリストをリセット
	m_wpHitObjectList.clear();

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
	m_rotationGround.rotMat = Math::Matrix::Identity;
	m_rotationGround.hitFlg = false;

	// スフィアで貫通するようになったらレイ判定
	if (m_gravity >= 1.0f)
	{
		// 地面とのレイ当たり判定
		// 当たった座標
		Math::Vector3 hitPos = Math::Vector3::Zero;
		// レイのスタート座標
		Math::Vector3 startPos = m_pos;
		startPos.y += m_enableStepHeight;

		// 当たったかどうかのフラグ
		bool hitFlg = false;
		hitFlg = RayHitGround(startPos, hitPos, Math::Vector3::Down, m_gravity + m_enableStepHeight);

		if (hitFlg)
		{
			// 当たったオブジェクト
			std::shared_ptr<TerrainBase> spHitObject = m_wpHitTerrain.lock();

			if (spHitObject)
			{
				switch (spHitObject->GetObjectType())
				{
					// バウンドする床に乗った場合
				case ObjectType::BoundGround:
					// 座標
					m_pos = hitPos;
					m_pos.y += m_enableStepHeight;
					// 重力
					m_gravity = -1.0f;
					//// 空中にいない
					//m_situationType &= (~SituationType::Air);
					//m_situationType &= (~SituationType::Jump);
					break;

					// 動く床に乗った場合
				case ObjectType::MoveGround:
				case ObjectType::DropGround:
					// 座標
					m_pos = hitPos;
					m_pos.y += m_enableStepHeight;
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
					// 座標
					m_pos = hitPos;
					m_pos.y += m_enableStepHeight;
					// 重力
					m_gravity = 0;
					// 空中にいない
					m_situationType &= (~SituationType::Air);
					m_situationType &= (~SituationType::Jump);
					// 動く前の行列
					m_rotationGround.hitFlg = true;
					break;

				default:
					// 座標
					m_pos = hitPos;
					m_pos.y += m_enableStepHeight;
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
	else
	{
		// 地面とのスフィア判定
		// 方向
		Math::Vector3 hitDir = Math::Vector3::Zero;
		// スフィアの中心座標
		Math::Vector3 centerPos = m_pos;
		// スフィアの半径
		float radius = 1.0f;
		centerPos.y += radius;

		// 当たったかどうかのフラグ
		bool hitFlg = false;
		// めり込んだ距離
		float maxOverLap = 0.0f;

		hitFlg = SphereHitGround(centerPos, radius, hitDir, maxOverLap);

		// 当たった場合
		if (hitFlg)
		{
			// 当たったオブジェクト
			std::shared_ptr<TerrainBase> spHitObject = m_wpHitTerrain.lock();

			// 当たったオブジェクト毎に処理を変える
			if (spHitObject)
			{
				spHitObject->OnHit();

				switch (spHitObject->GetObjectType())
				{
					// 跳ねる床
				case ObjectType::BoundGround:
					// X軸とZ軸の補正はなし
					hitDir.x = 0;
					hitDir.z = 0;
					hitDir.Normalize();
					m_pos += hitDir * maxOverLap;
					// 重力
					m_gravity = -1.0f;
					// 空中にいない
					//m_situationType &= (~SituationType::Air);
					//m_situationType &= (~SituationType::Jump);
					break;

					// 動く床
				case ObjectType::MoveGround:
				case ObjectType::DropGround:
					// X軸とZ軸の補正はなし
					hitDir.x = 0;
					hitDir.z = 0;
					hitDir.Normalize();
					m_pos += hitDir * maxOverLap;
					// 重力
					m_gravity = 0.0f;
					// 空中にいない
					m_situationType &= (~SituationType::Air);
					m_situationType &= (~SituationType::Jump);
					// 動く床の動く前の行列
					m_moveGround.transMat = Math::Matrix::CreateTranslation(spHitObject->GetPos());
					// 動く床に当たったかどうか
					m_moveGround.hitFlg = true;
					break;

					// 回る床
				case ObjectType::RotationGround:
					// X軸とZ軸の補正はなし
					hitDir.x = 0;
					hitDir.z = 0;
					hitDir.Normalize();
					m_pos += hitDir * maxOverLap;
					// 重力
					m_gravity = 0.0f;
					// 空中にいない
					m_situationType &= (~SituationType::Air);
					m_situationType &= (~SituationType::Jump);
					// 動く前の行列
					m_rotationGround.hitFlg = true;
					break;

					// 通常
				default:
					// X軸とZ軸の補正はなし
					//hitDir.x = 0;
					//hitDir.z = 0;
					hitDir.Normalize();
					m_pos += hitDir * maxOverLap;
					// 重力
					m_gravity = 0.0f;
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
}

void Player::HitJudgeEvent()
{
	Math::Vector3 spherePos = m_pos;
	spherePos.y += 6.f;

	bool hitFlg = false;
	hitFlg = SphereHitJudge(spherePos, 2.0f, KdCollider::TypeEvent, false);

	if (hitFlg)
	{
		// 当たったオブジェクト
		std::shared_ptr<KdGameObject> spHitObject;

		// 地面を探す
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
		}
	}
}

void Player::HitJudgeEnemy()
{
	// 方向
	Math::Vector3 hitDir = Math::Vector3::Zero;
	// スフィアの中心座標
	Math::Vector3 centerPos = m_pos;
	// スフィアの半径
	float radius = 1.0f;
	centerPos.y += radius;

	// 当たったかどうかのフラグ
	bool hitFlg = false;
	// めり込んだ距離
	float maxOverLap = 0.0f;

	hitFlg = SphereHitJudge(centerPos, radius, KdCollider::TypeDamage, hitDir, maxOverLap);

	// 当たっていた時の処理
	if (hitFlg)
	{
		// 当たったオブジェクト
		std::shared_ptr<KdGameObject> spHitObject;

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

		// 敵の座標
		Math::Vector3 enemyPos = spHitObject->GetPos();
		enemyPos.y += 2.0f;
		// 敵からプレイヤーのベクトル
		Math::Vector3 vec = m_pos - enemyPos;
		vec.z = 0;
		// 敵から見たプレイヤーの角度
		float degAng = DirectX::XMConvertToDegrees(atan2(vec.x, vec.y));
		if (degAng < 90 && degAng > -90)
		{
			// 敵を踏んだ時の処理
			spHitObject->OnHit();
			m_gravity = -0.7f;
		}
		else
		{
			m_aliveFlg = false;
		}
	}
}

void Player::GoalProcess()
{
	m_goalStayCnt++;
	if (m_goalStayCnt > m_goalStayTime)
	{
		m_pos.x = m_goalPos.x;
		m_pos.z = m_goalPos.z;
		m_angle = 180;
		m_rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));
		m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("Goal"), true);
	}
}