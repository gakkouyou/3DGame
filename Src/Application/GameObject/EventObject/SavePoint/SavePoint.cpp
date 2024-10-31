#include "SavePoint.h"

void SavePoint::Update()
{
	if (m_situationType == SituationType::Stand) return;
	if (m_situationType == SituationType::Up)
	{
		if (m_degAng > m_maxDegAng)
		{
			m_degAng -= m_sumDegAng;
			m_sumDegAng += 0.2f;
		}
		else
		{
			m_situationType = SituationType::Down;
			m_degAng = m_maxDegAng;
		}
	}

	if (m_situationType == SituationType::Down)
	{
		if (m_degAng < m_standDegAng)
		{
			m_degAng += m_sumDegAng;
		}
		else
		{
			m_degAng = m_standDegAng;
			m_situationType = SituationType::Stand;
		}
	}

	m_flagMat = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(m_degAng));
	m_flagMat.Translation(m_pos);

	//Math::Vector3 goalPos = (m_spPoleModel->FindNode("FlagPoint")->m_worldTransform * m_mWorld).Translation();

	//Math::Vector3 pos = Math::Vector3::Lerp(m_pos, goalPos, m_progress);

	//m_progress += m_speed;
	//if (m_progress >= 1)
	//{
	//	m_progress = 1;
	//}

	//m_flagMat.Translation(pos);
}

void SavePoint::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_flagMat);
	}
	if (m_spBaseModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spBaseModel, m_mWorld);
	}


	//if (m_spPoleModel)
	//{
	//	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spPoleModel, m_mWorld);
	//}

	//if (m_spFlagModel)
	//{
	//	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spFlagModel, m_flagMat);
	//}
}

void SavePoint::DrawLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_flagMat);
	}
	if (m_spBaseModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spBaseModel, m_mWorld);
	}

	//if (m_spPoleModel)
	//{
	//	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spPoleModel, m_mWorld);
	//}

	//if (m_spFlagModel)
	//{
	//	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spFlagModel, m_flagMat);
	//}
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
	EventObjectBase::Init();

	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/EventObject/SavePoint/savePoint.gltf");
	}

	if (!m_spBaseModel)
	{
		m_spBaseModel = std::make_shared<KdModelData>();
		m_spBaseModel->Load("Asset/Models/EventObject/SavePoint/Base/base.gltf");
	}

	if (!m_spPoleModel)
	{
		m_spPoleModel = std::make_shared<KdModelData>();
		m_spPoleModel->Load("Asset/Models/EventObject/SavePoint/Pole/pole.gltf");
	}

	if (!m_spFlagModel)
	{
		m_spFlagModel = std::make_shared<KdModelData>();
		m_spFlagModel->Load("Asset/Models/EventObject/SavePoint/Flag/flag.gltf");
	}

	m_objectType = ObjectType::SavePoint;

	if (m_situationType == SituationType::NotStand)
	{
		m_pCollider = std::make_unique<KdCollider>();
		//m_pCollider->RegisterCollisionShape("SavePoint", m_spModel, KdCollider::TypeEvent);
		m_pCollider->RegisterCollisionShape("SavePoint", m_spBaseModel, KdCollider::TypeEvent);
	}
}

void SavePoint::OnHit()
{
	if ((m_situationType == SituationType::NotStand) == 0) return;
	std::shared_ptr<KdSoundInstance> se = KdAudioManager::Instance().Play("Asset/Sounds/SE/savePoint.wav");
	se->SetVolume(0.06f);
	m_situationType = SituationType::Up;
}

void SavePoint::SetParam(const Param& _param)
{
	if (_param.modelNum == 1)
	{
		m_situationType = SituationType::Stand;
		m_degAng = m_standDegAng;
	}

	m_param = _param;

	m_pos = m_param.basePos;
	m_respawnPos = m_param.basePos;

	m_flagMat = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(m_degAng));
	m_flagMat.Translation(m_pos);

	m_mWorld.Translation(m_pos);
}
