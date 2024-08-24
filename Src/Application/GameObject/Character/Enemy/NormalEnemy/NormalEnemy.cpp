﻿#include "NormalEnemy.h"
#include "Application/main.h"
#include "../../../../Scene/SceneManager.h"

void NormalEnemy::Update()
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

	// ベクトルリセット
	m_moveVec = Math::Vector3::Zero;

	//Application::Instance().m_log.Clear();
	Application::Instance().m_log.AddLog("%.2f\n", m_gravity);

	if (m_setParamFlg == false)
	{
		// 追尾、見失ったモーションをしていなければ行動範囲内で動く
		if (((m_situationType & SituationType::Homing) == 0) && ((m_situationType & SituationType::LostTarget) == 0))
		{
			// 回転中じゃないなら動かす
			if (m_move.rotFlg == false)
			{
				// ゴールまでのベクトル
				Math::Vector3 vec = m_move.goalPos - m_pos;
				vec.y = 0;
				// もしゴールまでの距離がスピードより短くなったら、座標をゴール座標にして、回転させる
				if (vec.Length() < m_moveSpeed)
				{
					m_pos.x = m_move.goalPos.x;
					m_pos.z = m_move.goalPos.z;
					m_move.rotFlg = true;
					m_move.confirmedAngFlg = true;
				}
				// 違ったら移動ベクトルを確定
				else
				{
					vec.Normalize();
					m_moveVec = vec;
				}
			}

			// ゴール座標を決める
			if (m_move.confirmedAngFlg == true)
			{
				// ゴール座標を求めるための角度を求める
				// 今の角度の+-90度
				// 最小の角度
				float minAng = m_move.degAng + 90;

				float degAng = rand() % 180 + minAng;
				if (degAng >= 360)
				{
					degAng -= 360;
				}
				if (degAng < 0)
				{
					degAng += 360;
				}
				m_move.degAng = degAng;
				// ゴールの座標を求める
				m_move.goalPos.x = m_param.startPos.x + m_param.moveArea * cos(DirectX::XMConvertToRadians(m_move.degAng));
				m_move.goalPos.z = m_param.startPos.z + m_param.moveArea * sin(DirectX::XMConvertToRadians(m_move.degAng));
				m_move.confirmedAngFlg = false;
			}

			if (m_move.rotFlg == true)
			{
				// 向きたい方向 Y座標は無視
				Math::Vector3 vec = m_move.goalPos - m_pos;
				vec.y = 0;

				// 回転してなかったら回転処理終了
				if (!RotationCharacter(m_degAng, vec, m_move.minRotAng))
				{
					m_move.rotFlg = false;
				}
			}
		}

		// ホーミング処理前の状態
		UINT oldSituationType = m_situationType;

		// ターゲットの追尾
		TargetHoming();

		// ホーミング状態が外れた瞬間見失ったモーションをする
		if (oldSituationType & SituationType::Homing)
		{
			if ((m_situationType & SituationType::Homing) == 0)
			{
				// 見失った状態にする
				m_situationType |= SituationType::LostTarget;
				// 元の角度を保持
				m_lostTarget.oldDegAng = m_degAng;
				// 見失ったモーション用の変数を初期化
				m_lostTarget.sumLotAng	= 0;					// 回転した角度の合計値
				m_lostTarget.stayCount	= 0;					// 待機時間カウント
				m_lostTarget.stayFlg	= false;				// 待機中フラグ
				m_lostTarget.dir		= m_lostTarget.Left;	// 方向を左にする
			}
		}

		// 見失っている状態で見つけたら見失っている状態を終了させる
		if ((m_situationType & SituationType::Find) && (m_situationType & SituationType::LostTarget))
		{
			// 見失ったモーション用の変数を初期化
			m_lostTarget.sumLotAng = 0;				// 回転した角度の合計値
			m_lostTarget.stayCount = 0;				// 待機時間カウント
			m_lostTarget.stayFlg = false;			// 待機中フラグ
			m_lostTarget.dir = m_lostTarget.Left;	// 方向を左にする

			m_situationType &= (~SituationType::LostTarget);
		}
		
		// 見失っているときに処理
		if (m_situationType & SituationType::LostTarget)
		{
			LostTargetProcess();
		}

		// 見失ったモーションが終了したら回転処理をする
		if (oldSituationType & SituationType::LostTarget)
		{
			if ((m_situationType & SituationType::LostTarget) == 0)
			{
				m_move.rotFlg = true;
			}
		}

		// 移動
		float speed;
		// ホーミング状態ならスピードを上げる
		if (m_situationType & SituationType::Homing)
		{
			speed = m_homing.speed;
			m_walkMotion.stayCount = m_walkMotion.stayTime;
		}
		else
		{
			speed = m_moveSpeed;
		}
		
		// 見失ったモーション中でないときに処理
		if ((m_situationType & SituationType::LostTarget) == 0 && (m_situationType & SituationType::Find) == 0)
		{
			// 待機中でなければ動かす
			if (m_walkMotion.stayFlg == false)
			{
				m_pos += m_moveVec * speed;
			}
			// 待機中
			else
			{
				m_walkMotion.stayCount++;
				// 待機時間が終了したら、待機を終了し、少しジャンプさせる
				if (m_walkMotion.stayCount > m_walkMotion.stayTime)
				{
					m_walkMotion.stayFlg = false;
					if ((m_situationType & SituationType::Air) == 0)
					{
						m_gravity = -m_walkMotion.jumpPow;
					}
					m_walkMotion.stayCount = 0;
				}
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

		// 当たり判定
		HitJudge();
	}
	else
	{
		m_pos = m_param.startPos;
		m_degAng = 0;
		m_setParamFlg = false;
	}

	// 落下死
	if (m_pos.y < -60.0f)
	{
		m_isExpired = true;
	}
}

void NormalEnemy::PostUpdate()
{
	if (SceneManager::Instance().GetDebug())
	{
		// searchエリア可視化
		m_pDebugWire->AddDebugSphere(m_pos, m_param.searchArea, kRedColor);
		Math::Vector3 pos = m_param.startPos;
		pos.y = m_pos.y;
		// moveエリア可視化
		m_pDebugWire->AddDebugSphere(pos, m_param.moveArea, kWhiteColor);
	}

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);

	// 回転行列
	m_rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));

	Math::Matrix scaleMat = Math::Matrix::CreateScale(1.5f);

	m_mWorld = scaleMat * m_rotMat * transMat;
}

