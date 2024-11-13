#include "BackGround.h"

void BackGround::DrawUnLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullFront);

		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);

		KdShaderManager::Instance().UndoRasterizerState();
	}
}

void BackGround::Init()
{
	m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/BackGround/backGround.gltf");

	Math::Matrix scaleMat = Math::Matrix::CreateScale(15.0f);

	m_mWorld = scaleMat;
}