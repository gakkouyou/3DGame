#include "MouseClickHit.h"
#include "../../Scene/SceneManager.h"
#include "../../GameObject/Camera/CameraBase.h"
#include "../DebugWindow/DebugWindow.h"
#include "../../main.h"

#include "../../GameObject/Terrain/TerrainBase.h"
#include "../../GameObject/EventObject/EventObjectBase.h"
#include "../../GameObject/Character/Enemy/EnemyBase.h"
#include "../../GameObject/Terrain/CarryObject/CarryObjectBase.h"

#include "../ObjectController/TerrainController/TerrainController.h"
#include "../ObjectController/EventObjectController/EventObjectController.h"
#include "../ObjectController/EnemyController/EnemyController.h"
#include "../ObjectController/CarryObjectController/CarryObjectController.h"

void MouseClickHit::Update()
{
	if (SceneManager::Instance().GetDebug() == false) return;

	// クリックしたら選んだオブジェクトをセットする
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		if (m_rightClickFlg == true) return;
		m_rightClickFlg = true;

		ClickHit();
	}
	else
	{
		m_rightClickFlg = false;
	}
}

void MouseClickHit::ClickHit()
{
	// マウスでオブジェクトを選択する
	std::shared_ptr<const CameraBase> spCamera = m_wpCamera.lock();

	// カメラが無かったら終了
	if (!spCamera) return;

	// マウス位置の取得
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(Application::Instance().GetWindowHandle(), &mousePos);

	Math::Vector3	cameraPos = spCamera->GetPos();
	Math::Vector3	rayDir = Math::Vector3::Zero;
	float			rayRange = 100.0f;

	// レイの方向取得
	spCamera->GetCamera()->GenerateRayInfoFromClientPos(mousePos, cameraPos, rayDir, rayRange);

	Math::Vector3 endRayPos = cameraPos + (rayDir * rayRange);

	KdCollider::RayInfo rayInfo(KdCollider::TypeDebug, cameraPos, endRayPos);

	// 当たり判定の結果
	std::list<KdCollider::CollisionResult> resultList;

	// 当たったオブジェクトのリスト
	std::vector<std::shared_ptr<KdGameObject>> hitObjList;

	// レイ判定
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		if (obj->Intersects(rayInfo, &resultList))
		{
			hitObjList.push_back(obj);
		}
	}

	float maxOverLap = 0;
	bool hitFlg = false;
	int count = 0;

	// 一番近いオブジェクト
	std::shared_ptr<KdGameObject> hitObj = nullptr;

	// 一番近いオブジェクトを探す
	for (auto& result : resultList)
	{
		if (result.m_overlapDistance > maxOverLap)
		{
			maxOverLap = result.m_overlapDistance;
			hitFlg = true;
			hitObj = hitObjList[count];
		}
		count++;
	}

	// 確定
	ConfirmedObject(hitObj->GetBaseObjectType());

	switch (hitObj->GetBaseObjectType())
	{
	case BaseObjectType::Ground:
		if (m_wpTerrainController.expired() == false)
		{
			for (auto& obj : m_wpTerrainController.lock()->GetObjList())
			{
				if (hitObj->GetObjectName() == obj.lock()->GetObjectName())
				{
					m_wpTerrainController.lock()->SetObject(obj);
					break;
				}
			}
		}
		break;

	case BaseObjectType::Event:
		if (m_wpEventController.expired() == false)
		{
			for (auto& obj : m_wpEventController.lock()->GetObjList())
			{
				if (hitObj->GetObjectName() == obj.lock()->GetObjectName())
				{
					m_wpEventController.lock()->SetObject(obj);
					break;
				}
			}
		}
		break;

	case BaseObjectType::Enemy:
		if (m_wpEnemyController.expired() == false)
		{
			for (auto& obj : m_wpEnemyController.lock()->GetObjList())
			{
				if (hitObj->GetObjectName() == obj.lock()->GetObjectName())
				{
					m_wpEnemyController.lock()->SetObject(obj);
					break;
				}
			}
		}
		break;

	case BaseObjectType::CarryObject:
		if (m_wpCarryObjectController.expired() == false)
		{
			for (auto& obj : m_wpCarryObjectController.lock()->GetObjList())
			{
				if (hitObj->GetObjectName() == obj.lock()->GetObjectName())
				{
					m_wpCarryObjectController.lock()->SetObject(obj);
					break;
				}
			}
		}
		break;
	}
}

void MouseClickHit::ConfirmedObject(BaseObjectType _objectType)
{
	if (_objectType != BaseObjectType::Ground)
	{
		if (m_wpTerrainController.expired() == false)
		{
			m_wpTerrainController.lock()->ConfirmedObject();
		}
	}
	if (_objectType != BaseObjectType::Event)
	{
		if (m_wpEventController.expired() == false)
		{
			m_wpEventController.lock()->ConfirmedObject();
		}
	}
	if (_objectType != BaseObjectType::Enemy)
	{
		if (m_wpEnemyController.expired() == false)
		{
			m_wpEnemyController.lock()->ConfirmedObject();
		}
	}
	if (_objectType != BaseObjectType::CarryObject)
	{
		if (m_wpCarryObjectController.expired() == false)
		{
			m_wpCarryObjectController.lock()->ConfirmedObject();
		}
	}
}