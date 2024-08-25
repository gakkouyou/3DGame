#include "Goal.h"
#include "Application/main.h"

void Goal::Update()
{
	if (!m_goalFlg)
	{
		// 上下にふよふよさせる
		// sinカーブ
		m_sinAngle += 1.0f;
		if (m_sinAngle >= 360)
		{
			m_sinAngle -= 360;
		}

		Math::Vector3 pos = m_pos;
		pos.y += sin(DirectX::XMConvertToRadians(m_sinAngle));

		// 回転させる
		m_degAng += m_moveDegAng;
		if (m_degAng >= 360.0f)
		{
			m_degAng -= 360.0f;
		}

		Math::Matrix transMat = Math::Matrix::CreateTranslation(pos);

		Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));

		m_mWorld = rotMat * transMat;
	}
	else
	{
		m_goalStayCnt++;
		if (m_goalStayCnt > m_goalStayTime)
		{
			Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);

			m_mWorld = transMat;
		}
	}
}

void Goal::DrawUnLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void Goal::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/EventObject/Goal/goal.gltf");
	}

	// コライダー
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Goal", m_spModel, KdCollider::TypeEvent);

	m_baseObjectType = BaseObjectType::Event;
}

void Goal::OnHit()
{
	Application::Instance().m_log.Clear();
	Application::Instance().m_log.AddLog("Clear");

	m_goalFlg	= true;
}

void Goal::Reset()
{
	// 少し回転させる
	m_moveDegAng	= 1.0f;
	m_degAng		= 0.0f;

	// 上下にふよふよさせる
	m_sinAngle = 0;
}
