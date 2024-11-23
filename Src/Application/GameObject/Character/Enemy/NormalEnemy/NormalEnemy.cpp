#include "NormalEnemy.h"
#include "Application/main.h"
#include "../../../../Scene/SceneManager.h"
#include "../../../Terrain/TerrainBase.h"
#include "../../Player/Player.h"
#include "../../../Effect/Smoke/Smoke.h"

void NormalEnemy::Update()
{
	if (m_aliveFlg == false) return;

	// デバッグモード中は更新しない
	if (SceneManager::Instance().GetDebug()) return;

	// ポーズ画面中は更新しない
	if (m_pauseFlg == true) return;

	if (m_setParamFlg == false)
	{
		// 動く床に乗っていた時の処理
		if (m_moveGround.hitFlg == true)
		{
			if (m_wpHitTerrain.expired() == false)
			{
				Math::Matrix localMatFromRideObject = m_mWorld * m_moveGround.transMat.Invert();

				Math::Matrix hitTerrainTransMat = Math::Matrix::CreateTranslation(m_wpHitTerrain.lock()->GetPos());

				m_mWorld = localMatFromRideObject * hitTerrainTransMat;
				m_pos = m_mWorld.Translation();
			}
		}

		// 状態毎の更新
		if (m_nowAction)
		{
			m_nowAction->Update(*this);
		}

		// 重力
		m_gravity += m_gravityPow;
		m_pos.y -= m_gravity;
		// 重力の制限
		if (m_gravity >= m_maxGravity)
		{
			m_gravity = m_maxGravity;
		}

		// 当たり判定
		HitJudge();
	}
	else
	{
		m_pos = m_param.startPos;
		m_degAng = 0;
		m_setParamFlg = false;
	}

	// 落下死
	if (m_pos.y < -15.0f)
	{
		m_isExpired = true;
	}
}

void NormalEnemy::PostUpdate()
{
	// ポーズ画面中は更新しない
	if (m_pauseFlg == true) return;
	
	if (SceneManager::Instance().GetDebug())
	{
		// searchエリア可視化
		m_pDebugWire->AddDebugSphere(m_pos, m_param.searchArea, kRedColor);
		Math::Vector3 pos = m_param.startPos;
		pos.y = m_pos.y;
		// moveエリア可視化
		m_pDebugWire->AddDebugSphere(pos, m_param.moveArea, kWhiteColor);
	}

	// 回る床に当たっていた時の処理
	if (m_rotationGround.hitFlg)
	{
		// 当たった地面
		std::shared_ptr<TerrainBase> spHitTerrain = m_wpHitTerrain.lock();
		// 無かったら終了
		if (!spHitTerrain) return;
		Math::Vector3 terrainPos = spHitTerrain->GetPos();
		// プレイヤーから回転床までの距離
		Math::Vector3 vec = m_pos - spHitTerrain->GetPos();
		vec.z = 0;
		float length = vec.Length();
		// 移動する前の回転床から見たプレイヤーの角度
		float degAng = DirectX::XMConvertToDegrees(atan2(vec.x, vec.y));

		if (degAng <= 90 && degAng >= -90)
		{
			degAng -= 90;
			if (degAng < 0)
			{
				degAng += 360;
			}
			degAng = 360 - degAng;

			// 回転床が回転する角度
			float lotDegAng = spHitTerrain->GetParam().degAng.z;
			degAng += lotDegAng;
			m_pos.x = spHitTerrain->GetPos().x + length * cos(DirectX::XMConvertToRadians(degAng));
			m_pos.y = spHitTerrain->GetPos().y + length * sin(DirectX::XMConvertToRadians(degAng));
		}
	}

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);

	// 回転行列
	m_rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));

	// 死亡モーション
	if (m_aliveFlg == false)
	{
		DeathProcess();
	}

	/// 拡縮行列
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_scale);

	m_mWorld = scaleMat * m_rotMat * transMat;
}

