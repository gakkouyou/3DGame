#include "StageSelectObject.h"
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
}

void StageSelectObject::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_objMat);
	}
	if (m_spSphereModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spSphereModel, m_mWorld);
	}
}

void StageSelectObject::DrawLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_objMat);
	}
	if (m_spSphereModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spSphereModel, m_mWorld);
	}


}

void StageSelectObject::DrawSprite()
{

}

void StageSelectObject::Init()
{
	// モデル
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
	}

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
	m_pCollider->RegisterCollisionShape("StageSelectObject", m_spSphereModel, KdCollider::TypeGround | KdCollider::TypeEvent);

	// 拡縮
	m_scale = m_minScale;
}

void StageSelectObject::SetParam(const Param& _param)
{
	if (m_param.modelNum != _param.modelNum && _param.modelNum != 0)
	{
		m_spModel->Load("Asset/Models/EventObject/StageSelect/Stage" + std::to_string(_param.modelNum) + "/stage" + std::to_string(_param.modelNum) + ".gltf");
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
