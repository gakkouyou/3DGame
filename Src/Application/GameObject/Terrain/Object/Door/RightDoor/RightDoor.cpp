﻿#include "RightDoor.h"
#include "../LeftDoor/LeftDoor.h"
#include "../DoorWall/DoorWall.h"
#include "../../../../../Scene/SceneManager.h"
#include "../../../../Character/Player/Player.h"

void RightDoor::Update()
{
	if (m_pauseFlg == true) return;

	if (!m_activeFlg)
	{
		m_param.pos.x -= 0.05f;
		if (m_param.pos.x < m_param.startPos.x)
		{
			m_param.pos.x = m_param.startPos.x;

			//// ドアが閉まりきったらプレイヤーとレイ判定
			//KdCollider::RayInfo rayInfo;
			//rayInfo.m_dir = Math::Vector3::Left;
			//rayInfo.m_pos = m_param.startPos;
			//rayInfo.m_pos.y += 0.2f;
			//rayInfo.m_range = 0.3f;
			//rayInfo.m_type = KdCollider::TypeDamageLine;

			//KdCollider::SphereInfo sphereInfo;
			//sphereInfo.m_sphere.Center = rayInfo.m_pos;
			//sphereInfo.m_sphere.Radius = 0.01f;
			//sphereInfo.m_type = KdCollider::TypeDamageLine;
			//m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, 0.01f);

			////m_pDebugWire->AddDebugLine(rayInfo.m_pos, rayInfo.m_dir, rayInfo.m_range);

			//for (auto& obj : SceneManager::Instance().GetObjList())
			//{
			//	if (obj->Intersects(sphereInfo, nullptr))
			//	{
			//		obj->OnHit();
			//		break;
			//	}
			//}


		}
	}
	m_activeFlg = false;

	m_mWorld.Translation(m_param.pos);
}

void RightDoor::Init()
{
	m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Terrain/Object/Door/RightDoor/rightDoor.gltf");

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("RightDoor", m_spModel, KdCollider::TypeGround | KdCollider::TypeDebug);

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

	// アルファディザ有効
	m_ditherFlg = true;

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
}

void RightDoor::Reset()
{
	m_activeFlg = false;
	m_param.pos = m_param.startPos;

	m_pauseFlg = false;
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

void RightDoor::SetParam(const Param& _param)
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