void NormalEnemy::Init()
{
	DataLoad();

	srand(timeGetTime());

	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Character/Enemy/NormalEnemy/normalEnemy.gltf");

	}
	// 行列を作っておく
	m_mWorld = Math::Matrix::CreateTranslation(m_pos);

	// 当たり判定
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("NormalEnemy", m_spModel, KdCollider::TypeDamage | KdCollider::TypeDebug);

	// ゴールの座標を求める
	m_degAng = float(rand() % 360);
	m_move.goalPos.x = m_param.startPos.x + m_param.moveArea * cos(DirectX::XMConvertToRadians(m_degAng));
	m_move.goalPos.z = m_param.startPos.z + m_param.moveArea * sin(DirectX::XMConvertToRadians(m_degAng));

	// 回転させる
	m_move.rotFlg = true;

	// オブジェクトのタイプ
	m_objectType = ObjectType::NormalEnemy;

	EnemyBase::Init();

	// デバッグワイヤー
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	// 初期状態は待機状態
	m_nowAction = std::make_shared<Idle>();
}

void NormalEnemy::Reset()
{
	// ゴールの座標を求める
	m_degAng = float(rand() % 360);
	m_move.goalPos.x = m_param.startPos.x + m_param.moveArea * cos(DirectX::XMConvertToRadians(m_degAng));
	m_move.goalPos.z = m_param.startPos.z + m_param.moveArea * sin(DirectX::XMConvertToRadians(m_degAng));

	m_pos = m_param.startPos;

	// 回転させる
	m_move.rotFlg = true;
}

Math::Vector3 NormalEnemy::GetPos() const
{
	// これ以上上に当たったら倒せるという座標を返す
	Math::Vector3 pos;
	if (m_spModel)
	{
		pos = (m_spModel->FindNode("DeathPoint")->m_worldTransform * m_mWorld).Translation();
	}
	return pos;
}

void NormalEnemy::HitJudge()
{
	HitGround();

	HitEnemy();
}

