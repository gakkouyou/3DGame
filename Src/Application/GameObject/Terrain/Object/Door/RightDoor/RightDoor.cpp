#include "RightDoor.h"
#include "../LeftDoor/LeftDoor.h"
#include "../DoorWall/DoorWall.h"
#include "../../../../../Scene/SceneManager.h"

void RightDoor::Update()
{
	if (m_pauseFlg == true) return;

	if (!m_activeFlg)
	{
		m_param.pos.x -= 0.05f;
		if (m_param.pos.x < m_param.startPos.x)
		{
			m_param.pos.x = m_param.startPos.x;
		}
	}
	m_activeFlg = false;

	m_mWorld.Translation(m_param.pos);
}

void RightDoor::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/Object/Door/RightDoor/rightDoor.gltf");
	}

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("RightDoor", m_spModel, KdCollider::TypeGround);

	m_drawType = eDrawTypeLit | eDrawTypeDepthOfShadow;

	// オブジェクトタイプ
	m_objectType = ObjectType::Door;

	TerrainBase::Init();

	// 左のドア
	std::shared_ptr<LeftDoor> leftDoor = std::make_shared<LeftDoor>();
	leftDoor->Init();
	SceneManager::Instance().AddObject(leftDoor);
	// 保持
	m_wpLeftDoor = leftDoor;

	// ドアの壁
	std::shared_ptr<DoorWall> doorWall = std::make_shared<DoorWall>();
	doorWall->Init();
	SceneManager::Instance().AddObject(doorWall);
	// 保持
	m_wpDoorWall = doorWall;
}

void RightDoor::Active()
{
	if (m_activeFlg == true) return;
	m_activeFlg = true;
	m_param.pos.x += 0.05f;
	if (m_param.pos.x - m_param.startPos.x >= 3.0f)
	{
		m_param.pos.x = m_param.startPos.x + 3.0f;
	}
	if (m_wpLeftDoor.expired() == false)
	{
		m_wpLeftDoor.lock()->Active();
	}
}

void RightDoor::SetParam(Param _param)
{
	if (m_setParamFlg == true) return;
	m_param.pos = _param.startPos;
	m_param.startPos = _param.startPos;
	m_param.scale = _param.scale;

	m_setParamFlg = true;

	m_mWorld = Math::Matrix::CreateScale(m_param.scale);

	m_mWorld.Translation(m_param.pos);

	// 連携しているオブジェクトにも渡す
	if (m_wpLeftDoor.expired() == false)
	{
		m_wpLeftDoor.lock()->SetParam(_param);
	}
	if (m_wpDoorWall.expired() == false)
	{
		m_wpDoorWall.lock()->SetParam(_param);
	}
}
