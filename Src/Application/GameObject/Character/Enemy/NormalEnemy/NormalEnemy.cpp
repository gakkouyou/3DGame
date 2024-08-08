#include "NormalEnemy.h"
#include "Application/main.h"

void NormalEnemy::Update()
{
	// ベクトルリセット
	m_moveVec = Math::Vector3::Zero;
	Application::Instance().m_log.Clear();

	// 追尾していなければ行動範囲内で動く
	if ((m_situationType & SituationType::Homing) == 0)
	{
		// 待機中じゃないなら動かす
		if (m_stayFlg == false)
		{
			// ゴールまでのベクトル
			Math::Vector3 vec = m_goalPos - m_pos;
			vec.y = 0;
			Application::Instance().m_log.AddLog("%.2f\n", vec.Length());
			// もしゴールまでの距離がスピードより短くなったら、座標をゴール座標にして、待機させる
			if (vec.Length() < m_moveSpeed)
			{
				m_pos.x = m_goalPos.x;
				m_pos.z = m_goalPos.z;
				m_stayFlg = true;
			}
			// 違ったら移動ベクトルを確定
			else
			{
				vec.Normalize();
				m_moveVec = vec;
			}
		}

		// 待機カウントを進める
		if (m_stayFlg)
		{
			m_stayCnt++;
		}
		// 待機終了
		if (m_stayCnt > m_stayTime)
		{
			m_stayFlg = false;
			
			// ゴール座標を求めるための角度を求める
			// 今の角度の+-90度
			// 最小の角度
			float minAng = m_degAng + 90;

			float degAng = rand() % 180 + minAng;
			if (degAng >= 360)
			{
				degAng -= 360;
			}
			if (degAng < 0)
			{
				degAng += 360;
			}
			m_degAng = degAng;

			// ゴールの座標を求める
			m_goalPos.x = m_info.startPos.x + m_info.moveArea * cos(DirectX::XMConvertToRadians(m_degAng));
			m_goalPos.z = m_info.startPos.z + m_info.moveArea * sin(DirectX::XMConvertToRadians(m_degAng));
			m_stayCnt = 0;
		}
	}

	// ターゲットの追尾
	TargetHoming();

	// searchエリア可視化
	m_pDebugWire->AddDebugSphere(m_pos, m_info.searchArea, kRedColor);
	Math::Vector3 pos = m_info.startPos;
	pos.y = 0;
	// moveエリア可視化
	m_pDebugWire->AddDebugSphere(pos, m_info.moveArea, kBlackColor);

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

	Application::Instance().m_log.AddLog("ang:%.2f\n", m_degAng);
	Application::Instance().m_log.AddLog("x:%.2f y:%.2f z%.2f\n", m_pos.x, m_pos.y, m_pos.z);
	Application::Instance().m_log.AddLog("x:%.2f y:%.2f z%.2f", m_goalPos.x, m_goalPos.y, m_goalPos.z);

	HitJudge();
}

void NormalEnemy::PostUpdate()
{
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);

	m_mWorld = transMat;
}

void NormalEnemy::Init()
{
	srand(timeGetTime());

	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Character/Enemy/NormalEnemy/normalEnemy.gltf");
	}

	m_pos = { 20, 6, 20 };
	m_info.startPos = m_pos;

	m_mWorld = Math::Matrix::CreateTranslation(m_pos);

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	m_moveSpeed = 0.1f;

	m_info.moveArea = 10.0f;
	m_info.searchArea = 15.0f;

	// 当たり判定
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("NormalEnemy", m_spModel, KdCollider::TypeDamage);

	m_degAng = rand() % 360;

	// ゴールの座標を求める
	m_goalPos.x = m_info.startPos.x + m_info.moveArea * cos(DirectX::XMConvertToRadians(m_degAng));
	m_goalPos.z = m_info.startPos.z + m_info.moveArea * sin(DirectX::XMConvertToRadians(m_degAng));

	// ゴールの座標を求める
	Application::Instance().m_log.AddLog("x:%.2f y:%.2f z%.2f\n", m_pos.x, m_pos.y, m_pos.z);
	Application::Instance().m_log.AddLog("x:%.2f y:%.2f z%.2f", m_goalPos.x, m_goalPos.y, m_goalPos.z);
}

void NormalEnemy::HitJudge()
{
	// 地面とのスフィア判定
	// 方向
	Math::Vector3 hitDir = Math::Vector3::Zero;
	// スフィアの中心座標
	Math::Vector3 centerPos = m_pos;
	centerPos.y -= 2.0f;
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
				//m_moveGround.transMat = Math::Matrix::CreateTranslation(spHitObject->GetPos());
				// 動く床に当たったかどうか
				//m_moveGround.hitFlg = true;
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
}

void NormalEnemy::TargetHoming()
{
	std::shared_ptr<KdGameObject> spTarget = m_wpTarget.lock();
	if (spTarget)
	{
		Math::Vector3 vec = spTarget->GetPos() - m_pos;

		if (vec.Length() < m_info.searchArea)
		{
			vec.y = 0;
			vec.Normalize();
			m_moveVec = vec;
			m_situationType |= SituationType::Homing;
		}
		else
		{
			m_situationType &= (~SituationType::Homing);
		}
	}
}