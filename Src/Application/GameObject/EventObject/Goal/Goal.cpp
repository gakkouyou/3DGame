﻿#include "Goal.h"
#include "Application/main.h"
#include "../../../Scene/SceneManager.h"

void Goal::Update()
{
	if (m_pauseFlg) return;

	if (m_setParamFlg == true)
	{
		m_setParamFlg = false;
		return;
	}

	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_scale);

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
		pos.y += sin(DirectX::XMConvertToRadians(m_sinAngle)) * m_height;
		Application::Instance().m_log.AddLog("%f\n", pos.y);

		// 回転させる
		m_degAng += m_moveDegAng;
		if (m_degAng >= 360.0f)
		{
			m_degAng -= 360.0f;
		}

		Math::Matrix transMat = Math::Matrix::CreateTranslation(pos);

		Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));

		m_mWorld = scaleMat * rotMat * transMat;
	}
	else
	{
		m_goalStayCount++;
		if (m_goalStayCount > m_goalStayTime)
		{
			Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);
			Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_forwardDegAng));

			m_mWorld = scaleMat * rotMat * transMat;
		}
		else
		{
			m_degAng += m_moveDegAng;
			m_moveDegAng += m_addMoveDegAng;

			Math::Vector3 pos = m_pos;
			pos.y += sin(DirectX::XMConvertToRadians(m_sinAngle)) * m_height;

			Math::Matrix transMat = Math::Matrix::CreateTranslation(pos);

			Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));

			m_mWorld = scaleMat * rotMat * transMat;
		}
	}
}

void Goal::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void Goal::DrawLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void Goal::Init()
{
	EventObjectBase::Init();

	// コライダー
	m_pCollider = std::make_unique<KdCollider>();

	m_objectType = ObjectType::Goal;
}

void Goal::OnHit()
{
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

void Goal::SetParam(const Param& _param)
{
	if (m_param.modelNum != _param.modelNum && _param.modelNum != 0)
	{
		switch (_param.modelNum)
		{
		// ステージ１のモデル
		case 1:
			m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/EventObject/Clock/Body/body.gltf");
			m_pCollider->RegisterCollisionShape("Goal", m_spModel, KdCollider::TypeEvent | KdCollider::TypeDebug);
			break;

		// ステージ２のモデル
		case 2:
			m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/EventObject/Clock/Needle/needle.gltf");
			m_pCollider->RegisterCollisionShape("Goal", m_spModel, KdCollider::TypeEvent | KdCollider::TypeDebug);
			break;

		// ステージ３のモデル
		case 3:
			m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/EventObject/Clock/Bell/bell.gltf");
			m_pCollider->RegisterCollisionShape("Goal", m_spModel, KdCollider::TypeEvent | KdCollider::TypeDebug);
			break;

		// 発表用のモデル
		case 4:
			m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/EventObject/Clock/clock.gltf");
			m_pCollider->RegisterCollisionShape("Goal", m_spModel, KdCollider::TypeEvent | KdCollider::TypeDebug);
			break;
		}
	}

	m_param = _param;

	m_pos = _param.basePos;

	m_setParamFlg = false;

	m_mWorld.Translation(m_pos);
}
