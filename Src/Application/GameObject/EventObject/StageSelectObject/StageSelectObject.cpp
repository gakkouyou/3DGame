#include "StageSelectObject.h"
#include "../../../Scene/SceneManager.h"
#include "../../Camera/CameraBase.h"

void StageSelectObject::Update()
{
	m_degAng += m_addDegAng;

	if (m_degAng >= 360)
	{
		m_degAng -= 360;
	}

	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_param.basePos);
	m_objMat = rotMat * transMat;
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
	//Math::Vector3 pos = GetPos();
	//pos.y += 3.f;

	//std::shared_ptr<KdCamera> spCamera;
	//if (m_wpCamera.expired() == false)
	//{
	//	spCamera = m_wpCamera.lock()->GetCamera();
	//}

	//// 2D座標
	//Math::Vector3 resultPos;
	//if (spCamera)
	//{
	//	spCamera->ConvertWorldToScreenDetail(pos, resultPos);
	//}

	//if (m_onHitFlg)
	//{
	//	if (m_spSpeechBubbleTex)
	//	{
	//		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spSpeechBubbleTex, resultPos.x, resultPos.y);
	//	}
	///*	if (m_spStageTex)
	//	{
	//		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spStageTex, resultPos.x, resultPos.y);
	//	}*/
	//}
	//m_onHitFlg = false;
}

void StageSelectObject::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
	}

	EventObjectBase::Init();

	m_objectType = ObjectType::StageSelectObject;

	if (!m_spSphereModel)
	{
		m_spSphereModel = std::make_shared<KdModelData>();
		m_spSphereModel->Load("Asset/Models/EventObject/StageSelect/sphere.gltf");
	}

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("StageSelectObject", m_spSphereModel, KdCollider::TypeGround);

	if (!m_spSpeechBubbleTex)
	{
		m_spSpeechBubbleTex = std::make_shared<KdTexture>();
		m_spSpeechBubbleTex->Load("Asset/Textures/StageSelect/UI/stage1Photo.png");
	}

	if (!m_spStageTex)
	{
		m_spStageTex = std::make_shared<KdTexture>();
		m_spStageTex->Load("Asset/Textures/StageSelect/UI/stage1.png");
	}
}

void StageSelectObject::SetParam(const Param& _param)
{
	if (m_param.modelNum != _param.modelNum && _param.modelNum != 0)
	{
		m_spModel->Load("Asset/Models/EventObject/StageSelect/Stage" + std::to_string(_param.modelNum) + "/stage" + std::to_string(_param.modelNum) + ".gltf");
		//m_pCollider->RegisterCollisionShape("StageSelectObject" + std::to_string(_param.modelNum), m_spModel, KdCollider::TypeEvent | KdCollider::TypeGround);
	}

	m_param = _param;

	m_mWorld.Translation(m_param.basePos);
}

void StageSelectObject::OnHit()
{
	// ステージを決める
	SceneManager::Instance().SetNowStage(m_param.modelNum);
	m_onHitFlg = true;
}
