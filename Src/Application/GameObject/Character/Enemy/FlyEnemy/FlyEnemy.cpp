#include "FlyEnemy.h"
#include "../../../../Scene/SceneManager.h"
#include "../../../Effect/PlayerSmoke/PlayerSmoke.h"

void FlyEnemy::Update()
{
	if (m_aliveFlg == false) return;

	// ポーズ画面中は更新しない
	if (m_pauseFlg == true) return;

	m_angle++;
	if (m_angle >= 360)
	{
		m_angle -= 360;
	}

	float pos = sin(DirectX::XMConvertToRadians(m_angle)) * m_param.moveArea;

	m_pos.y = m_param.startPos.y + pos;
}

void FlyEnemy::PostUpdate()
{
	// ポーズ画面中は更新しない
	if (m_pauseFlg == true) return;

	if (SceneManager::Instance().GetDebug())
	{
		// searchエリア可視化
		//m_pDebugWire->AddDebugSphere(m_pos, m_param.searchArea, kRedColor);
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
			std::shared_ptr<PlayerSmoke> smoke = std::make_shared<PlayerSmoke>();
			smoke->Init();
			smoke->SetPos(m_pos);
			smoke->SetSmokeType(PlayerSmoke::SmokeType::DeathSmoke);
			SceneManager::Instance().AddObject(smoke);
		}
	}

	Math::Matrix rotMat		= Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_param.rotDegAng));

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

	// スピード
	m_moveSpeed = 5.f;

	// 当たり判定
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("FlyEnemy", m_spModel, KdCollider::TypeDamage);

	// オブジェクトのタイプ
	m_objectType = ObjectType::FlyEnemy;

	EnemyBase::Init();

	// デバッグワイヤー
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

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
	m_aliveFlg = false;
}