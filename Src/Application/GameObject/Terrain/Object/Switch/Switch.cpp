﻿#include "Switch.h"
#include "SwitchBaseModel/SwitchBaseModel.h"
#include "../../../../Scene/SceneManager.h"
#include "../../../../Tool/ObjectController/TerrainController/TerrainController.h"

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

void Switch::Init()
{
	// スイッチのボタンのモデル
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/Object/Switch/Button/button.gltf");
	}

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("SwitchButton", m_spModel, KdCollider::TypeGround);

	// オブジェクトタイプ
	m_objectType = ObjectType::Switch;

	m_drawType = eDrawTypeLit | eDrawTypeDepthOfShadow;

	TerrainBase::Init();

	// スイッチの土台を作成
	std::shared_ptr<SwitchBaseModel> switchBaseModel = std::make_shared<SwitchBaseModel>();
	switchBaseModel->Init();
	// シーンのオブジェクトリストに追加
	SceneManager::Instance().AddObject(switchBaseModel);
	// 保持
	m_wpBase = switchBaseModel;
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

void Switch::Reset()
{
	m_onHitFlg = false;
	m_param.pos = m_param.startPos;
	m_pauseFlg = false;
}

void Switch::SetParam(const Param& _param)
{
	if (m_setParamFlg == true) return;

	m_param.pos			= _param.startPos;
	m_param.startPos	= _param.startPos;
	m_param.scale		= _param.scale;
	m_param.targetName	= _param.targetName;

	m_mWorld = Math::Matrix::CreateScale(m_param.scale) * Math::Matrix::CreateTranslation(m_param.startPos);

	m_setParamFlg = true;

	// スイッチの土台にも入れる
	if (m_wpBase.expired() == false)
	{
		m_wpBase.lock()->SetParam(_param);
	}
}
