#include "ResultBackGround.h"

void ResultBackGround::DrawLit()
{
	if (m_spModel)
	{
		if (m_startAnimationFlg && !m_blackAnimationFlg)
		{
			m_color = Math::Vector3::Lerp(m_startColor, m_goalColor, m_progress);
			m_progress += m_speed;

			if (m_progress > 1)
			{
				m_progress = 0.0f;
				m_blackAnimationFlg = true;
			}
		}

		if (m_blackAnimationFlg)
		{
			m_color = Math::Vector3::Lerp(m_goalColor, { 0, 0, 0 }, m_progress);
			m_progress += m_speed;

			if (m_progress > 1)
			{
				m_progress = 1.0f;
			}
		}

		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullFront);

		KdShaderManager::Instance().m_StandardShader.SetColorEnable(true, { m_color.x, m_color.y, m_color.z });

		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);

		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullBack);
	}
}

void ResultBackGround::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/BackGround/ResultBackGround/resultBackGround.gltf");
	}

	Math::Matrix scaleMat = Math::Matrix::CreateScale(9.5f);

	m_mWorld = scaleMat;

	m_mWorld.Translation({ 0, -5, 20 });

	m_startColor.x = 160 / 255.0f;
	m_startColor.y = 216 / 255.0f;
	m_startColor.z = 239 / 255.0f;

	m_color = m_startColor;

	m_goalColor.x = 238 / 255.0f;
	m_goalColor.y = 120 / 255.0f;
	m_goalColor.z = 0;

	m_speed = 1.0f / m_frame;
}
