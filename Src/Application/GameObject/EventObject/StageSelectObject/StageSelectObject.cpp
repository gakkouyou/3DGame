#include "StageSelectObject.h"
#include "../../../Scene/SceneManager.h"

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
	if (m_param.modelNum != _param.modelNum && _param.modelNum != 0)
	{
		m_spModel->Load("Asset/Models/EventObject/StageSelect/Stage" + std::to_string(_param.modelNum) + "/stage" + std::to_string(_param.modelNum) + ".gltf");
		m_pCollider->RegisterCollisionShape("StageSelectObject" + std::to_string(_param.modelNum), m_spModel, KdCollider::TypeEvent | KdCollider::TypeGround);
	}

	m_param = _param;

	m_mWorld.Translation(m_param.basePos);
}

void StageSelectObject::OnHit()
{
	// ステージを決める
	SceneManager::Instance().SetNowStage(m_param.modelNum);
}
