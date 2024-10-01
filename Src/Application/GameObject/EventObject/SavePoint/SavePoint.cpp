#include "SavePoint.h"

void SavePoint::Update()
{
	
}

void SavePoint::GenerateDepthMapFromLight()
{
	//if (m_spModel)
	//{
	//	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	//}
}

void SavePoint::DrawLit()
{
	//if (m_spModel)
	//{
	//	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	//}
}

void SavePoint::DrawUnLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void SavePoint::Init()
{
	EventObjectBase::Init();

	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/EventObject/Goal/goal.gltf");
	}

	m_objectType = ObjectType::SavePoint;

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("SavePoint", m_spModel, KdCollider::TypeEvent);
}

void SavePoint::OnHit()
{
}

void SavePoint::SetParam(const Param& _param)
{
	m_param = _param;

	m_pos = m_param.basePos;
	m_respawnPos = m_param.basePos;

	m_mWorld.Translation(m_pos);
}
