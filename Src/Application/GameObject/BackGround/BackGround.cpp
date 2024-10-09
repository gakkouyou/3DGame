#include "BackGround.h"

void BackGround::DrawUnLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullFront);

		//KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);

		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullBack);
	}
}

void BackGround::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/BackGround/backGround.gltf");
	}

	Math::Matrix scaleMat = Math::Matrix::CreateScale(15.0f);

	m_mWorld = scaleMat;
}