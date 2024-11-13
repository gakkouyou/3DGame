#include "FlyEnemy.h"
#include "../../../../Scene/SceneManager.h"
#include "../../../Effect/Smoke/Smoke.h"

void FlyEnemy::Update()
{
	// ポーズ画面中は更新しない
	if (m_pauseFlg == true) return;

	// 生きている時だけ動く
	if (m_aliveFlg == true)
	{
		// 上下に動く
		m_angle++;
		if (m_angle >= 360)
		{
			m_angle -= 360;
		}

		float pos = sin(DirectX::XMConvertToRadians(m_angle)) * m_param.moveArea;

		m_pos.y = m_param.startPos.y + pos;
	}

	if (SceneManager::Instance().GetDebug())
	{
		Math::Vector3 pos = m_param.startPos;
		pos.y = m_pos.y;
		// moveエリア可視化
		m_pDebugWire->AddDebugSphere(m_param.startPos, m_param.moveArea, kBlackColor);
	}

	// 死んでいたらアニメーションしない
	if (m_aliveFlg == true)
	{
		// アニメーションの更新
		// 止まっていたらアニメーションしない
		m_spAnimator->AdvanceTime(m_spModel->WorkNodes(), 2.0f);
		m_spModel->CalcNodeMatrices();
	}

	// 行列
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);

	Math::Matrix scaleMat = Math::Matrix::CreateScale(0.25f);

	// 死亡モーション
	if (m_aliveFlg == false)
	{
		scaleMat = Math::Matrix::CreateScale({ 0.25f, 0.05f, 0.25f });
		m_deathCount++;

		if (m_deathCount >= m_deathTime)
		{
			m_isExpired = true;

			// 煙を生み出す
			std::shared_ptr<Smoke> smoke = std::make_shared<Smoke>();
			smoke->Init();
			smoke->SetPos(m_pos);
			smoke->SetSmokeType(Smoke::SmokeType::DeathSmoke);
			SceneManager::Instance().AddObject(smoke);
		}
	}

	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_param.rotDegAng));

	m_mWorld = scaleMat * rotMat * transMat;
}

void FlyEnemy::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Character/Enemy/FlyEnemy/flyEnemy.gltf");

		// 初期のアニメーション
		m_spAnimator = std::make_shared<KdAnimator>();
		m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("Fly"));
	}

	// 行列を作っておく
	m_mWorld = Math::Matrix::CreateTranslation(m_pos);

	// 当たり判定
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("FlyEnemy", m_spModel, KdCollider::TypeDamage | KdCollider::TypeDebug);

	// オブジェクトのタイプ
	m_objectType = ObjectType::FlyEnemy;

	EnemyBase::Init();

	// デバッグワイヤー
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
}

Math::Vector3 FlyEnemy::GetPos() const
{
	// これ以上上なら倒せるという座標を返す
	Math::Vector3 pos;
	if (m_spModel)
	{
		pos = (m_spModel->FindNode("DeathPoint")->m_worldTransform * m_mWorld).Translation();
	}
	return pos;
}

void FlyEnemy::SetParam(Param _param)
{
	m_pos				= _param.startPos;
	m_param.startPos	= _param.startPos;
	m_param.moveArea	= _param.moveArea;
	m_param.rotDegAng	= _param.rotDegAng;
}

void FlyEnemy::OnHit()
{
	// 死ぬ
	m_aliveFlg = false;
}