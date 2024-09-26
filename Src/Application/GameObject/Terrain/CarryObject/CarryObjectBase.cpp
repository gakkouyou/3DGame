#include "CarryObjectBase.h"
#include "../../../Scene/SceneManager.h"
#include "../../Terrain/TerrainBase.h"
#include "../../../Tool/ObjectController/TerrainController/TerrainController.h"

void CarryObjectBase::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void CarryObjectBase::DrawLit()
{
	if (m_spModel)
	{
		if (m_whiteFlg)
		{
			KdShaderManager::Instance().m_StandardShader.SetColorEnable(true);
			m_whiteFlg = false;
		}
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}


// レイ判定　当たったオブジェクトをリストに追加
bool CarryObjectBase::RayHitJudge(const KdCollider::RayInfo& _rayInfo, Math::Vector3& _hitPos, const bool _debugFlg)
{
	// レイに当たったオブジェクト情報を格納するリスト
	std::list<KdCollider::CollisionResult> retRayList;

	// レイに当たったオブジェクトのタイプを格納するリスト
	std::vector<std::weak_ptr<KdGameObject>> hitObjectList;

	// 当たり判定
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		if (obj->Intersects(_rayInfo, &retRayList))
		{
			// 当たったオブジェクトをリストで保持
			hitObjectList.push_back(obj);
		}
	}

	// 一番近いオブジェクトを探す
	bool hit = false;
	float maxOverLap = 0;

	// カウント
	int cnt = 0;

	// 一番近いオブジェクトを保持
	std::weak_ptr<KdGameObject> hitObj;

	for (auto& ret : retRayList)
	{
		if (ret.m_overlapDistance > maxOverLap)
		{
			hit = true;
			maxOverLap = ret.m_overlapDistance;
			_hitPos = ret.m_hitPos;

			hitObj = hitObjectList[cnt];
		}
		cnt++;
	}

	// 一番近かったオブジェクトをリストに追加
	m_wpHitObjectList.push_back(hitObj);

	// デバッグ
	if (_debugFlg)
	{
		if (m_pDebugWire)
		{
			m_pDebugWire->AddDebugLine(_rayInfo.m_pos, _rayInfo.m_dir, _rayInfo.m_range);
		}
	}

	return hit;
}

// レイ判定　地面のみ　当たったオブジェクトを引数に指定した変数に保持
bool CarryObjectBase::RayHitJudge(const KdCollider::RayInfo& _rayInfo, Math::Vector3& _hitPos, std::weak_ptr<TerrainBase>& _hitObject, const bool _debugFlg)
{
	// テレインコントローラーがセットされていなければ終了
	if (m_wpTerrainController.expired()) return false;

	// レイに当たったオブジェクト情報を格納するリスト
	std::list<KdCollider::CollisionResult> retRayList;

	// レイに当たったオブジェクトのタイプを格納するリスト
	std::vector<std::weak_ptr<TerrainBase>> retObjectList;

	// 当たり判定
	for (auto& obj : m_wpTerrainController.lock()->GetObjList())
	{
		if (!obj.expired())
		{
			if (obj.lock()->Intersects(_rayInfo, &retRayList))
			{
				// 当たったオブジェクトをリストで保持
				retObjectList.push_back(obj);
			}
		}
	}

	// 一番近いオブジェクト
	bool hit = false;
	float maxOverLap = 0;

	int cnt = 0;

	for (auto& ret : retRayList)
	{
		if (ret.m_overlapDistance > maxOverLap)
		{
			hit = true;
			maxOverLap = ret.m_overlapDistance;
			_hitPos = ret.m_hitPos;

			// 最も近くにあるオブジェクトを保持
			_hitObject = retObjectList[cnt];
		}
		cnt++;
	}

	// デバッグ
	if (_debugFlg)
	{
		if (m_pDebugWire)
		{
			m_pDebugWire->AddDebugLine(_rayInfo.m_pos, _rayInfo.m_dir, _rayInfo.m_range);
		}
	}

	return hit;
}

bool CarryObjectBase::SphereHitJudge(const KdCollider::SphereInfo& _sphereInfo, KdCollider::CollisionResult& _collisionResult, bool& _multiHit, const bool _debugFlg)
{
	//==================
	// 球判定
	//==================
	// デバッグ用
	if (_debugFlg)
	{
		if (m_pDebugWire)
		{
			m_pDebugWire->AddDebugSphere(_sphereInfo.m_sphere.Center, _sphereInfo.m_sphere.Radius);
		}
	}

	// 球に当たったオブジェクト情報を格納
	std::list<KdCollider::CollisionResult> retSphereList;

	// 球に当たったオブジェクトを格納するリスト
	std::vector<std::weak_ptr<KdGameObject>> retObjList;

	int hitCount = 0;

	// 当たり判定！！！！！！！！！！！！！！！
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		if (obj->Intersects(_sphereInfo, &retSphereList))
		{
			// 当たったオブジェクトをリストで保持
			retObjList.push_back(obj);
			hitCount++;
		}
	}
	// 当たったオブジェクトが一つだった場合
	if (hitCount == 1)
	{
		_collisionResult = retSphereList.front();
		return true;
	}
	// 当たったオブジェクトが複数だった場合
	else if (hitCount >= 2)
	{
		_multiHit = true;
		return true;
	}

	// 当たらなかった場合
	return false;
}

bool CarryObjectBase::SphereHitJudge(const KdCollider::SphereInfo& _sphereInfo, const bool _debugFlg)
{
	//==================
	// 球判定
	//==================
	// デバッグ用
	if (_debugFlg)
	{
		if (m_pDebugWire)
		{
			m_pDebugWire->AddDebugSphere(_sphereInfo.m_sphere.Center, _sphereInfo.m_sphere.Radius);
		}
	}

	// 球に当たったオブジェクトを格納するリスト
	std::vector<std::weak_ptr<KdGameObject>> hitObjList;

	// 当たったかどうかのフラグ
	bool hitFlg = false;

	// 当たり判定！！！！！！！！！！！！！！！
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		if (obj->Intersects(_sphereInfo, nullptr))
		{
			m_wpHitObjectList.push_back(obj);
			hitFlg = true;
		}
	}

	// 当たらなかった場合
	return true;
}

bool CarryObjectBase::SphereHitGround(const KdCollider::SphereInfo& _sphereInfo, const bool _debugFlg)
{
	if (m_wpTerrainController.expired() == true) return false;
	//==================
	// 球判定
	//==================
	// デバッグ用
	if (_debugFlg)
	{
		if (m_pDebugWire)
		{
			m_pDebugWire->AddDebugSphere(_sphereInfo.m_sphere.Center, _sphereInfo.m_sphere.Radius);
		}
	}

	// 球に当たったオブジェクトを格納するリスト
	std::vector<std::weak_ptr<KdGameObject>> hitObjList;

	// 当たったかどうかのフラグ
	bool hitFlg = false;

	// 当たり判定！！！！！！！！！！！！！！！
	for (auto& obj : m_wpTerrainController.lock()->GetObjList())
	{
		if (obj.expired() == true) continue;
		if (obj.lock()->Intersects(_sphereInfo, nullptr))
		{
			m_wpHitObjectList.push_back(obj);
			hitFlg = true;
		}
	}

	// 当たらなかった場合
	return hitFlg;
}
