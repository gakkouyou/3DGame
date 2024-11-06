#include "Candy.h"

void Candy::Update()
{
	// 回転させる
	m_degAng += m_moveDegAng;
	if (m_degAng >= 360.0f)
	{
		m_degAng -= 360.0f;
	}

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_param.basePos);

	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_scale);

	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));
	Math::Matrix rotZMat = Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(30.0f));

	m_mWorld = scaleMat * rotZMat * rotMat * transMat;
}

void Candy::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void Candy::DrawLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void Candy::Init()
{
	EventObjectBase::Init();

	if (!m_spModel)
	{

	}

	m_objectType = ObjectType::Candy;

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Candy", m_spModel, KdCollider::TypeEvent);
}

void Candy::OnHit()
{
	m_isExpired = true;
}
