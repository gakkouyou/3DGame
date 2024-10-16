#include "ResultPlayer.h"

void ResultPlayer::Update()
{
	// アニメーションの更新
	m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
	m_spModel->CalcNodeMatrices();

	if (m_startAnimationFlg == false) return;
	if (m_endAnimationFlg == true) return;

	// カウント
	m_stayCount++;
	if (m_stayCount == m_stayTime)
	{
		// アニメーション変更
		if (m_spAnimator && m_spModel)
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("Run"), true);
		}
	}
	if (m_stayCount >= m_stayTime)
	{
		// 右に走る
		Math::Matrix scaleMat = Math::Matrix::CreateScale(8.0f);
		Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(90.0f));
		Math::Matrix transMat = Math::Matrix::CreateTranslation(GetPos() + m_moveVec * m_speed);

		m_mWorld = scaleMat * rotMat * transMat;
	}

	if (GetPos().x > m_goalPos.x)
	{
		m_endAnimationFlg = true;
	}
}

void ResultPlayer::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void ResultPlayer::DrawLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void ResultPlayer::Init()
{
	if (!m_spModel)
	{
		// モデル
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Character/Player/player.gltf");

		// 初期のアニメーション
		m_spAnimator = std::make_shared<KdAnimator>();
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Idle"), false);
	}

	Math::Matrix scaleMat = Math::Matrix::CreateScale(8.0f);
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(180.0f));
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_startPos);

	m_mWorld = scaleMat * rotMat * transMat;
}