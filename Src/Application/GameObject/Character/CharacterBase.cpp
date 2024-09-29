#include "CharacterBase.h"
#include "../../Scene/SceneManager.h"
#include "../../Tool/ObjectController/TerrainController/TerrainController.h"
#include "../../Tool/ObjectController/CarryObjectController/CarryObjectController.h"
#include "../Terrain/TerrainBase.h"
#include "../Terrain/CarryObject/CarryObjectBase.h"

void CharacterBase::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void CharacterBase::DrawLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void CharacterBase::Reset()
{
	// 座標
	m_pos = Math::Vector3::Zero;

	// 移動ベクトル
	m_moveVec = Math::Vector3::Zero;

	// 重力
	m_gravity = 0;

	// 当たったオブジェクトリストをリセット
	m_wpHitObjectList.clear();

	// 当たった地形をリセット
	m_wpHitTerrain.reset();

	m_pauseFlg = false;
}

// レイ判定　当たったオブジェクトをリストに追加
bool CharacterBase::RayHitJudge(const KdCollider::RayInfo& _rayInfo, Math::Vector3& _hitPos, const bool _debugFlg)
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
bool CharacterBase::RayHitJudge(const KdCollider::RayInfo& _rayInfo, Math::Vector3& _hitPos, std::weak_ptr<TerrainBase>& _hitObject, const bool _debugFlg)
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

// レイ判定　運べるオブジェクトのみ　当たったオブジェクトを引数に指定した変数に保持
bool CharacterBase::RayHitJudge(const KdCollider::RayInfo& _rayInfo, Math::Vector3& _hitPos, std::weak_ptr<CarryObjectBase>& _hitObject, const bool _debugFlg)
{
	// CarryObjectControllerがセットされていなければ終了
	if (m_wpCarryObjectController.expired()) return false;

	// レイに当たったオブジェクト情報を格納するリスト
	std::list<KdCollider::CollisionResult> retRayList;

	// レイに当たったオブジェクトのタイプを格納するリスト
	std::vector<std::weak_ptr<CarryObjectBase>> retObjectList;

	// 当たり判定
	for (auto& obj : m_wpCarryObjectController.lock()->GetObjList())
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

			// 当たったオブジェクトを保持
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

bool CharacterBase::SphereHitJudge(const KdCollider::SphereInfo& _sphereInfo, KdCollider::CollisionResult& _collisionResult, bool& _multiHit, const bool _debugFlg)
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

	_multiHit = false;

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

bool CharacterBase::SphereHitJudge(const KdCollider::SphereInfo& _sphereInfo, const bool _debugFlg)
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

bool CharacterBase::RotationCharacter(float& _degAng, Math::Vector3 _toVec, const float _minDegAng)
{
	// 回転したかどうかのフラグ
	bool rotFlg = false;

	// キャラクターの正面方向
	Math::Vector3 nowVec = m_mWorld.Backward();
	nowVec.Normalize();

	// 向きたい方向
	_toVec.Normalize();

	// 内積
	float d = nowVec.Dot(_toVec);

	// 丸め誤差
	d = std::clamp(d, -1.0f, 1.0f);

	// アークコサインで角度に変換
	float ang = DirectX::XMConvertToDegrees(acos(d));

	// ゆっくり回転するように処理
	if (ang >= 0.1f)
	{
		rotFlg = true;

		// 回転制御
		if (ang > _minDegAng)
		{
			ang = _minDegAng;
		}

		// 外積
		Math::Vector3 c = _toVec.Cross(nowVec);

		if (c.y >= 0)	// 上
		{
			_degAng -= ang;
			if (_degAng < 0)
			{
				_degAng += 360;
			}
		}
		else			//下
		{
			_degAng += ang;
			if (_degAng >= 360)
			{
				_degAng -= 360;
			}
		}
	}
	else
	{
		rotFlg = false;
	}

	return rotFlg;
}