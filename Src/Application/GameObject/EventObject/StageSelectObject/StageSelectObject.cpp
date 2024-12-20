﻿#include "StageSelectObject.h"
#include "../../../Scene/SceneManager.h"
#include "../../Camera/CameraBase.h"
#include "../../UI/StageSelectUI/StageSelectUI.h"

void StageSelectObject::Update()
{
	// 回す
	m_degAng += m_addDegAng;

	if (m_degAng >= 360)
	{
		m_degAng -= 360;
	}

	// パーツの回転
	m_partsDegAng += m_addPartsDegAng;

	if (m_partsDegAng >= 360)
	{
		m_partsDegAng -= 360;
	}

	// プレイヤーが近くにいなかったら小さくする
	if (m_onHitFlg == false)
	{
		m_scale -= m_addScale;
		if (m_scale < m_minScale)
		{
			m_scale = m_minScale;
		}
	}
	// フラグリセット
	m_onHitFlg = false;

	// 行列作成
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_scale);
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_param.basePos);
	m_objMat = scaleMat * rotMat * transMat;

	Math::Matrix partsRotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_partsDegAng));
	m_partsMat = scaleMat * partsRotMat * transMat;
}

void StageSelectObject::GenerateDepthMapFromLight()
{
	// ステージセレクトのモデル
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_objMat);
	}
	// 土台
	if (m_spSphereModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spSphereModel, m_mWorld);
	}
	// 時計のパーツ(クリアの時だけ表示)
	if (SceneManager::Instance().GetStageInfo()[m_param.modelNum - 1] != SceneManager::StageInfo::NotClear)
	{
		if (m_spClockPartsModel)
		{
			KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spClockPartsModel, m_partsMat);
		}
	}
}

void StageSelectObject::DrawLit()
{
	// ステージセレクトのモデル
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_objMat);
	}
	// 土台
	if (m_spSphereModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spSphereModel, m_mWorld);
	}
	// 時計のパーツ(クリアの時だけ表示)
	if (SceneManager::Instance().GetStageInfo()[m_param.modelNum - 1] != SceneManager::StageInfo::NotClear)
	{
		if (m_spClockPartsModel)
		{
			KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spClockPartsModel, m_partsMat);
		}
	}
}

void StageSelectObject::Init()
{
	EventObjectBase::Init();

	// オブジェクトタイプ
	m_objectType = ObjectType::StageSelectObject;

	// 土台のモデル
	if (!m_spSphereModel)
	{
		m_spSphereModel = std::make_shared<KdModelData>();
		m_spSphereModel->Load("Asset/Models/EventObject/StageSelect/baseGround.gltf");
	}

	// コライダー
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("StageSelectObject", m_spSphereModel, KdCollider::TypeGround | KdCollider::TypeEvent | KdCollider::TypeDebug);

	// 拡縮
	m_scale = m_minScale;
}

void StageSelectObject::SetParam(const Param& _param)
{
	if (m_param.modelNum != _param.modelNum && _param.modelNum != 0)
	{
		m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/EventObject/StageSelect/Stage" + std::to_string(_param.modelNum) + "/StageObject/stage" + std::to_string(_param.modelNum) + ".gltf");
		m_spClockPartsModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/EventObject/StageSelect/Stage" + std::to_string(_param.modelNum) + "/ClockParts/clockParts.gltf");
	}

	m_param = _param;

	m_mWorld.Translation(m_param.basePos);
}

void StageSelectObject::OnHit()
{
	// ステージを決める
	SceneManager::Instance().SetNowStage(m_param.modelNum);
	
	m_scale += m_addScale;
	if (m_scale > m_maxScale)
	{
		m_scale = m_maxScale;
	}

	m_onHitFlg = true;

	if (m_wpStageSelectUI.expired() == false)
	{
		m_wpStageSelectUI.lock()->OnHit();
	}
}
