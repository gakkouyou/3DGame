#include "MousePosBox.h"
#include "../../GameObject/Camera/TPSCamera/TPSCamera.h"
#include "../../Scene/SceneManager.h"

void MousePosBox::Update()
{
	// デバッグモードじゃなくなったら消す
	if (!SceneManager::Instance().GetDebug())
	{
		m_isExpired = true;
	}

	// カメラの行列ゲット
	Math::Matrix cameraTransMat = Math::Matrix::Identity;
	Math::Vector3 vec			= Math::Vector3::Zero;
	std::shared_ptr<TPSCamera> spCamera = m_wpCamera.lock();
	if (spCamera)
	{
		cameraTransMat	= Math::Matrix::CreateTranslation(spCamera->GetPos());
		vec				= Math::Vector3::TransformNormal(m_localMat.Translation(), spCamera->GetRotationMatrix());
	}
	
	Math::Matrix transMat	= Math::Matrix::CreateTranslation(vec);

	Math::Matrix rotMat		= spCamera->GetRotationMatrix();

	// 行列確定
	m_mWorld = rotMat * cameraTransMat * transMat;

	//m_pDebugWire->AddDebugSphere(GetPos(), 5.0f);
}

void MousePosBox::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Debug/mousePos.gltf");
	}

	// ローカル行列
	m_localMat = Math::Matrix::CreateTranslation(0, 0.0f, 20.0f);

	m_pCollider = std::make_unique<KdCollider>();
	//m_pCollider->RegisterCollisionShape("MousePosBox", m_spModel, KdCollider::TypeDebug);

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
}

void MousePosBox::DrawLit()
{
	//KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}
