#include "House.h"

void House::GenerateDepthMapFromLight()
{
	//if (m_spModel)
	//{
	//	KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
	//	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	//	KdShaderManager::Instance().UndoRasterizerState();
	//}
}

void House::DrawUnLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
		KdShaderManager::Instance().UndoRasterizerState();
	}
}

void House::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Title/House/house.gltf");
	}
}
