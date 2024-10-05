#include "TitleBackGround.h"

void TitleBackGround::DrawUnLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullFront);

		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);

		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullBack);
	}
}

void TitleBackGround::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Title/BackGround/backGround.gltf");
	}

	Math::Matrix scaleMat = Math::Matrix::CreateScale(9.5f);

	m_mWorld = scaleMat;

	m_mWorld.Translation({ 0, -5, 20 });
}
