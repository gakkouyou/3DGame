#include "StageSelectObject.h"

void StageSelectObject::Update()
{
}

void StageSelectObject::DrawLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void StageSelectObject::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
	}

	EventObjectBase::Init();

	m_objectType = ObjectType::StageSelectObject;

	m_pCollider = std::make_unique<KdCollider>();
}

void StageSelectObject::SetParam(const Param& _param)
{
	if (m_param.stageNum != _param.stageNum)
	{
		m_spModel->Load("Asset/Models/EventObject/StageSelect/Stage" + std::to_string(_param.stageNum) + "/stage" + std::to_string(_param.stageNum) + ".gltf");
		m_pCollider->RegisterCollisionShape("StageSelectObject" + std::to_string(_param.stageNum), m_spModel, KdCollider::TypeEvent);
	}

	m_param = _param;

	m_mWorld.Translation(m_param.basePos);
}

void StageSelectObject::OnHit()
{

}
