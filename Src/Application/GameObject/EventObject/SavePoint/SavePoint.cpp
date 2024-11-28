#include "SavePoint.h"

void SavePoint::Update()
{
	// ポーズ中なら更新しない
	if (m_pauseFlg == true) return;

	if (m_situationType == SituationType::Stand) return;
	if (m_situationType == SituationType::Up)
	{
		if (m_degAng > m_maxDegAng)
		{
			m_degAng -= m_sumDegAng;
			m_sumDegAng += m_addSumDegAng;
		}
		else
		{
			m_situationType = SituationType::Down;
			m_degAng = m_maxDegAng;
			m_sumDegAng = m_baseSumDegAng;
		}
	}

	if (m_situationType == SituationType::Down)
	{
		if (m_degAng < m_minDegAng)
		{
			m_degAng += m_sumDegAng;
			m_sumDegAng += m_addSumDegAng;
		}
		else
		{
			m_degAng = m_minDegAng;
			m_situationType = SituationType::Up2;
			m_sumDegAng = m_baseSumDegAng;
		}
	}

	if (m_situationType == SituationType::Up2)
	{
		if (m_degAng > m_standDegAng)
		{
			m_degAng -= m_sumDegAng;
			m_sumDegAng += m_addSumDegAng;
		}
		else
		{
			m_degAng = m_standDegAng;
			m_situationType = SituationType::Stand;
		}
	}

	m_flagMat = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(m_degAng));
	m_flagMat.Translation(m_pos);

	// エフェクト
	// 立っていたら終了
	if (m_situationType != SituationType::NotStand) return;
	// 少しずつ大きくする
	m_effectScale += m_addEffectScale;
	// リセット
	if (m_effectScale >= 1.5f)
	{
		m_effectScale = 0.0f;
	}
	Math::Matrix scaleMat = Math::Matrix::CreateScale({ m_effectScale, 1.0f, m_effectScale });
	m_effectMat = scaleMat;
	m_effectMat.Translation(m_pos);
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
	if (m_situationType != SituationType::NotStand) return;
	if (m_spEffectModel)
	{
		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spEffectModel, m_effectMat);
		KdShaderManager::Instance().UndoRasterizerState();
	}
}

void SavePoint::DrawBright()
{
	if (m_situationType != SituationType::NotStand) return;
	Math::Color color = { 0.3, 0.3, 0.3, 1 };
	if (m_spEffectModel)
	{
		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spEffectModel, m_effectMat, color);
		KdShaderManager::Instance().UndoRasterizerState();
	}
}

void SavePoint::Init()
{
	EventObjectBase::Init();

	// 旗
	m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/EventObject/SavePoint/savePoint.gltf");

	// 土台
	m_spBaseModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/EventObject/SavePoint/Base/base.gltf");

	// エフェクト
	m_spEffectModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/EventObject/SavePoint/Effect/effect.gltf");

	m_objectType = ObjectType::SavePoint;

	if (m_situationType == SituationType::NotStand)
	{
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("SavePoint", m_spBaseModel, KdCollider::TypeEvent | KdCollider::TypeDebug);
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
