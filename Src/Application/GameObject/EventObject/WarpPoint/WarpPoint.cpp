#include "WarpPoint.h"

void WarpPoint::Update()
{
}

void WarpPoint::DrawUnLit()
{
	if (m_spInModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spInModel, m_mWorld);
	}
	if (m_spOutModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spOutModel, m_outMat);
	}
}

void WarpPoint::Init()
{
	EventObjectBase::Init();

	if (!m_spInModel)
	{
		m_spInModel = std::make_shared<KdModelData>();
		m_spInModel->Load("Asset/Models/EventObject/WarpPoint/In/in.gltf");
	}

	if (!m_spOutModel)
	{
		m_spOutModel = std::make_shared<KdModelData>();
		m_spOutModel->Load("Asset/Models/EventObject/WarpPoint/Out/out.gltf");
	}

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("WarpPoint", m_spInModel, KdCollider::TypeEvent | KdCollider::TypeDebug);

	m_objectType = ObjectType::WarpPoint;
}

void WarpPoint::OnHit()
{
}

void WarpPoint::SetParam(const Param& _param)
{
	m_param = _param;

	// ワープの入り口の座標
	m_inPos		= m_param.basePos;
	// ワープの出口の座標
	m_outPos	= m_param.secondPos;

	m_mWorld.Translation(m_inPos);
	m_outMat.Translation(m_outPos);
}
