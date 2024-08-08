﻿#include "Player.h"
#include "../../../Scene/SceneManager.h"
#include "../../Camera/CameraBase.h"
#include "../../Terrain/TerrainBase.h"

#include "../../../main.h"

void Player::Update()
{
	// デバッグモード中は更新しない
	if (SceneManager::Instance().GetDebug()) return;

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

	// WASDで移動
	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_moveVec.z += 1.0f;
		m_situationType |= SituationType::Run;
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_moveVec.x -= 1.0f;
		m_situationType |= SituationType::Run;
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_moveVec.z -= 1.0f;
		m_situationType |= SituationType::Run;
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_moveVec.x += 1.0f;
		m_situationType |= SituationType::Run;
	}

	// カメラの向きで移動方向を補正
	m_moveVec = m_moveVec.TransformNormal(m_moveVec, m_wpCamera.lock()->GetRotationYMatrix());
	m_moveVec.Normalize();

	// 動いていなかったら止まっている状態にする。
	if (m_moveVec.LengthSquared() == 0)
	{
		m_situationType &= (~SituationType::Run);
	}

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

	if (m_situationType != oldSituationType)
	{
		// 止まっている状態→歩き
		if (m_situationType & SituationType::Run)
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
		if (m_situationType & SituationType::Stop)
		{
			if (oldSituationType & SituationType::Run)
			{
				//m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("Stop"));
			}
		}
	}

	if (m_spAnimator->IsAnimationEnd())
	{
		// 歩いている状態の時
		// 空中にいたらアニメーションしない
		if ((m_situationType & SituationType::Run) && !(m_situationType & SituationType::Air))
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
	m_pos += m_moveVec * m_moveSpeed;

	// 重力
	m_gravity += m_gravityPow;
	m_pos.y -= m_gravity;
	// 重力の制限
	if (m_gravity >= m_maxGravity)
	{
		m_gravity = m_maxGravity;
	}

	// 移動中
	// 回転処理
	if (m_situationType & SituationType::Run)
	{
		// 今キャラクターが向いている方向
		// ①キャラの回転行列を作成
		Math::Matrix nowRotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));
		// ②現在の方向を求める
		// ベースの角度からキャラの角度で変形する？
		Math::Vector3 nowVec = Math::Vector3::TransformNormal({ 0, 0, -1 }, nowRotMat);

		// もう一つ
		//Math::Vector3 nowVec = m_mWorld.Backward();

		// 向きたい方向
		Math::Vector3 toVec = m_moveVec;
		toVec.Normalize();

		// 内積...ベクトルAとベクトルBとコサインなす角
		// ベクトルA*ベクトルB*コサインなす角
		//     1    *    1    *コサインなす角
		// = コサインなす角
		// A・B
		float d = nowVec.Dot(toVec);

		// 丸め誤差...小数点以下を省略した際に生じる誤差
		// 内積は１～ー１がでる。
		// ー１～１でdの値をクランプ(遮断)する
		d = std::clamp(d, -1.0f, 1.0f);

		// アークコサインで角度に変換
		float ang = DirectX::XMConvertToDegrees(acos(d));

		// ゆっくり回転するように処理
		if (ang >= 0.1f)
		{
			// とりあえず最大で５度回転するように制御
			if (ang > 5.0f)
			{
				ang = 5.0f;
			}


			// 外積...ベクトルAとベクトルBに垂直なベクトル
			// A×B
			// 掛け算ではなく「クロス」
			Math::Vector3 c = toVec.Cross(nowVec);

			if (c.y >= 0)	// 上
			{
				m_angle -= ang;
				if (m_angle < 0)
				{
					m_angle += 360;
				}
			}
			else			//下
			{
				m_angle += ang;
				if (m_angle >= 360)
				{
					m_angle -= 360;
				}
			}
		}
		// 回転行列
		m_rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));
	}

	if (!m_effectFlg)
	{
		m_effectFlg = true;
		KdEffekseerManager::GetInstance().Play("test.efkefc", { 0, 3, 0 }, { 90.0f, 90.0f, 0 }, 0.1f, 0.5f, true);
	}

	Application::Instance().m_log.AddLog("pos.x:%.2f pos.y:%.2f pos.z:%.2f\n", m_pos.x, m_pos.y, m_pos.z);
	Application::Instance().m_log.AddLog("situation:%d\n", m_situationType);

	// 当たり判定
	HitJudge();
}

void Player::PostUpdate()
{
	Math::Matrix transMat = Math::Matrix::Identity;
	// 動く床に当たっていた時の処理
	if (m_moveGround.hitFlg)
	{
		// 動く床の動く前の逆行列
		Math::Matrix inverseMatrix = DirectX::XMMatrixInverse(nullptr, m_moveGround.transMat);
		// 動く床から見たプレイヤーの座標行列
		Math::Matrix playerMat = Math::Matrix::CreateTranslation(m_pos) * inverseMatrix;

		// 動く床の動いた後の行列
		Math::Matrix afterMoveGroundMat = Math::Matrix::CreateTranslation(m_wpHitObject.lock()->GetMatrix().Translation());

		// 座標を確定
		m_pos = afterMoveGroundMat.Translation() + playerMat.Translation();
	}

	// 回る床に当たっていた時の処理
	if (m_rotationGround.hitFlg)
	{

	}

	transMat = Math::Matrix::CreateTranslation(m_pos);

	m_mWorld = m_rotMat * transMat;

	// アニメーションの更新
	// 止まっていたらアニメーションしない
	m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
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

	// 移動速度
	m_moveSpeed = 0.3f;

	// ジャンプ力
	m_jumpPow = 0.5f;

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	m_pos.y = 5.0f;
	m_pos.z = -20.0f;
}

// 当たり判定
void Player::HitJudge()
{
	// 地面との当たり判定
	HitJudgeGround();

	// 触れたらイベントが発生する
	HitJudgeEvent();
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
		hitFlg = RayHitJudge(startPos, hitPos, Math::Vector3::Down, m_gravity + m_enableStepHeight, KdCollider::TypeGround);

		if (hitFlg)
		{
			// 当たったオブジェクトのタイプ
			std::shared_ptr<KdGameObject> spHitObject = m_wpHitObject.lock();

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

		hitFlg = SphereHitJudge(centerPos, radius, KdCollider::TypeGround, hitDir, maxOverLap);

		// 当たった場合
		if (hitFlg)
		{
			// 当たったオブジェクトのタイプ
			std::shared_ptr<KdGameObject> spHitObject = m_wpHitObject.lock();

			// 当たったオブジェクト毎に処理を変える
			if (spHitObject)
			{
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
					m_rotationGround.rotMat = spHitObject->GetRotationMatrix();
					m_rotationGround.transMat = Math::Matrix::CreateTranslation(spHitObject->GetPos());
					break;

					// 通常
				default:
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
	spherePos.y += 1.5f;

	bool hitFlg = false;
	hitFlg = SphereHitJudge(spherePos, 2.0f, KdCollider::TypeEvent, true);

	if (hitFlg)
	{
		std::shared_ptr<KdGameObject> spHitObject = m_wpHitObject.lock();
		if (spHitObject)
		{
			spHitObject->OnHit();
		}
	}
}