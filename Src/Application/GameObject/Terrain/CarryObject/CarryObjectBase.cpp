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
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}


bool CarryObjectBase::SphereHitJudge(const Math::Vector3 _centerPos, const float _radius, const KdCollider::Type _type, Math::Vector3& _hitDir, float& _maxOverLap, const bool _debugFlg)
{
	//==================
	// 球判定
	//==================
	KdCollider::SphereInfo sphereInfo;
	// 球の中心位置を設定(座標)
	sphereInfo.m_sphere.Center = _centerPos;
	// 球の半径を設定
	sphereInfo.m_sphere.Radius = _radius;
	// 当たり判定をしたいタイプを設定
	sphereInfo.m_type = _type;

	// デバッグ用
	if (_debugFlg)
	{
		if (m_pDebugWire)
		{
			m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, sphereInfo.m_sphere.Radius);
		}
	}

	// 球に当たったオブジェクト情報を格納
	std::list<KdCollider::CollisionResult> retSphereList;

	// 球に当たったオブジェクトを格納するリスト
	std::vector<std::weak_ptr<KdGameObject>> retObjList;

	// 当たり判定！！！！！！！！！！！！！！！
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		if (obj->Intersects(sphereInfo, &retSphereList))
		{
			// 当たったオブジェクトをリストで保持
			retObjList.push_back(obj);
		}
	}

	// 球に当たったリストから一番近いオブジェクトを検出
	_maxOverLap = 0;
	bool hitFlg = false;

	int cnt = 0;

	for (auto& ret : retSphereList)
	{
		if (_maxOverLap < ret.m_overlapDistance)
		{
			_maxOverLap = ret.m_overlapDistance;
			_hitDir = ret.m_hitDir;
			hitFlg = true;

			// 当たったオブジェクトを保持
			//m_wpHitObjectList.push_back(retObjList[cnt]);
		}
		cnt++;
	}

	return hitFlg;
}

bool CarryObjectBase::SphereHitJudge(const Math::Vector3 _centerPos, const float _radius, const KdCollider::Type _type, const bool _debugFlg)
{
	//==================
	// 球判定
	//==================
	KdCollider::SphereInfo sphereInfo;
	// 球の中心位置を設定(座標)
	sphereInfo.m_sphere.Center = _centerPos;
	// 球の半径を設定
	sphereInfo.m_sphere.Radius = _radius;
	// 当たり判定をしたいタイプを設定
	sphereInfo.m_type = _type;

	// デバッグ用
	if (_debugFlg)
	{
		if (m_pDebugWire)
		{
			m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, sphereInfo.m_sphere.Radius);
		}
	}

	// 球に当たったオブジェクト情報を格納
	std::list<KdCollider::CollisionResult> retSphereList;

	// 球に当たったオブジェクトのタイプを格納するリスト
	std::vector <std::weak_ptr<KdGameObject>> retObjctList;

	// 当たり判定！！！！！！！！！！！！！！！
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		if (obj->Intersects(sphereInfo, &retSphereList))
		{
			// 当たったオブジェクトをリストで保持
			retObjctList.push_back(obj);
		}
	}

	// 球に当たったリストから一番近いオブジェクトを検出
	float maxOverLap = 0;
	bool hitFlg = false;

	int cnt = 0;

	for (auto& ret : retSphereList)
	{
		if (maxOverLap < ret.m_overlapDistance)
		{
			maxOverLap = ret.m_overlapDistance;
			hitFlg = true;

			// 当たったオブジェクトを保持
			//m_wpHitObjectList.push_back(retObjctList[cnt]);
		}
		cnt++;
	}

	return hitFlg;
}

bool CarryObjectBase::SphereHitJudge(const Math::Vector3 _centerPos, const float _radius, KdCollider::Type _type, std::list<Math::Vector3>& _hitDirList, float& _maxOverLap, const bool _debugFlg)
{
	//==================
// 球判定
//==================
	KdCollider::SphereInfo sphereInfo;
	// 球の中心位置を設定(座標)
	sphereInfo.m_sphere.Center = _centerPos;
	// 球の半径を設定
	sphereInfo.m_sphere.Radius = _radius;
	// 当たり判定をしたいタイプを設定
	sphereInfo.m_type = _type;

	// デバッグ用
	if (_debugFlg)
	{
		if (m_pDebugWire)
		{
			m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, sphereInfo.m_sphere.Radius);
		}
	}

	// 球に当たったオブジェクト情報を格納
	std::list<KdCollider::CollisionResult> retSphereList;

	// 球に当たったオブジェクトを格納するリスト
	std::vector<std::weak_ptr<KdGameObject>> retObjList;

	// 当たり判定！！！！！！！！！！！！！！！
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		if (obj->Intersects(sphereInfo, &retSphereList))
		{
			// 当たったオブジェクトをリストで保持
			retObjList.push_back(obj);
		}
	}

	// 球に当たったリストから一番近いオブジェクトを検出
	_maxOverLap = 0;
	bool hitFlg = false;

	int cnt = 0;

	for (auto& ret : retSphereList)
	{
		if (_maxOverLap < ret.m_overlapDistance)
		{
			_maxOverLap = ret.m_overlapDistance;
			_hitDirList.push_back(ret.m_hitDir);
			hitFlg = true;

			// 当たったオブジェクトを保持
			m_wpHitObjectList.push_back(retObjList[cnt]);
		}
		cnt++;
	}

	return hitFlg;
}

bool CarryObjectBase::RayHitGround(const Math::Vector3 _startPos, Math::Vector3& _hitPos, const Math::Vector3 _dir, const float _range, const bool _debugFlg)
{
	// テレインコントローラーがセットされていなければ終了
	if (m_wpTerrainController.expired()) return false;

	// 地面との当たり判定
	KdCollider::RayInfo rayInfo;

	// レイの発射位置
	rayInfo.m_pos = _startPos;

	// レイの方向
	rayInfo.m_dir = Math::Vector3::Down;

	// レイの長さ
	rayInfo.m_range = _range;

	// 当たり判定のタイプ
	rayInfo.m_type = KdCollider::TypeGround;

	// レイに当たったオブジェクト情報を格納するリスト
	std::list<KdCollider::CollisionResult> retRayList;

	// レイに当たったオブジェクトのタイプを格納するリスト
	std::vector<std::weak_ptr<TerrainBase>> retObjectList;

	// 当たり判定
	for (auto& obj : m_wpTerrainController.lock()->GetObjList())
	{
		if (!obj.expired())
		{
			if (obj.lock()->Intersects(rayInfo, &retRayList))
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

			// 当たったオブジェクトを保持
			m_wpHitTerrain = retObjectList[cnt];
		}
		cnt++;
	}

	// デバッグ
	if (_debugFlg)
	{
		if (m_pDebugWire)
		{
			m_pDebugWire->AddDebugLine(_startPos, _dir, _range);
		}
	}

	return hit;
}