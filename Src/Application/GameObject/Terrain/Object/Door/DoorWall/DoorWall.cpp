#include "DoorWall.h"

void DoorWall::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/Object/Door/DoorWall/DoorWall.gltf");
	}

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("DoorWall", m_spModel, KdCollider::TypeGround);

	m_drawType = eDrawTypeLit | eDrawTypeDepthOfShadow;

	// オブジェクトタイプ
	m_objectType = ObjectType::Door;

	TerrainBase::Init();

	// アルファディザ有効
	m_ditherFlg = true;
}

void DoorWall::SetParam(const Param& _param)
{
	if (m_setParamFlg == true) return;
	m_param.pos = _param.startPos;
	m_param.startPos = _param.startPos;
	m_param.scale = _param.scale;

	m_setParamFlg = true;

	m_mWorld = Math::Matrix::CreateScale(m_param.scale);

	m_mWorld.Translation(m_param.pos);

	// 連携しているオブジェクトにも渡す
	if (m_wpRightDoor.expired() == false)
	{
		m_wpRightDoor.lock()->SetParam(_param);
	}
	if (m_wpLeftDoor.expired() == false)
	{
		m_wpLeftDoor.lock()->SetParam(_param);
	}
}

void DoorWall::Active()
{
	if (m_wpRightDoor.expired() == false)
	{
		m_wpRightDoor.lock()->Active();
	}
	if (m_wpLeftDoor.expired() == false)
	{
		m_wpLeftDoor.lock()->Active();
	}
}
