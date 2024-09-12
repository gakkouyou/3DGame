#include "Heart.h"

void Heart::Update()
{

}

void Heart::GenerateDepthMapFromLight()
{
	if (!m_aliveFlg) return;
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void Heart::DrawLit()
{
	if (!m_aliveFlg) return;
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void Heart::DrawBright()
{
	if (!m_aliveFlg) return;
	if (m_spModel)
	{
		Math::Color color = { 0.5f, 0.5f, 0.5f, 1.0f };
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld, color);
	}
}

void Heart::Init()
{
	// モデルロード
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/EventObject/Item/Heart/heart.gltf");
	}

	m_mWorld.Translation({ 0, 2, 3 });

	// コライダー
	if (!m_pCollider)
	{
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("Heart", m_spModel, KdCollider::TypeEvent);
	}

	// オブジェクトのタイプ
	m_objectType = ObjectType::HealItem;

	// 大まかなオブジェクトのタイプ
	m_baseObjectType = BaseObjectType::Event;
}

void Heart::OnHit()
{
	m_aliveFlg = false;
	m_pCollider->SetEnable("Heart", false);
}

void Heart::Reset()
{
	m_aliveFlg = true;
	m_pCollider->SetEnable("Heart", true);
}