void NormalEnemy::HitGround()
{
	// 動く床関連をリセット
	// 動く床
	m_moveGround.transMat = Math::Matrix::Identity;
	m_moveGround.hitFlg = false;
	// 回る床
	m_rotationGround.transMat = Math::Matrix::Identity;
	m_rotationGround.hitFlg = false;

	// 当たった地面をリセット
	m_wpHitTerrain.reset();

	// 地面とのレイ当たり判定
	{
		// 当たった座標
		Math::Vector3 hitPos = Math::Vector3::Zero;
		// ノードの座標
		Math::Vector3 nodePos = Math::Vector3::Zero;
		// 当たったかどうか
		bool hitFlg = false;

		// レイの情報
		KdCollider::RayInfo rayInfo;
		// レイの長さ
		rayInfo.m_range = m_gravity + 1.0f + m_enableStepHeight;
		// レイの座標(キャラの中心)
		rayInfo.m_pos.y = m_enableStepHeight + 1.0f;
		rayInfo.m_pos += m_pos;
		// レイの方向
		rayInfo.m_dir = Math::Vector3::Down;
		// レイのタイプ
		rayInfo.m_type = KdCollider::TypeGround;

		// レイ判定
		hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitTerrain);

		// 当たっていた時の処理
		if (hitFlg)
		{
			// 当たったオブジェクト
			std::shared_ptr<TerrainBase> spHitObject = m_wpHitTerrain.lock();

			if (spHitObject)
			{
				spHitObject->OnHit();

				switch (spHitObject->GetObjectType())
				{
					// バウンドする床に乗った場合
				case ObjectType::BoundGround:
					// 座標
					m_pos.y = hitPos.y;
					// 重力
					m_gravity = -0.25f;

					// 地面に当たっていない
					m_isGround = false;
					break;

					// 動く床に乗った場合
				case ObjectType::MoveGround:
				case ObjectType::DropGround:
				case ObjectType::Switch:
					// 座標
					m_pos.y = hitPos.y;
					// 重力
					m_gravity = 0;
					// 動く床の動く前の行列
					m_moveGround.transMat = Math::Matrix::CreateTranslation(spHitObject->GetPos());
					// 動く床に当たったかどうかのフラグ
					m_moveGround.hitFlg = true;
					// 地面に当たっている
					m_isGround = true;
					break;

					// 回る床に乗った場合
				case ObjectType::RotationGround:
				case ObjectType::Propeller:
					// 座標
					m_pos.y = hitPos.y;
					// 重力
					m_gravity = 0;
					// 当たったかどうか
					m_rotationGround.hitFlg = true;
					// 地面に当たっている
					m_isGround = true;
					break;

				default:
					// 座標
					m_pos.y = hitPos.y;
					// 重力
					m_gravity = 0;

					// 地面に当たっている
					m_isGround = true;
					break;
				}
			}
		}
		else
		{
			// 地面に当たっていない
			m_isGround = false;
		}
	}

	// 地面(壁)とのスフィア判定
	{
		// スフィアの情報
		KdCollider::SphereInfo sphereInfo;
		// スフィアの中心座標
		sphereInfo.m_sphere.Center = m_pos;
		// スフィアの半径
		sphereInfo.m_sphere.Radius = 0.4f;
		sphereInfo.m_sphere.Center.y += sphereInfo.m_sphere.Radius + 0.1f;
		// スフィアのタイプ
		sphereInfo.m_type = KdCollider::TypeGround;

		// 当たったかどうかのフラグ
		bool hitFlg = false;
		// 当たった結果
		std::vector<KdCollider::CollisionResult> collisionResultList;
		// 複数に当たったかどうかのフラグ
		bool multiHitFlg = false;

		hitFlg = SphereHitJudge(sphereInfo, collisionResultList, multiHitFlg);

		// 複数のオブジェクトに当たっていた場合
		if (multiHitFlg == true)
		{
			std::vector<Math::Vector3> normalList1 = collisionResultList[0].m_hitNormalList;
			std::vector<Math::Vector3> normalList2 = collisionResultList[1].m_hitNormalList;

			bool flg = false;

			for (auto& normal : normalList1)
			{
				normal.Normalize();
				for (auto& normal2 : normalList2)
				{
					normal2.Normalize();
					float dot = normal.Dot(normal2);
					dot = std::clamp(dot, -1.0f, 1.0f);
					float deg = DirectX::XMConvertToDegrees(acos(dot));

					if (deg <= m_doubleObjectHitMaxDegAng || deg >= m_doubleObjectHitMaxDegAng)
					{
						flg = true;
					}
				}
			}
			if (flg == false)
			{
				// Y座標以外、更新前の座標に戻す
				m_pos.x = m_oldPos.x;
				m_pos.z = m_oldPos.z;
			}
			else
			{
				if (collisionResultList[0].m_overlapDistance > collisionResultList[1].m_overlapDistance)
				{
					KdCollider::CollisionResult result = collisionResultList[0];
					// Y軸の補正はなし
					result.m_hitDir.Normalize();
					Math::Vector3 pos = result.m_hitDir * result.m_overlapDistance;
					// 当たった時のスフィアの座標
					Math::Vector3 hitSpherePos = sphereInfo.m_sphere.Center;
					m_pos.x = hitSpherePos.x + pos.x;
					m_pos.z = hitSpherePos.z + pos.z;
				}
				else
				{
					KdCollider::CollisionResult result = collisionResultList[1];
					// Y軸の補正はなし
					result.m_hitDir.Normalize();
					Math::Vector3 pos = result.m_hitDir * result.m_overlapDistance;
					// 当たった時のスフィアの座標
					Math::Vector3 hitSpherePos = sphereInfo.m_sphere.Center;
					m_pos.x = hitSpherePos.x + pos.x;
					m_pos.z = hitSpherePos.z + pos.z;
				}
			}
		}
		// 一つのオブジェクトに当たった場合
		else if (hitFlg)
		{
			KdCollider::CollisionResult result = collisionResultList.front();
			// Y軸の補正はなし
			result.m_hitDir.Normalize();
			Math::Vector3 pos = result.m_hitDir * result.m_overlapDistance;
			pos.y = 0;
			m_pos += pos;
		}
	}
}

