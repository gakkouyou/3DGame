#include "PlayerSmoke.h"

void PlayerSmoke::Update()
{
	m_alpha -= 0.01f;
	if (m_alpha < 0)
	{
		m_isExpired = true;
	}
}

void PlayerSmoke::DrawLit()
{
	if (m_spModel)
	{
		Math::Color color = { 1, 1, 1, m_alpha };

		static float scale = 1.0f;
		scale -= 0.02f;

		Math::Matrix scaleMat = Math::Matrix::CreateScale(scale);

		m_mWorld = scaleMat * GetPos();

		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld, color);
	}
}

void PlayerSmoke::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Effect/Smoke/smoke.gltf");
	}
}
