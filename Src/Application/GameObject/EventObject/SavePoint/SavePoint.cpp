#include "SavePoint.h"

void SavePoint::Update()
{
	if (m_progress >= 1 || m_aliveFlg == true) return;
	if (m_aliveFlg == false && m_degAng > 0)
	{
		m_degAng -= m_sumDegAng;
	}
	else if(m_aliveFlg == false && m_degAng <= 0)
	{
		m_degAng = 0;
	}

	m_mWorld = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(m_degAng));
	m_mWorld.Translation(m_pos);

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
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
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
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
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

	if (m_aliveFlg == true)
	{
		m_pCollider = std::make_unique<KdCollider>();
		//m_pCollider->RegisterCollisionShape("SavePoint", m_spModel, KdCollider::TypeEvent);
		m_pCollider->RegisterCollisionShape("SavePoint", m_spPoleModel, KdCollider::TypeEvent);
	}
	else
	{
		m_flagMat = m_spPoleModel->FindNode("FlagPoint")->m_worldTransform * m_mWorld;
		m_progress = 1.0f;
	}
}

void SavePoint::OnHit()
{
	if (m_aliveFlg == false) return;
	std::shared_ptr<KdSoundInstance> se = KdAudioManager::Instance().Play("Asset/Sounds/SE/savePoint.wav");
	se->SetVolume(0.06f);
	m_aliveFlg = false;
}

void SavePoint::SetParam(const Param& _param)
{
	if (_param.modelNum == 1)
	{
		m_aliveFlg = false;
		m_degAng = 0;
	}

	m_param = _param;

	m_pos = m_param.basePos;
	m_respawnPos = m_param.basePos;

	m_mWorld = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(m_degAng));
	m_mWorld.Translation(m_pos);

	m_flagMat.Translation(m_pos);
}