void NormalEnemy::HitEnemy()
{
	// スフィアの情報
	KdCollider::SphereInfo sphereInfo;
	// スフィアの中心座標
	sphereInfo.m_sphere.Center = m_pos;
	// スフィアの半径
	sphereInfo.m_sphere.Radius = 0.25f;
	sphereInfo.m_sphere.Center.y += sphereInfo.m_sphere.Radius + 0.2f;
	// スフィアのタイプ
	sphereInfo.m_type = KdCollider::TypeDamage;

	// 当たったかどうかのフラグ
	bool hitFlg = false;

	// 当たった結果
	std::vector<KdCollider::CollisionResult> collisionResultList;

	bool flg = false;
	hitFlg = SphereHitJudge(sphereInfo, collisionResultList, flg);

	// 当たっていた場合
	if (hitFlg == true)
	{
		// 一番近いオブジェクトを見つける
		float maxOverLap = 0;
		Math::Vector3 hitDir;

		for (auto& result : collisionResultList)
		{
			if (result.m_overlapDistance > maxOverLap)
			{
				maxOverLap = result.m_overlapDistance;
				hitDir = result.m_hitDir;
			}
		}
		// 補正
		hitDir.Normalize();
		Math::Vector3 pos = hitDir * maxOverLap;
		// Y軸は補正無し
		pos.y = 0;
		m_pos += pos;
	}
}

void NormalEnemy::DeathProcess()
{
	m_scale = m_deathScale;
	m_deathCount++;

	if (m_deathCount >= m_deathTime)
	{
		m_isExpired = true;

		// 煙を生み出す
		std::shared_ptr<Smoke> smoke = std::make_shared<Smoke>();
		smoke->Init();
		smoke->SetPos(m_pos);
		smoke->SetSmokeType(Smoke::SmokeType::DeathSmoke);
		SceneManager::Instance().AddObject(smoke);
	}
}

void NormalEnemy::SetParam(Param _param)
{
	m_param = _param;
	m_setParamFlg = true;
	m_pos = m_param.startPos;
	// ゴールの座標を求める
	m_move.goalPos.x = m_param.startPos.x + m_param.moveArea * cos(DirectX::XMConvertToRadians(m_degAng));
	m_move.goalPos.z = m_param.startPos.z + m_param.moveArea * sin(DirectX::XMConvertToRadians(m_degAng));
}

void NormalEnemy::OnHit()
{
	m_aliveFlg = false;
}

bool NormalEnemy::TargetViewingAngleCheck()
{
	// ターゲット
	std::shared_ptr<Player> spTarget = m_wpTarget.lock();
	if (spTarget)
	{
		Math::Vector3 vec = spTarget->GetPos() - m_pos;
		Math::Vector3 forwardVec = m_mWorld.Backward();
		forwardVec.Normalize();

		// プレイヤーが索敵範囲内にいるなら、視野角判定をする
		if (vec.Length() < m_param.searchArea)
		{
			vec.Normalize();
			float dot = forwardVec.Dot(vec);
			dot = std::clamp(dot, -1.0f, 1.0f);
			float deg = DirectX::XMConvertToDegrees(acos(dot));
			if (deg < m_homing.viewingAngle)
			{
				return true;
			}
		}
	}
	return false;
}

