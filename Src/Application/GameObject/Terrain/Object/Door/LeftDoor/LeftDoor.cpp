#include "LeftDoor.h"
#include "../../../../../Scene/SceneManager.h"

void LeftDoor::Update()
{
	if (m_pauseFlg == true) return;

	if (!m_activeFlg)
	{
		m_param.pos.x += 0.05f;
		if (m_param.pos.x > m_param.startPos.x)
		{
			m_param.pos.x = m_param.startPos.x;
		}
	}
	m_activeFlg = false;

	m_mWorld.Translation(m_param.pos);
}

void LeftDoor::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/Object/Door/RightDoor/RightDoor.gltf");
	}

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("LeftDoor", m_spModel, KdCollider::TypeGround);

	m_drawType = eDrawTypeLit | eDrawTypeDepthOfShadow;

	// オブジェクトタイプ
	m_objectType = ObjectType::Door;

	TerrainBase::Init();

	// アルファディザ有効
	m_ditherFlg = true;
}

void LeftDoor::Active()
{
	if (m_activeFlg == true) return;
	m_activeFlg = true;
	m_param.pos.x -= 0.05f;
	if (m_param.pos.x - m_param.startPos.x <= -3.0f)
	{
		m_param.pos.x = m_param.startPos.x - 3.0f;
	}

	if (m_wpRightDoor.expired() == false)
	{
		m_wpRightDoor.lock()->Active();
	}
}

void LeftDoor::SetParam(const Param& _param)
{
	if (m_setParamFlg == true) return;
	m_param.pos = _param.startPos;
	m_param.startPos = _param.startPos;
	m_param.scale = _param.scale;

	m_setParamFlg = true;

	m_mWorld = Math::Matrix::CreateScale(m_param.scale) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(180.0f));

	m_mWorld.Translation(m_param.pos);

	// 連携しているオブジェクトにも渡す
	if (m_wpRightDoor.expired() == false)
	{
		m_wpRightDoor.lock()->SetParam(_param);
	}
	if (m_wpDoorWall.expired() == false)
	{
		m_wpDoorWall.lock()->SetParam(_param);
	}
}
