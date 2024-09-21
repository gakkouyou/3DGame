#include "Switch.h"

void Switch::Update()
{
	if (m_pauseFlg == true) return;
	// OnHitに入っていなかった時の処理
	if (m_onHitFlg == false)
	{
		m_param.pos.y += m_movePow;
		if (m_param.pos.y >= m_param.startPos.y)
		{
			m_param.pos.y = m_param.startPos.y;
		}
	}
	// OnHitチェックフラグをリセット
	m_onHitFlg = false;

	m_mWorld.Translation(m_param.pos);
}

void Switch::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
	if (m_spBaseModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spBaseModel, m_baseMatrix);
	}
}

void Switch::DrawLit()
{
	if (m_spModel)
	{
		if (m_setParamFlg == true)
		{
			KdShaderManager::Instance().m_StandardShader.SetColorEnable(true);
		}
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
	if (m_spBaseModel)
	{
		if (m_setParamFlg == true)
		{
			KdShaderManager::Instance().m_StandardShader.SetColorEnable(true);
			m_setParamFlg = false;
		}
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spBaseModel, m_baseMatrix);
	}
}

void Switch::Init()
{
	// スイッチのボタンのモデル
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/Object/Switch/Button/button.gltf");
	}

	// スイッチの土台のようなモデル
	if (!m_spBaseModel)
	{
		m_spBaseModel = std::make_shared<KdModelData>();
		m_spBaseModel->Load("Asset/Models/Terrain/Object/Switch/Base/base.gltf");
	}

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("SwitchButton", m_spModel, KdCollider::TypeGround);

	m_pBaseCollider = std::make_unique<KdCollider>();
	m_pBaseCollider->RegisterCollisionShape("SwitchBase", m_spBaseModel, KdCollider::TypeGround);

	// オブジェクトタイプ
	m_objectType = ObjectType::Switch;

	TerrainBase::Init();
}

void Switch::OnHit()
{
	m_onHitFlg = true;
	m_param.pos.y -= m_movePow;
	if (m_param.startPos.y - m_param.pos.y >= m_maxMove)
	{
		m_param.pos.y = m_param.startPos.y - m_maxMove;
		if (m_wpTarget.expired() == false)
		{
			m_wpTarget.lock()->Active();
		}
	}
}

void Switch::SetParam(Param _param)
{
	m_param.pos			= _param.startPos;
	m_param.startPos	= _param.startPos;
	m_param.scale		= _param.scale;
	m_param.targetName	= _param.targetName;

	m_baseMatrix = Math::Matrix::CreateTranslation(m_param.startPos);
	m_mWorld = Math::Matrix::CreateTranslation(m_param.startPos);

	m_setParamFlg = true;
}