void NormalEnemy::DataLoad()
{
	// JSONファイルを読み込む
	std::ifstream file(m_path.data());
	if (!file.is_open()) return;

	nlohmann::json data;
	file >> data;

	// JSONデータを格納していく
	m_moveSpeed		= data["NormalEnemy"]["m_moveSpeed"];	// 移動速度
	m_jumpPow		= data["NormalEnemy"]["m_jumpPow"];		// ジャンプ力
	m_findJumpPow	= data["NormalEnemy"]["m_findJumpPow"];	// 見つけた時のジャンプ力
}




void NormalEnemy::ChangeActionState(std::shared_ptr<StateBase> _nextState)
{
	if (m_nowAction)m_nowAction->Exit(*this);
	m_nowAction = _nextState;
	m_nowAction->Enter(*this);
}

//==============================================
// 回転中など地面にいる時の処理
//==============================================
void NormalEnemy::Idle::Enter(NormalEnemy& _owner)
{
	// 待機時間をリセット
	_owner.m_move.stayCount = 0;
}

void NormalEnemy::Idle::Update(NormalEnemy& _owner)
{
	// もし回転しなければいけない状態なら回転
	if (_owner.m_move.rotFlg == true)
	{
		// 向きたい方向 Y座標は無視
		Math::Vector3 vec = _owner.m_move.goalPos - _owner.m_pos;
		vec.y = 0;
		vec.Normalize();
		// 回転処理
		if (_owner.RotationCharacter(_owner.m_degAng, vec, _owner.m_move.minRotAng) == false)
		{
			// 回転し終わったら回転状態を終了
			_owner.m_move.rotFlg = false;
			// ジャンプ移動に移行
			_owner.ChangeActionState(std::make_shared<JumpMove>());
		}
	}
	else
	{
		// 回転処理中でなければ待機カウントを進める
		_owner.m_move.stayCount++;
	}

	// 視野角内なら見つけたモーションへ移行
	if (_owner.TargetViewingAngleCheck() == true)
	{
		_owner.ChangeActionState(std::make_shared<Find>());
	}

	// 待機終了し、ジャンプ移動に移行
	if (_owner.m_move.stayCount > _owner.m_move.stayTime)
	{
		_owner.ChangeActionState(std::make_shared<JumpMove>());
	}
}

void NormalEnemy::Idle::Exit(NormalEnemy& _owner)
{
	// 回転状態かどうかをリセットする
	_owner.m_move.rotFlg = false;
}

//==============================================
// ジャンプ移動
//==============================================
void NormalEnemy::JumpMove::Enter(NormalEnemy& _owner)
{
	// ジャンプさせる
	_owner.m_gravity = -_owner.m_jumpPow;
}

void NormalEnemy::JumpMove::Update(NormalEnemy& _owner)
{
	// 回転する状態なら、移動する必要がないので、移動しない
	if (_owner.m_move.rotFlg == false)
	{
		// 移動ベクトルを求める
		Math::Vector3 moveVec = _owner.m_move.goalPos - _owner.m_pos;
		// Y除外
		moveVec.y = 0;

		// もしゴールまでの距離がスピードより短くなったら、座標をゴール座標にする
		if (moveVec.Length() < _owner.m_moveSpeed)
		{
			_owner.m_pos.x = _owner.m_move.goalPos.x;
			_owner.m_pos.z = _owner.m_move.goalPos.z;
			// 回転する状態にする
			_owner.m_move.rotFlg = true;

			// 新たなゴール座標を決める
			// ゴール座標を求めるための角度を求める(否も角度の+-90度は除外)
			// 最小の角度
			float minAng = _owner.m_move.degAng + 90;

			float degAng = rand() % 180 + minAng;
			if (degAng >= 360)
			{
				degAng -= 360;
			}
			if (degAng < 0)
			{
				degAng += 360;
			}
			_owner.m_move.degAng = degAng;
			// ゴールの座標を求める
			_owner.m_move.goalPos.x = _owner.m_param.startPos.x + _owner.m_param.moveArea * cos(DirectX::XMConvertToRadians(_owner.m_move.degAng));
			_owner.m_move.goalPos.z = _owner.m_param.startPos.z + _owner.m_param.moveArea * sin(DirectX::XMConvertToRadians(_owner.m_move.degAng));
		}
		// 違ったら移動
		else
		{
			moveVec.Normalize();
			// 移動
			_owner.m_pos += moveVec * _owner.m_moveSpeed;
		}
	}

	// 接地したら、待機状態に移行
	if (_owner.m_isGround)
	{
		_owner.ChangeActionState(std::make_shared<Idle>());
	}
}

