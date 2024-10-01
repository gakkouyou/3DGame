#include "HealItem.h"

void HealItem::Update()
{

}

void HealItem::GenerateDepthMapFromLight()
{
	if (!m_aliveFlg) return;
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void HealItem::DrawLit()
{
	if (!m_aliveFlg) return;
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void HealItem::DrawBright()
{
	if (!m_aliveFlg) return;
	if (m_spModel)
	{
		Math::Color color = { 0.5f, 0.5f, 0.5f, 1.0f };
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld, color);
	}
}

void HealItem::Init()
{
	EventObjectBase::Init();

	// モデルロード
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/EventObject/Item/Heart/heart.gltf");
	}

	// コライダー
	if (!m_pCollider)
	{
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("HealItem", m_spModel, KdCollider::TypeEvent);
	}

	// オブジェクトのタイプ
	m_objectType = ObjectType::HealItem;
}

void HealItem::OnHit()
{
	m_aliveFlg = false;
	m_pCollider->SetEnable("HealItem", false);
}

void HealItem::Reset()
{
	m_aliveFlg = true;
	m_pCollider->SetEnable("HealItem", true);
}

void HealItem::SetParam(const Param& _param)
{
	m_param = _param;
	m_mWorld.Translation(m_param.basePos);
}
