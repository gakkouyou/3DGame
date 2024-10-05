#include "House.h"

void House::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
		KdShaderManager::Instance().UndoRasterizerState();

		Math::Matrix mat = Math::Matrix::CreateTranslation({ -20, 0, 0 });
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, mat);

		mat.Translation({ 20, 0, 0 });
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, mat);
	}

	if (m_spRoadModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spRoadModel, m_roadMat);
	}
}

void House::DrawLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
		KdShaderManager::Instance().UndoRasterizerState();

		Math::Matrix mat = Math::Matrix::CreateTranslation({ -20, 0, 0 });
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, mat);

		mat.Translation({ 20, 0, 0 });
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, mat);
	}

	if (m_spRoadModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spRoadModel, m_roadMat);
	}
}

void House::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Title/House/house.gltf");
	}

	// ベッドの座標
	m_bedPos = (m_spModel->FindNode("BedPoint")->m_worldTransform * m_mWorld).Translation();

	if (!m_spRoadModel)
	{
		m_spRoadModel = std::make_shared<KdModelData>();
		m_spRoadModel->Load("Asset/Models/Title/Road/road.gltf");

		Math::Matrix transMat = Math::Matrix::CreateTranslation({ 0, 0, -10 });
		Math::Matrix scaleMat = Math::Matrix::CreateScale(2);
		m_roadMat = scaleMat * transMat;
	}
}