//==============================================
// 見つけた時の処理
//==============================================
void NormalEnemy::Find::Enter(NormalEnemy& _owner)
{
	// ジャンプさせる
	_owner.m_gravity = -_owner.m_findJumpPow;
}

void NormalEnemy::Find::Update(NormalEnemy& _owner)
{
	// ターゲットまでのベクトル
	Math::Vector3 vec;
	if (_owner.m_wpTarget.expired() == false)
	{
		vec = _owner.m_wpTarget.lock()->GetPos() - _owner.m_pos;
	}
	// Yは除外
	vec.y = 0;
	vec.Normalize();
	// 回転処理
	_owner.RotationCharacter(_owner.m_degAng, vec, _owner.m_homing.minRotDegAng);

	// 接地した
	if (_owner.m_isGround == true)
	{
		// 接地したタイミングで、視野角内にターゲットがいれば、追尾
		if (_owner.TargetViewingAngleCheck() == true)
		{
			_owner.ChangeActionState(std::make_shared<Homing>());
		}
		// 視野角内にターゲットがいなければ、見失ったモーション
		else
		{
			_owner.ChangeActionState(std::make_shared<LostTarget>());
		}
	}
}

//==============================================
// 追尾処理
//==============================================
void NormalEnemy::Homing::Update(NormalEnemy& _owner)
{
	// ターゲット
	std::shared_ptr<Player> player = _owner.m_wpTarget.lock();
	// ターゲットがいなければ終了
	if (!player) return;

	// ターゲットまでのベクトル
	Math::Vector3 vec;
	if (_owner.m_wpTarget.expired() == false)
	{
		vec = _owner.m_wpTarget.lock()->GetPos() - _owner.m_pos;
	}
	// Yは除外
	vec.y = 0;
	vec.Normalize();
	// 回転処理
	_owner.RotationCharacter(_owner.m_degAng, vec, _owner.m_homing.minRotDegAng);

	// 移動
	_owner.m_pos += vec * _owner.m_moveSpeed;

	// 接地したときの処理
	if (_owner.m_isGround == true)
	{
		// 接地した際に視野角内にいなかったら、見失ったモーションへ移行
		if (_owner.TargetViewingAngleCheck() == false)
		{
			_owner.ChangeActionState(std::make_shared<LostTarget>());
			return;
		}
		// ジャンプ
		_owner.m_gravity = -_owner.m_jumpPow;
	}
}

//==============================================
// 見失った時の処理
//==============================================
void NormalEnemy::LostTarget::Enter(NormalEnemy& _owner)
{
	// 元の角度を保持
	_owner.m_lostTarget.oldDegAng = _owner.m_degAng;
}

