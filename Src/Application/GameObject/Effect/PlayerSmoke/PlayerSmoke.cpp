#include "PlayerSmoke.h"

void PlayerSmoke::Update()
{
	// ポーズ画面中は更新しない
	if (m_pauseFlg == true) return;

	m_scale -= 0.02f;
	if (m_scale < 0)
	{
		m_isExpired = true;
		m_scale = 0;
	}
}

void PlayerSmoke::DrawLit()
{
	if (m_spModel)
	{
		Math::Matrix scaleMat = Math::Matrix::CreateScale(m_scale);

		Math::Matrix transMat = Math::Matrix::CreateTranslation(GetPos());

		m_mWorld = scaleMat * transMat;

		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void PlayerSmoke::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Effect/Smoke/smoke.gltf");
	}

	m_scale = 1.0f;
}
