#include "SavePoint.h"

void SavePoint::Update()
{
	
}

void SavePoint::GenerateDepthMapFromLight()
{
	if (m_isExpired == true) return;

	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void SavePoint::DrawLit()
{
	if (m_isExpired == true) return;

	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void SavePoint::DrawUnLit()
{
	//if (m_spModel)
	//{
	//	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	//}
}

void SavePoint::DrawBright()
{
	//Math::Color color = { 0.3, 0.3, 0.3, 1 };
	//if (m_spModel)
	//{
	//	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld, color);
	//}
}

void SavePoint::Init()
{
	if (m_isExpired == true) return;

	EventObjectBase::Init();

	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/EventObject/SavePoint/savePoint.gltf");
	}

	m_objectType = ObjectType::SavePoint;

	if (m_aliveFlg == true)
	{
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("SavePoint", m_spModel, KdCollider::TypeEvent);
	}
}

void SavePoint::OnHit()
{
	std::shared_ptr<KdSoundInstance> se = KdAudioManager::Instance().Play("Asset/Sounds/SE/savePoint.wav");
	se->SetVolume(0.02f);
	m_isExpired = true;
}

void SavePoint::SetParam(const Param& _param)
{
	if (_param.modelNum == 1)
	{
		m_isExpired = true;
		return;
	}

	m_param = _param;

	m_pos = m_param.basePos;
	m_respawnPos = m_param.basePos;

	m_mWorld.Translation(m_pos);
}
