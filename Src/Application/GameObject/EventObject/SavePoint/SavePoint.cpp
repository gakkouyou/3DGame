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
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/EventObject/Goal/goal.gltf");
	}

	m_objectType = ObjectType::SavePoint;

	m_baseObjectType = BaseObjectType::Event;

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("SavePoint", m_spModel, KdCollider::TypeEvent);
}

void SavePoint::SetPos(const Math::Vector3& _pos)
{
	m_pos = _pos;
	m_respawnPos = _pos;

	m_mWorld.Translation(m_pos);
}

void SavePoint::OnHit()
{
}