#include "FinalGoal.h"

void FinalGoal::Update()
{
}

void FinalGoal::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void FinalGoal::DrawLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void FinalGoal::Init()
{
	EventObjectBase::Init();

	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
	}

	// コライダー
	m_pCollider = std::make_unique<KdCollider>();

	m_objectType = ObjectType::FinalGoal;

	m_wpSound = KdAudioManager::Instance().Play3D("Asset/Sounds/SE/clock.wav", m_pos, true);
	if (!m_wpSound.expired())
	{
		m_wpSound.lock()->SetVolume(0.1f);
		m_wpSound.lock()->Stop();
	}
	m_flg = false;
}

void FinalGoal::OnHit()
{
}

void FinalGoal::SetParam(const Param& _param)
{
	if (m_param.modelNum != _param.modelNum && _param.modelNum != 0)
	{
		switch (_param.modelNum)
		{
		// ステージ１をクリアしたときのモデル
		case 1:
			m_spModel->Load("Asset/Models/EventObject/Clock/Body/body.gltf");
			break;

			// ステージ２をクリアしたときのモデル
		case 2:
			m_spModel->Load("Asset/Models/EventObject/Clock/BodyNeedle/bodyNeedle.gltf");
			break;

			// ステージ３をクリアしたときのモデル
		case 3:
			m_spModel->Load("Asset/Models/EventObject/Clock/clock.gltf");
			m_pCollider->RegisterCollisionShape("FinalGoal", m_spModel, KdCollider::TypeEvent);
			break;
		}
	}

	m_param = _param;

	m_pos = _param.basePos;

	m_setParamFlg = false;

	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_scale);
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(90));
	m_mWorld = scaleMat * rotMat;
	m_mWorld.Translation(m_pos);
}