void NormalEnemy::Init()
{
	srand(timeGetTime());

	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Character/Enemy/NormalEnemy/normalEnemy.gltf");
	}

	// 行列を作っておく
	m_mWorld = Math::Matrix::CreateTranslation(m_pos);



	// スピード
	m_moveSpeed = 0.2f;

	// 当たり判定
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("NormalEnemy", m_spModel, KdCollider::TypeDamage);

	// ゴールの座標を求める
	m_degAng = rand() % 360;
	m_move.goalPos.x = m_param.startPos.x + m_param.moveArea * cos(DirectX::XMConvertToRadians(m_degAng));
	m_move.goalPos.z = m_param.startPos.z + m_param.moveArea * sin(DirectX::XMConvertToRadians(m_degAng));

	// 回転させる
	m_move.rotFlg = true;

	// オブジェクトのタイプ
	m_objectType = ObjectType::NormalEnemy;

	EnemyBase::Init();

	// デバッグワイヤー
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
}

void NormalEnemy::Reset()
{
	// ゴールの座標を求める
	m_degAng = rand() % 360;
	m_move.goalPos.x = m_param.startPos.x + m_param.moveArea * cos(DirectX::XMConvertToRadians(m_degAng));
	m_move.goalPos.z = m_param.startPos.z + m_param.moveArea * sin(DirectX::XMConvertToRadians(m_degAng));

	// 回転させる
	m_move.rotFlg = true;

	// 今の状況
	m_situationType = SituationType::Stop;
}

void NormalEnemy::HitJudge()
{
	// 地面とのスフィア判定
	// 方向
	Math::Vector3 hitDir = Math::Vector3::Zero;
	// スフィアの中心座標
	Math::Vector3 centerPos = m_pos;
	//centerPos.y -= 2.0f;
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
		// 当たったオブジェクト
		std::shared_ptr<KdGameObject> spHitObject;

		// 地面を探す
		for (auto& hitObject : m_wpHitObjectList)
		{
			if (!hitObject.expired())
			{
				if (hitObject.lock()->GetBaseObjectType() == BaseObjectType::Ground)
				{
					spHitObject = hitObject.lock();
					break;
				}
			}
		}

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
				//m_moveGround.transMat = Math::Matrix::CreateTranslation(spHitObject->GetPos());
				// 動く床に当たったかどうか
				//m_moveGround.hitFlg = true;

				m_walkMotion.stayFlg = true;
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
				//m_rotationGround.rotMat = spHitObject->GetRotationMatrix();
				//m_rotationGround.transMat = Math::Matrix::CreateTranslation(spHitObject->GetPos());

				m_walkMotion.stayFlg = true;
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

				// 見つけたモーション中
				if ((m_situationType & SituationType::Find))
				{
					// ホーミング状態じゃないときは、少しジャンプする
					if ((m_situationType & SituationType::Homing) == 0)
					{
						m_situationType |= SituationType::Homing;
						m_gravity = -m_findJumpPow;
					}
					else
					{
						m_situationType &= (~SituationType::Find);
					}
				}
				// 空中にいない
				m_situationType &= (~SituationType::Air);
				m_situationType &= (~SituationType::Jump);

				m_walkMotion.stayFlg = true;
				break;
			}
		}
	}
	else
	{
		m_situationType |= SituationType::Air;
	}
}

