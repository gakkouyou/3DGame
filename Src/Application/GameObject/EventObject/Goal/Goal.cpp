#include "Goal.h"
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
		pos.y += sin(DirectX::XMConvertToRadians(m_sinAngle));

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
			Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(90));

			m_mWorld = scaleMat * rotMat * transMat;
		}
	}

	if (m_flg == false)
	{
		m_wpSound.lock()->SetPos(m_pos);
		//m_wpSound.lock()->Play(true);
		m_flg = true;
	}
}

void Goal::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
		KdShaderManager::Instance().UndoRasterizerState();
	}
}

void Goal::DrawLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
		KdShaderManager::Instance().UndoRasterizerState();
	}
}

void Goal::Init()
{
	EventObjectBase::Init();

	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
	}

	// コライダー
	m_pCollider = std::make_unique<KdCollider>();

	m_objectType = ObjectType::Goal;

	m_wpSound = KdAudioManager::Instance().Play3D("Asset/Sounds/SE/clock.wav", m_pos, true);
	if (!m_wpSound.expired())
	{
		m_wpSound.lock()->SetVolume(0.1f);
		m_wpSound.lock()->Stop();
	}
	m_flg = false;
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
			m_spModel->Load("Asset/Models/EventObject/Clock/Body/body.gltf");
			m_pCollider->RegisterCollisionShape("Goal", m_spModel, KdCollider::TypeEvent);
			break;

		// ステージ２のモデル
		case 2:
			m_spModel->Load("Asset/Models/EventObject/Clock/Needle/needle.gltf");
			m_pCollider->RegisterCollisionShape("Goal", m_spModel, KdCollider::TypeEvent);
			break;

		// ステージ３のモデル
		case 3:
			m_spModel->Load("Asset/Models/EventObject/Clock/Bell/bell.gltf");
			m_pCollider->RegisterCollisionShape("Goal", m_spModel, KdCollider::TypeEvent);
			break;
		}
	}

	m_param = _param;

	m_pos = _param.basePos;

	m_setParamFlg = false;

	m_mWorld.Translation(m_pos);
}
