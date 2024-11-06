#include "FinalGoal.h"

void FinalGoal::Update()
{
	if (m_param.modelNum == 3 || m_param.modelNum == 4)
	{
		// 時計の音を鳴らす
		if (!m_flg)
		{
			if (m_wpSound.expired() == false)
			{
				m_wpSound.lock()->Play(true);
			}
			m_flg = true;
		}
		// 座標を入れる
		m_wpSound.lock()->SetPos(m_pos);
	}
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

	// コライダー
	m_pCollider = std::make_unique<KdCollider>();

	m_objectType = ObjectType::FinalGoal;

	m_wpSound = KdAudioManager::Instance().Play3D("Asset/Sounds/SE/alarm.wav", m_pos, true);
	if (!m_wpSound.expired())
	{
		m_wpSound.lock()->SetVolume(0.3f);
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
			m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/EventObject/Clock/Body/body.gltf");
			break;

			// ステージ２をクリアしたときのモデル
		case 2:
			m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/EventObject/Clock/BodyNeedle/bodyNeedle.gltf");
			break;

			// ステージ３をクリアしたときのモデル
		case 3:
		case 4:
			m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/EventObject/Clock/clock.gltf");
			m_pCollider->RegisterCollisionShape("FinalGoal", m_spModel, KdCollider::TypeEvent | KdCollider::TypeDebug);
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