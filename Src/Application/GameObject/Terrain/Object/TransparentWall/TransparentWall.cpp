#include "TransparentWall.h"
#include "../../../../Scene/SceneManager.h"

void TransparentWall::DrawLit()
{
	if (SceneManager::Instance().GetDebug() == true)
	{
		if (m_spModel)
		{
			KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
			KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
			KdShaderManager::Instance().UndoRasterizerState();
		}
	}
}

void TransparentWall::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/Object/TransparentWall/transparentWall.gltf");
	}

	m_objectType = ObjectType::TransparentWall;

	TerrainBase::Init();

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("TransParentWall", m_spModel, KdCollider::TypeGround);
}

void TransparentWall::SetParam(const Param& _param)
{
	m_param.pos			= _param.startPos;
	m_param.startPos	= _param.startPos;
	m_param.scale		= _param.scale;
	m_param.degAng		= _param.degAng;
	m_param.targetName	= _param.targetName;
	m_param.yetActive	= _param.yetActive;

	if (m_param.yetActive == true)
	{
		// 当たり判定をなくす
		m_pCollider->SetEnable("TransParentWall", false);
	}

	Math::Matrix transMat	= Math::Matrix::CreateTranslation(m_param.pos);
	Math::Matrix rotMat		= Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_param.degAng.y), DirectX::XMConvertToRadians(m_param.degAng.x), DirectX::XMConvertToRadians(m_param.degAng.z));
	Math::Matrix scaleMat	= Math::Matrix::CreateScale(m_param.scale);

	m_mWorld = scaleMat * rotMat * transMat;
}

void TransparentWall::Active()
{
	m_param.yetActive = true;
	// 当たり判定をなくす
	m_pCollider->SetEnable("TransParentWall", false);
}