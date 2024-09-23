#include "SwitchBaseModel.h"

void SwitchBaseModel::Init()
{
	// スイッチの土台のようなモデル
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/Object/Switch/Base/base.gltf");
	}

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("SwitchBase", m_spModel, KdCollider::TypeGround);

	// オブジェクトタイプ
	m_objectType = ObjectType::Switch;

	m_drawType = eDrawTypeLit | eDrawTypeDepthOfShadow;

	TerrainBase::Init();
}

void SwitchBaseModel::SetParam(Param _param)
{
	if (m_setParamFlg == true) return;
	
	m_param.pos = _param.startPos;
	m_param.startPos = _param.startPos;
	m_param.scale = _param.scale;
	m_param.targetName = _param.targetName;

	m_mWorld = Math::Matrix::CreateScale(m_param.scale) * Math::Matrix::CreateTranslation(m_param.startPos);

	m_setParamFlg = true;

	if (m_wpSwitch.expired() == false)
	{
		m_wpSwitch.lock()->SetParam(_param);
	}
}
