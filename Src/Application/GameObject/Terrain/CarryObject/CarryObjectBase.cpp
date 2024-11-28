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

void CarryObjectBase::Init()
{
	BaseDataLoad();
}

void CarryObjectBase::Reset()
{
	// 動く床
	m_moveGround.hitFlg = false;
	m_moveGround.transMat = Math::Matrix::Identity;
	// 回る床
	m_rotationGround.hitFlg = false;
	m_rotationGround.transMat = Math::Matrix::Identity;

	// 座標
	m_pos = m_param.startPos;

	// 重力
	m_gravity = 0;

	// 持たれているかどうかのフラグ
	m_carryFlg = false;

	// 動きを完全に停止させるフラグ
	m_pauseFlg = false;

	// 当たった地形
	m_wpHitTerrain.reset();

	// 当たったオブジェクトのリスト
	m_wpHitObjectList.clear();

	// 少し白くするフラグ
	m_whiteFlg = false;
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
			m_pDebugWire->AddDebugLine(_rayInfo.m_pos, _rayInfo.m_dir, _rayInfo.m_range, {1, 0, 0, 1});
		}
	}

	return hit;
}

bool CarryObjectBase::SphereHitJudge(const KdCollider::SphereInfo& _sphereInfo, std::list<KdCollider::CollisionResult>& _collisionResult, const bool _debugFlg)
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
	std::vector<std::weak_ptr<KdGameObject>> retObjList;

	bool hitFlg = false;

	// 当たり判定！！！！！！！！！！！！！！！
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		if (obj->Intersects(_sphereInfo, &_collisionResult))
		{
			// 当たったオブジェクトをリストで保持
			retObjList.push_back(obj);
			hitFlg = true;
		}
	}

	return hitFlg;
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

void CarryObjectBase::BaseDataLoad()
{
	// JSONファイルを読み込む
	std::ifstream file(m_basePath.data());
	if (!file.is_open()) return;

	nlohmann::json data;
	file >> data;

	// JSONデータを格納していく
	m_gravityPow = data["Gravity"]["m_gravityPow"];	// 重力
	m_maxGravity = data["Gravity"]["m_maxGravity"];	// 重力の上限
}