void NormalEnemy::TargetHoming()
{
	std::shared_ptr<KdGameObject> spTarget = m_wpTarget.lock();
	if (spTarget)
	{
		Math::Vector3 vec = spTarget->GetPos() - m_pos;
		Math::Vector3 forwardVec = m_mWorld.Backward();

		vec.y = 0;

		// プレイヤーが索敵範囲内にいるなら、視野角判定をする
		if (vec.Length() < m_param.searchArea)
		{
			float dot = forwardVec.Dot(vec);
			dot = std::clamp(dot, -1.0f, 1.0f);
			float deg = DirectX::XMConvertToDegrees(acos(dot));
			if (deg < m_homing.viewingAngle)
			{
				// ホーミング状態ならホーミング
				if (m_situationType & SituationType::Homing)
				{
					vec.y = 0;
					vec.Normalize();
					m_moveVec = vec;
				}
				// ホーミング状態じゃないなら、見つけたモーションをする
				else
				{
					m_situationType |= SituationType::Find;
				}

				RotationCharacter(m_degAng, vec, m_homing.rotDegAng);
			}
			// 視野から外れたらホーミング状態と見つけたモーションを終了
			else
			{
				m_situationType &= (~SituationType::Homing);
				m_situationType &= (~SituationType::Find);
			}
		}
		// 索敵範囲外に行ったら、ホーミング状態と見つけたモーションを終了
		else
		{
			m_situationType &= (~SituationType::Homing);
			m_situationType &= (~SituationType::Find);
		}
	}
}

void NormalEnemy::SetParam(Param _param)
{
	m_param = _param;
	m_setParamFlg = true;
	m_pos = m_param.startPos;
	// ゴールの座標を求める
	m_move.goalPos.x = m_param.startPos.x + m_param.moveArea * cos(DirectX::XMConvertToRadians(m_degAng));
	m_move.goalPos.z = m_param.startPos.z + m_param.moveArea * sin(DirectX::XMConvertToRadians(m_degAng));
}

void NormalEnemy::OnHit()
{
	m_isExpired = true;
}

void NormalEnemy::FindTarget()
{
	
}

void NormalEnemy::LostTargetProcess()
{
	// 左を見る
	if (m_lostTarget.dir == m_lostTarget.Left && m_lostTarget.stayFlg == false)
	{
		// 何度回転したか保存しておく
		m_lostTarget.sumLotAng += m_lostTarget.lotAng;
		// 回転角度が最大値より大きくなったら、キャラの角度を元の角度-最大値にする
		if (m_lostTarget.sumLotAng > m_lostTarget.maxLotAng)
		{
			m_degAng = m_lostTarget.oldDegAng - m_lostTarget.maxLotAng;
			// 何度回転したかを最大値にする
			m_lostTarget.sumLotAng = m_lostTarget.maxLotAng;
			// 待機させる
			m_lostTarget.stayFlg = true;
			// 方向フラグを変える
			m_lostTarget.dir = m_lostTarget.Right;
		}
		else
		{
			// 回転させる
			m_degAng -= m_lostTarget.lotAng;
		}
	}
	
	// 左→右を見る
	if (m_lostTarget.dir == m_lostTarget.Right && m_lostTarget.stayFlg == false)
	{
		// 何度回転したか保存しておく(すでに最大値になっているはずなので-最大値になるまでする)
		m_lostTarget.sumLotAng -= m_lostTarget.lotAng;
		// 回転角度が最大値のマイナスより小さくなったら、キャラの角度を元の角度+最大値にする
		if (m_lostTarget.sumLotAng < -m_lostTarget.maxLotAng)
		{
			m_degAng = m_lostTarget.oldDegAng + m_lostTarget.maxLotAng;
			// 待機させる
			m_lostTarget.stayFlg = true;
			// 方向を変える
			m_lostTarget.dir = m_lostTarget.Base;
		}
		else
		{
			// 回転させる
			m_degAng += m_lostTarget.lotAng;
		}
	}

	// 右→元の角度に戻る
	if (m_lostTarget.dir == m_lostTarget.Base && m_lostTarget.stayFlg == false)
	{
		float dif = m_degAng - m_lostTarget.oldDegAng;

		if (dif > 360)
		{
			dif -= 360;
		}

		// 角度の差が回転角度より少なかったら角度を元の角度にする
		if (dif < m_lostTarget.lotAng)
		{
			m_degAng = m_lostTarget.oldDegAng;
			// 待機させる
			m_lostTarget.stayFlg = true;
			// 終了
			m_lostTarget.dir = m_lostTarget.End;
		}
		else
		{
			m_degAng -= m_lostTarget.lotAng;
		}
	}

	// 角度の調整
	if (m_degAng < 0)
	{
		m_degAng += 360;
	}
	if (m_degAng > 360)
	{
		m_degAng -= 360;
	}

	// 待機
	if (m_lostTarget.stayFlg)
	{
		m_lostTarget.stayCount++;

		// 待機終了
		if (m_lostTarget.stayCount > m_lostTarget.stayTime)
		{
			m_lostTarget.stayFlg = false;
			m_lostTarget.stayCount = 0;

			// もし元に戻ったら見失った状態をやめる
			if (m_lostTarget.dir == m_lostTarget.End)
			{
				m_situationType &= (~SituationType::LostTarget);
			}
		}
	}
}