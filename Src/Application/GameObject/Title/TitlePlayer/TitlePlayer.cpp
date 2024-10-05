#include "TitlePlayer.h"

void TitlePlayer::Update()
{
	KdShaderManager::Instance().WorkAmbientController().AddPointLight({ 5, 5, 5 }, 10, { 0, 3, 0 });

	m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
	m_spModel->CalcNodeMatrices();
}

void TitlePlayer::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void TitlePlayer::DrawLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void TitlePlayer::Init()
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

	Math::Matrix scaleMat = Math::Matrix::CreateScale(5.0f);
	Math::Matrix rotMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(180.0f), DirectX::XMConvertToRadians(-90.0f), 0);
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_bedPos);

	m_mWorld = scaleMat * rotMat * transMat;
}