void NormalEnemy::LostTarget::Update(NormalEnemy& _owner)
{
	// 視野角内にターゲットがいれば、見つけたモーションへ移行
	if (_owner.TargetViewingAngleCheck() == true)
	{
		_owner.ChangeActionState(std::make_shared<Find>());
	}

	// 左を見る
	if (_owner.m_lostTarget.dir == _owner.m_lostTarget.Left && _owner.m_lostTarget.stayFlg == false)
	{
		// 何度回転したか保存しておく
		_owner.m_lostTarget.sumLotAng += _owner.m_lostTarget.lotAng;
		// 回転角度が最大値より大きくなったら、キャラの角度を元の角度-最大値にする
		if (_owner.m_lostTarget.sumLotAng > _owner.m_lostTarget.maxLotAng)
		{
			_owner.m_degAng = _owner.m_lostTarget.oldDegAng - _owner.m_lostTarget.maxLotAng;
			// 何度回転したかを最大値にする
			_owner.m_lostTarget.sumLotAng = _owner.m_lostTarget.maxLotAng;
			// 待機させる
			_owner.m_lostTarget.stayFlg = true;
			// 方向フラグを変える
			_owner.m_lostTarget.dir = _owner.m_lostTarget.Right;
		}
		else
		{
			// 回転させる
			_owner.m_degAng -= _owner.m_lostTarget.lotAng;
		}
	}

	// 左→右を見る
	if (_owner.m_lostTarget.dir == _owner.m_lostTarget.Right && _owner.m_lostTarget.stayFlg == false)
	{
		// 何度回転したか保存しておく(すでに最大値になっているはずなので-最大値になるまでする)
		_owner.m_lostTarget.sumLotAng -= _owner.m_lostTarget.lotAng;
		// 回転角度が最大値のマイナスより小さくなったら、キャラの角度を元の角度+最大値にする
		if (_owner.m_lostTarget.sumLotAng < -_owner.m_lostTarget.maxLotAng)
		{
			_owner.m_degAng = _owner.m_lostTarget.oldDegAng + _owner.m_lostTarget.maxLotAng;
			// 待機させる
			_owner.m_lostTarget.stayFlg = true;
			// 方向を変える
			_owner.m_lostTarget.dir = _owner.m_lostTarget.Base;
		}
		else
		{
			// 回転させる
			_owner.m_degAng += _owner.m_lostTarget.lotAng;
		}
	}

	// 右→元の角度に戻る
	if (_owner.m_lostTarget.dir == _owner.m_lostTarget.Base && _owner.m_lostTarget.stayFlg == false)
	{
		float dif = _owner.m_degAng - _owner.m_lostTarget.oldDegAng;

		if (dif > 360)
		{
			dif -= 360;
		}

		// 角度の差が回転角度より少なかったら角度を元の角度にする
		if (dif < _owner.m_lostTarget.lotAng)
		{
			_owner.m_degAng = _owner.m_lostTarget.oldDegAng;
			// 待機させる
			_owner.m_lostTarget.stayFlg = true;
			// 終了
			_owner.m_lostTarget.dir = _owner.m_lostTarget.End;
		}
		else
		{
			_owner.m_degAng -= _owner.m_lostTarget.lotAng;
		}
	}

	// 角度の調整
	if (_owner.m_degAng < 0)
	{
		_owner.m_degAng += 360;
	}
	if (_owner.m_degAng > 360)
	{
		_owner.m_degAng -= 360;
	}

	// 待機
	if (_owner.m_lostTarget.stayFlg)
	{
		_owner.m_lostTarget.stayCount++;

		// 待機終了
		if (_owner.m_lostTarget.stayCount > _owner.m_lostTarget.stayTime)
		{
			_owner.m_lostTarget.stayFlg = false;
			_owner.m_lostTarget.stayCount = 0;

			// もし元に戻ったら待機状態に戻る
			if (_owner.m_lostTarget.dir == _owner.m_lostTarget.End)
			{
				// 回転状態にする
				_owner.m_move.rotFlg = true;
				_owner.ChangeActionState(std::make_shared<Idle>());
			}
		}
	}
}

void NormalEnemy::LostTarget::Exit(NormalEnemy& _owner)
{
	// 数値をリセット
	_owner.m_lostTarget.sumLotAng	= 0;							// 動いた角度の合計
	_owner.m_lostTarget.stayCount	= 0;							// 待機時間のカウント
	_owner.m_lostTarget.stayFlg		= false;						// 待機フラグ
	_owner.m_lostTarget.oldDegAng	= 0;							// 元の角度を保持するため
	_owner.m_lostTarget.dir			= LostTargetAnimation::Left;	// 方向 
}