#include "CharacterBase.h"
#include "../../Scene/SceneManager.h"
#include "CharacterBase.h"

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
}

bool CharacterBase::RayHitJudge(const Math::Vector3 _startPos, Math::Vector3& _hitPos, const Math::Vector3 _dir, const float _range, const KdCollider::Type _type, const bool _debugFlg)
{
	// 地面との当たり判定
	KdCollider::RayInfo rayInfo;

	// レイの発射位置
	rayInfo.m_pos = _startPos;

	// レイの方向
	rayInfo.m_dir = Math::Vector3::Down;

	// レイの長さ
	rayInfo.m_range = _range;

	// 当たり判定のタイプ
	rayInfo.m_type = _type;

	// レイに当たったオブジェクト情報を格納するリスト
	std::list<KdCollider::CollisionResult> retRayList;

	// レイに当たったオブジェクトのタイプを格納するリスト
	std::vector<std::weak_ptr<KdGameObject>> retObjectList;

	// 当たり判定
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		if (obj->Intersects(rayInfo, &retRayList))
		{
			// 当たったオブジェクトをリストで保持
			retObjectList.push_back(obj);
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
			m_wpHitObjectList.push_back(retObjectList[cnt]);
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

bool CharacterBase::SphereHitJudge(const Math::Vector3 _centerPos, const float _radius, const KdCollider::Type _type, Math::Vector3& _hitDir, float& _maxOverLap, const bool _debugFlg)
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
	m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, sphereInfo.m_sphere.Radius);

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
			m_wpHitObjectList.push_back(retObjctList[cnt]);
		}
		cnt++;
	}

	return hitFlg;
}

bool CharacterBase::SphereHitJudge(const Math::Vector3 _centerPos, const float _radius, const KdCollider::Type _type, const bool _debugFlg)
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
	m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, sphereInfo.m_sphere.Radius);

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
			m_wpHitObjectList.push_back(retObjctList[cnt]);
		}
		cnt++;
	}

	return hitFlg;
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