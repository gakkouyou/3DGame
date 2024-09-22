#include "Door.h"

void Door::Update()
{
	if (m_pauseFlg == true) return;

	if (!m_activeFlg)
	{
		m_param.pos.x -= 0.05f;
		m_leftPos.x += 0.05f;
		if (m_param.pos.x < m_param.startPos.x)
		{
			m_param.pos.x = m_param.startPos.x;
			m_leftPos.x = m_param.startPos.x;
		}
	}
	m_activeFlg = false;

	m_mWorld.Translation(m_param.pos);

	m_leftMat.Translation(m_leftPos);
}

void Door::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spLeftDoorModel, m_leftMat);
	}
	if (m_spWallModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spWallModel, m_baseMat);
	}
	
}

void Door::DrawLit()
{
	if (m_spModel)
	{
		if (m_setParamFlg == true)
		{
			KdShaderManager::Instance().m_StandardShader.SetColorEnable(true);
		}
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
		if (m_setParamFlg == true)
		{
			KdShaderManager::Instance().m_StandardShader.SetColorEnable(true);
		}
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spLeftDoorModel, m_leftMat);
	}

	if (m_spWallModel)
	{
		if (m_setParamFlg == true)
		{
			KdShaderManager::Instance().m_StandardShader.SetColorEnable(true);
			m_setParamFlg = false;
		}
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spWallModel, m_baseMat);
	}
}

void Door::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/Object/Door/RightDoor/rightDoor.gltf");
	}

	if (!m_spLeftDoorModel)
	{
		m_spLeftDoorModel = std::make_shared<KdModelData>();
		m_spLeftDoorModel->Load("Asset/Models/Terrain/Object/Door/LeftDoor/leftDoor.gltf");
	}

	if (!m_spWallModel)
	{
		m_spWallModel = std::make_shared<KdModelData>();
		m_spWallModel->Load("Asset/Models/Terrain/Wall/DoorWall/doorWall.gltf");
	}

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("RightDoor", m_spModel, KdCollider::TypeGround);
	m_pCollider->RegisterCollisionShape("LeftDoor", m_spLeftDoorModel, KdCollider::TypeGround);
	m_pCollider->RegisterCollisionShape("DoorWall", m_spWallModel, KdCollider::TypeGround);

	m_drawType = eDrawTypeLit | eDrawTypeDepthOfShadow;

	// オブジェクトタイプ
	m_objectType = ObjectType::Door;

	TerrainBase::Init();

	//m_leftMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(180.0f));
}

void Door::Active()
{
 	m_activeFlg = true;
	m_param.pos.x += 0.05f;
	m_leftPos.x -= 0.05f;
	if (m_param.pos.x - m_param.startPos.x >= 3.0f)
	{
		m_param.pos.x = m_param.startPos.x + 3.0f;
		m_leftPos.x = m_param.startPos.x - 3.0f;
	}
}

void Door::SetParam(Param _param)
{
	m_param.pos			= _param.startPos;
	m_param.startPos	= _param.startPos;
	m_leftPos			= _param.startPos;
	m_param.scale		= _param.scale;

	m_setParamFlg = true;

	m_mWorld = Math::Matrix::CreateScale(m_param.scale);
	//m_leftMat = Math::Matrix::CreateScale(m_param.scale) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(180.0f));

	m_mWorld.Translation(m_param.pos);

	m_leftMat.Translation(m_leftPos);

	m_baseMat.Translation(m_param.pos);
}
