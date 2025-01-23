#include "BoxEnemy.h"
#include "../../../Character/Player/Player.h"
#include "../../TerrainBase.h"
#include "Application/main.h"
#include "../../../../Scene/SceneManager.h"

#include "../../MoveObjectRideProcess/MoveObjectRideProcess.h"

void BoxEnemy::Update()
{
	m_oldPos = m_pos;

	if (m_pauseFlg) return;

	// 動く床に乗っていた時の処理
	if (m_moveGround.hitFlg == true)
	{
		if (m_wpHitTerrain.expired() == false)
		{
			m_pos = MoveObjectRideProcess::Instance().MoveGroundRide(m_mWorld, m_moveGround.transMat, m_wpHitTerrain.lock()->GetPos());
		}
	}

	if (m_lerpFlg == true)
	{
		HoldLerp();
	}

	// 状態毎の更新
	if (m_nowAction)
	{
		m_nowAction->Update(*this);
	}

	// ヒットフラグをリセット
	m_onHitFlg = false;

	// 重力の処理
	if (m_isCarry == false)
	{
		// 重力
		m_gravity += m_gravityPow;
		m_pos.y -= m_gravity;
		// 重力の制限
		if (m_gravity >= m_maxGravity)
		{
			m_gravity = m_maxGravity;
		}
	}

	if (SceneManager::Instance().GetDebug())
	{
		//m_pDebugWire->AddDebugSphere(m_pos, m_param.area, kGreenColor);
	}

	// Y座標が一定以下になると死亡
	if (m_pos.y < m_underLine)
	{
		// ステートを待機状態へ変更
		ChangeActionState(std::make_shared<Idle>());
		// 復活音
		if (m_wpRespawnSound.expired() == false)
		{
			m_wpRespawnSound.lock()->Play();
		}
	}

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);

	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));

	m_mWorld = rotMat * transMat;
}

void BoxEnemy::PostUpdate()
{
	if (m_pauseFlg) return;

	bool isGround = m_isGround;

	HitJudge();

	// 着地した瞬間
	if (isGround == false && m_isGround == true)
	{
		// 何の地面に乗っているかによって、音を変える
		if (!m_wpHitTerrain.expired())
		{
			ObjectType type = m_wpHitTerrain.lock()->GetObjectType();
			switch (type)
			{
				// 草の音
			case ObjectType::NormalGround:
			case ObjectType::SlopeGround:
			case ObjectType::RotationGround:
				m_wpLandSound[LandSoundType::Grass].lock()->Play();
				break;

			case ObjectType::BoundGround:
				m_wpLandSound[LandSoundType::Bound].lock()->Play();
				break;

				// コツコツ見たいな音
			default:
				m_wpLandSound[LandSoundType::Tile].lock()->Play();
				break;
			}
		}
	}
	// 音座標更新
	// 着地音
	for (int i = 0; i < LandSoundType::MaxNum; i++)
	{
		if (m_wpLandSound[i].expired() == true) continue;
		if (m_wpLandSound[i].lock()->IsPlaying() == true)
		{
			m_wpLandSound[i].lock()->SetPos(m_pos);
		}
	}
	// ジャンプ音
	if (m_wpJumpSound.expired() == false)
	{
		if (m_wpJumpSound.lock()->IsPlaying() == true)
		{
			m_wpJumpSound.lock()->SetPos(m_pos);
		}
	}
	// 復活音
	if (m_wpRespawnSound.expired() == false)
	{
		if (m_wpRespawnSound.lock()->IsPlaying() == true)
		{
			m_wpRespawnSound.lock()->SetPos(m_pos);
		}
	}

	// 運ばれていない時の処理
	if (m_isCarry == false)
	{
		Math::Matrix transMat = Math::Matrix::Identity;
		// 回る床に当たっていた時の処理
		if (m_rotationGround.hitFlg)
		{
			// 当たった地面
			std::shared_ptr<TerrainBase> spHitTerrain = m_wpHitTerrain.lock();
			// 無かったら終了
			if (!spHitTerrain) return;
			Math::Vector3 terrainPos = spHitTerrain->GetPos();
			// 回転床が回転する角度
			Math::Vector3 rotDegAng = spHitTerrain->GetParam().degAng;

			// Z軸回転の場合
			if (rotDegAng.z != 0)
			{
				m_rayDownFlg = MoveObjectRideProcess::Instance().RotationGroundRide(m_pos, terrainPos, rotDegAng);
			}
			// Y軸回転の場合
			else if (rotDegAng.y != 0)
			{
				MoveObjectRideProcess::Instance().PropellerRide(m_pos, terrainPos, rotDegAng, m_degAng);
			}
		}
	}
	// 運ばれている時の処理(もたれた瞬間は吸い寄せられる様に動かすため処理しない)
	else if (m_lerpFlg == false)
	{
		// プレイヤー
		std::shared_ptr<Player> spPlayer = m_wpPlayer.lock();

		if (!spPlayer) return;

		// プレイヤーのモデル
		const std::shared_ptr<KdModelWork> spPlayerModel = spPlayer->GetModel();
		// プレイヤーの角度
		m_degAng = spPlayer->GetAngle();

		// 運ぶオブジェクトを置く座標の計算
		Math::Vector3 playerCarryPos = spPlayerModel->FindNode("CarryPoint")->m_worldTransform.Translation();
		// 回転処理
		playerCarryPos.x = playerCarryPos.z * sin(DirectX::XMConvertToRadians(m_degAng));
		playerCarryPos.z = playerCarryPos.z * cos(DirectX::XMConvertToRadians(m_degAng));
		// 座標足しこみ
		playerCarryPos += spPlayer->GetPos();

		// 回転込みの行列を作成
		Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));
		Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);
		Math::Matrix mat = rotMat * transMat;

		// 頂点の座標を回転
		for (int i = 0; i < Max; i++)
		{
			m_edgePos[i] = (Math::Matrix::CreateTranslation(m_edgeBasePos[i]) * mat).Translation();
		}

		// 座標調整
		Math::Vector3 carryPos = (m_spModel->FindNode("Carry")->m_worldTransform * mat).Translation() - mat.Translation();

		m_pos = playerCarryPos - carryPos;
	}

	// 行列確定
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);

	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));

	m_mWorld = rotMat * m_shakeMat * transMat;

	// アニメーションの更新
	m_spAnimator->AdvanceTime(m_spEnemyModel->WorkNodes());
	m_spEnemyModel->CalcNodeMatrices();
}

void BoxEnemy::GenerateDepthMapFromLight()
{
	// 箱状態
	if (m_enemyFlg == false)
	{
		if (m_spModel)
		{
			KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
		}
	}
	// 敵状態
	else
	{
		if (m_spEnemyModel)
		{
			KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spEnemyModel, m_mWorld);
		}
	}
}

void BoxEnemy::DrawLit()
{
	// 箱状態
	if (m_enemyFlg == false)
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
	// 敵状態
	else
	{
		if (m_spEnemyModel)
		{
			KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spEnemyModel, m_mWorld);
		}
	}
}

void BoxEnemy::Init()
{
	CarryObjectBase::Init();

	DataLoad();

	srand(timeGetTime());

	m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Terrain/CarryObject/BoxEnemyBox/BoxEnemyBox.gltf");

	// 角っこの座標
	Math::Vector3 rightBackUp = m_spModel->FindNode("RightBackUp")->m_worldTransform.Translation();
	Math::Vector3 leftFrontDown = m_spModel->FindNode("LeftFrontDown")->m_worldTransform.Translation();
	m_edgeBasePos[RightBack] = { rightBackUp.x, 0, rightBackUp.z };		// 右後ろ
	m_edgeBasePos[RightFront] = { rightBackUp.x, 0, leftFrontDown.z };	// 右前
	m_edgeBasePos[LeftBack] = { leftFrontDown.x, 0, rightBackUp.z };	// 左後ろ
	m_edgeBasePos[LeftFront] = { leftFrontDown.x, 0, leftFrontDown.z };	// 左前
	m_edgeBasePos[Up] = { 0, rightBackUp.y, 0 };						// 上
	m_edgeBasePos[Down] = { 0, leftFrontDown.y, 0 };					// 下

	for (int i = 0; i < Max; i++)
	{
		m_edgePos[i] = m_edgeBasePos[i] + m_pos;
	}

	// 最初の角度
	m_degAng = m_baseDegAng;

	// 中心座標
	m_centerPos = m_spModel->FindNode("Center")->m_worldTransform.Translation();

	// コライダー
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("BoxEnemyBox", m_spModel, KdCollider::TypeGround | KdCollider::TypeDebug);

	// オブジェクトのタイプ
	m_objectType = ObjectType::BoxEnemy;

	// ベースのオブジェクトのタイプ
	m_baseObjectType = BaseObjectType::CarryObject;

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	// 敵状態のモデル
	if (!m_spEnemyModel)
	{
		m_spEnemyModel = std::make_shared<KdModelWork>();
		m_spEnemyModel->SetModelData("Asset/Models/Terrain/CarryObject/BoxEnemy/BoxEnemy.gltf");
	}
	m_spAnimator = std::make_shared<KdAnimator>();

	m_pCollider->RegisterCollisionShape("BoxEnemyEnemy", m_spEnemyModel, KdCollider::TypeDamage | KdCollider::TypeDebug);

	// 音
	// 草の上に着地した音
	m_wpLandSound[LandSoundType::Grass] = KdAudioManager::Instance().Play3D("Asset/Sounds/SE/grassWalk.wav", m_pos, false);
	if (!m_wpLandSound[LandSoundType::Grass].expired())
	{
		m_wpLandSound[LandSoundType::Grass].lock()->SetVolume(2.0f);
		m_wpLandSound[LandSoundType::Grass].lock()->Stop();
		m_wpLandSound[LandSoundType::Grass].lock()->SetCurveDistanceScaler(0.7f);
	}
	// かたい地面を着地した音
	m_wpLandSound[LandSoundType::Tile] = KdAudioManager::Instance().Play3D("Asset/Sounds/SE/tileWalk.wav", m_pos, false);
	if (!m_wpLandSound[LandSoundType::Tile].expired())
	{
		m_wpLandSound[LandSoundType::Tile].lock()->SetVolume(1.0f);
		m_wpLandSound[LandSoundType::Tile].lock()->Stop();
		m_wpLandSound[LandSoundType::Tile].lock()->SetCurveDistanceScaler(0.7f);
	}
	// キノコではねた時の音
	m_wpLandSound[LandSoundType::Bound] = KdAudioManager::Instance().Play3D("Asset/Sounds/SE/bound.wav", m_pos, false);
	if (!m_wpLandSound[LandSoundType::Bound].expired())
	{
		m_wpLandSound[LandSoundType::Bound].lock()->SetVolume(0.06f);
		m_wpLandSound[LandSoundType::Bound].lock()->Stop();
		m_wpLandSound[LandSoundType::Bound].lock()->SetCurveDistanceScaler(0.7f);
	}

	// ジャンプの音
	m_wpJumpSound = KdAudioManager::Instance().Play3D("Asset/Sounds/SE/boxEnemyJump.wav", m_pos, false);
	if (!m_wpJumpSound.expired())
	{
		m_wpJumpSound.lock()->SetVolume(0.3f);
		m_wpJumpSound.lock()->Stop();
		m_wpJumpSound.lock()->SetCurveDistanceScaler(0.3f);
	}

	// 復活音
	m_wpRespawnSound = KdAudioManager::Instance().Play3D("Asset/Sounds/SE/respawn.wav", m_pos, false);
	if (!m_wpRespawnSound.expired())
	{
		m_wpRespawnSound.lock()->SetVolume(0.7f);
		m_wpRespawnSound.lock()->Stop();
		m_wpRespawnSound.lock()->SetCurveDistanceScaler(0.05f);
	}

	// 最初の当たり判定は箱のみ
	m_pCollider->SetEnable("BoxEnemyEnemy", false);

	// 初期状態は待機状態
	m_nowAction = std::make_shared<Idle>();
}

void BoxEnemy::CarryFlg(bool _carryFlg)
{
	m_carryFlg = true;
	if (_carryFlg == true)
	{
		m_pCollider->SetEnable("BoxEnemyBox", false);
		m_gravity = 0;
		
		// 敵に戻るカウントをリセット
		m_enemyCount = 0;

		// 吸い寄せられる
		m_lerpFlg = true;
	}
	else
	{
		m_pCollider->SetEnable("BoxEnemyBox", true);
		// 敵に戻るカウントをリセット
		m_enemyCount = 0;
	}
}

Math::Vector3 BoxEnemy::GetPos() const
{
	Math::Vector3 pos;
	if (m_spEnemyModel)
	{
		pos = (m_spEnemyModel->FindNode("DeathPoint")->m_worldTransform * m_mWorld).Translation();
	}
	return pos;
}

void BoxEnemy::SetParam(Param _param)
{
	m_param = _param;
	m_pos = m_param.startPos;

	m_setParamFlg = true;
}

void BoxEnemy::OnHit()
{
	m_onHitFlg = true;

	// 当たり判定の切り替え
	m_pCollider->SetEnable("BoxEnemyEnemy", false);
	m_pCollider->SetEnable("BoxEnemyBox", true); 

	// 敵状態じゃなくす
	m_enemyFlg = false;
}

void BoxEnemy::Reset()
{
	CarryObjectBase::Reset();

	// 当たり判定は最初は箱
	m_pCollider->SetEnable("BoxEnemyBox", true);
	m_pCollider->SetEnable("BoxEnemyEnemy", false);

	// レイを少し上から出す処理をやめるフラグ
	m_rayDownFlg = false;

	m_oldPos = Math::Vector3::Zero;

	// 敵に戻るまでのカウント
	m_enemyCount = 0;

	// 敵状態か箱状態かのフラグ
	m_enemyFlg = false;

	// 角度
	m_degAng = m_baseDegAng;

	// 初期状態は待機状態
	m_nowAction = std::make_shared<Idle>();
}

void BoxEnemy::HitJudge()
{
	// レイ判定
	// 動く床関連をリセット
	// 動く床
	m_moveGround.transMat = Math::Matrix::Identity;
	m_moveGround.hitFlg = false;
	// 回る床
	m_rotationGround.transMat = Math::Matrix::Identity;
	m_rotationGround.hitFlg = false;

	// 当たった地面をリセット
	m_wpHitTerrain.reset();

	if (m_isCarry == false)
	{
		// 地面とのレイ当たり判定
		{
			// 当たったかどうか
			bool hitFlg = false;
			// ちょっと上から
			const float enableStepHeight = 0.03f;

			// 当たった座標
			Math::Vector3 hitPos = Math::Vector3::Zero;
			// ノードの座標
			Math::Vector3 nodePos = Math::Vector3::Zero;

			// レイの情報
			KdCollider::RayInfo rayInfo;
			// レイの方向
			rayInfo.m_dir = Math::Vector3::Down;
			// レイのタイプ
			rayInfo.m_type = KdCollider::TypeGround;


			// 真ん中からレイ判定
			if (!m_rayDownFlg)
			{
				// レイの長さ
				rayInfo.m_range = m_gravity + enableStepHeight;
				// レイの座標(キャラの中心)
				rayInfo.m_pos.y = enableStepHeight;
				rayInfo.m_pos += m_pos;
			}
			else
			{
				// レイの長さ
				rayInfo.m_range = m_gravity;
				// レイの座標(キャラの中心)
				rayInfo.m_pos += m_pos;
			}


			// 頂点の座標を回転
			for (int i = 0; i < Max; i++)
			{
				m_edgePos[i] = (Math::Matrix::CreateTranslation(m_edgeBasePos[i]) * m_mWorld).Translation();
			}

			float right = m_edgePos[RightBack].x;
			float back = m_edgePos[RightBack].z;
			float left = m_edgePos[LeftFront].x;
			float front = m_edgePos[LeftFront].z;

			// 真ん中からレイ判定
			hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitTerrain);

			// 回転床の場合、当たっていないことにする
			if (hitFlg)
			{
				if (m_wpHitTerrain.expired() == false)
				{
					if (m_wpHitTerrain.lock()->GetObjectType() == ObjectType::RotationGround)
					{
						hitFlg = false;
						hitPos = Math::Vector3::Zero;
						m_wpHitTerrain.reset();
					}
				}
			}

			// 右前から
			if (!hitFlg)
			{
				rayInfo.m_pos.x = right;
				rayInfo.m_pos.z = front;
				// レイ判定
				hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitTerrain);
			}
			// 左前から
			if (!hitFlg)
			{
				rayInfo.m_pos.x = left;
				rayInfo.m_pos.z = front;
				// レイ判定
				hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitTerrain);
			}
			// 左後ろから
			if (!hitFlg)
			{
				rayInfo.m_pos.x = left;
				rayInfo.m_pos.z = back;
				// レイ判定
				hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitTerrain);
			}
			// 右後ろから
			if (!hitFlg)
			{
				rayInfo.m_pos.x = right;
				rayInfo.m_pos.z = back;
				// レイ判定
				hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitTerrain);
			}

			// 当たっていた時の処理
			if (hitFlg)
			{
				// 当たったオブジェクト
				std::shared_ptr<TerrainBase> spHitObject = m_wpHitTerrain.lock();

				if (spHitObject)
				{
					spHitObject->OnHit();

					// 追尾していいようにする
					m_homingFlg = true;

					switch (spHitObject->GetObjectType())
					{
						// バウンドする床に乗った場合
					case ObjectType::BoundGround:
						// 座標
						m_pos.y = hitPos.y;
						// 重力
						m_gravity = -0.25f;
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
						// 地面にいる
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
						// 地面にいる
						m_isGround = true;
						break;

					default:
						// 座標
						m_pos.y = hitPos.y;
						// 重力
						m_gravity = 0;
						// 地面にいる
						m_isGround = true;
						break;
					}
				}
			}
			else
			{
				// 地面にいない
				m_isGround = false;
			}
		}
		m_rayDownFlg = false;
	}

	// 地面(壁)とのスフィア判定
	if (m_isCarry == true)
	{
		// スフィアの情報
		KdCollider::SphereInfo sphereInfo;
		// スフィアの中心座標
		sphereInfo.m_sphere.Center = m_pos;
		// スフィアの半径
		sphereInfo.m_sphere.Radius = m_edgeBasePos[Up].y - m_edgeBasePos[Down].y;
		sphereInfo.m_sphere.Radius *= 0.5f;
		sphereInfo.m_sphere.Center.y += sphereInfo.m_sphere.Radius + 0.03f;
		// スフィアのタイプ
		sphereInfo.m_type = KdCollider::TypeGround;

		// 当たったかどうかのフラグ
		bool hitFlg = false;

		hitFlg = SphereHitGround(sphereInfo, false);

		if (hitFlg == true)
		{
			if (m_wpPlayer.expired() == false)
			{
				m_wpPlayer.lock()->BackPos();
			}
		}
	}

	// 敵状態の時の壁とのスフィア判定
	if (m_enemyFlg == true)
	{
		// スフィアの情報
		KdCollider::SphereInfo sphereInfo;
		// スフィアの中心座標
		sphereInfo.m_sphere.Center = m_pos;
		// スフィアの半径
		sphereInfo.m_sphere.Radius = m_edgeBasePos[Up].y - m_edgeBasePos[Down].y;
		sphereInfo.m_sphere.Radius *= 0.5f;
		sphereInfo.m_sphere.Center.y += sphereInfo.m_sphere.Radius + 0.03f;
		// スフィアのタイプ
		sphereInfo.m_type = KdCollider::TypeGround;

		// 当たった結果
		std::list<KdCollider::CollisionResult> resultList;

		// 当たったかどうか
		bool hitFlg = false;

		hitFlg = SphereHitJudge(sphereInfo, resultList);

		// 押し出し
		if (hitFlg == true)
		{
			Math::Vector3 adjustVec;
			for (auto& result : resultList)
			{
				result.m_hitDir.Normalize();
				adjustVec += result.m_hitDir * result.m_overlapDistance;
			}

			m_pos += adjustVec;
		}
	}
}

void BoxEnemy::DataLoad()
{
	// JSONファイルを読み込む
	std::ifstream file(m_path.data());
	if (!file.is_open()) return;

	nlohmann::json data;
	file >> data;

	// JSONデータを格納していく
	m_moveSpeed			= data["BoxEnemy"]["m_moveSpeed"];			// 移動量
	m_jumpPow			= data["BoxEnemy"]["m_jumpPow"];			// ジャンプ力
	m_enemyTime			= data["BoxEnemy"]["m_enemyTime"];			// 敵に戻るまでの時間
	m_shakeTime			= data["BoxEnemy"]["m_shakeTime"];			// 震えるまでの時間
	m_enemyChangeLength = data["BoxEnemy"]["m_enemyChangeLength"];	// 箱から敵になる、プレイヤーからの距離
	m_underLine			= data["BoxEnemy"]["m_underLine"];			// 死亡するライン
	m_stayTime			= data["BoxEnemy"]["m_stayTime"];			// ジャンプの待機時間
	m_maxDegAng			= data["BoxEnemy"]["m_maxDegAng"];			// ガタガタの角度制限
	m_baseDegAng		= data["BoxEnemy"]["m_baseDegAng"];			// 初期の角度
	m_jumpChargeTime	= data["BoxEnemy"]["m_jumpChargeTime"];		// ジャンプの前兆用の時間
}

void BoxEnemy::ChangeActionState(std::shared_ptr<StateBase> _nextState)
{
	if (m_nowAction)m_nowAction->Exit(*this);
	m_nowAction = _nextState;
	m_nowAction->Enter(*this);
}

void BoxEnemy::Idle::Enter(BoxEnemy& _owner)
{
	// リスポーン
	_owner.m_pos = _owner.m_param.startPos;
	// 少し上から
	_owner.m_pos.y += 1.0f;
	_owner.m_gravity = 0;
	// 箱状態にする
	_owner.m_enemyFlg = false;
	// 角度をリセット
	_owner.m_degAng = _owner.m_baseDegAng;

	// 当たり判定の切り替え
	_owner.m_pCollider->SetEnable("BoxEnemyEnemy", false);
	_owner.m_pCollider->SetEnable("BoxEnemyBox", true);
}

void BoxEnemy::Idle::Update(BoxEnemy& _owner)
{
	// 接地している時のみ処理
	if (_owner.m_isGround == false) return;

	// プレイヤーが範囲内に来たら震える状態へ移行
	if (_owner.m_wpPlayer.expired() == true) return;
	float length = (_owner.m_wpPlayer.lock()->GetPos() - _owner.m_param.startPos).Length();

	if (length < _owner.m_enemyChangeLength)
	{
		_owner.ChangeActionState(std::make_shared<Shake>());
		return;
	}
}

void BoxEnemy::JumpStay::Update(BoxEnemy& _owner)
{
	// 踏まれていたら箱状態へ移行
	if (_owner.m_onHitFlg == true)
	{
		_owner.ChangeActionState(std::make_shared<Box>());
		return;
	}

	std::shared_ptr<Player> spPlayer = _owner.m_wpPlayer.lock();
	if (!spPlayer) return;

	// 敵からプレイヤーへのベクトル
	Math::Vector3 vec = spPlayer->GetPos() - _owner.m_pos;
	vec.y = 0;
	vec.Normalize();

	// キャラクターの正面方向
	Math::Vector3 nowVec = _owner.m_mWorld.Backward();
	nowVec.Normalize();

	// 向きたい方向
	vec.Normalize();

	// 内積
	float d = nowVec.Dot(vec);

	// 丸め誤差
	d = std::clamp(d, -1.0f, 1.0f);

	// アークコサインで角度に変換
	float ang = DirectX::XMConvertToDegrees(acos(d));

	float _minDegAng = 5.0f;

	// ゆっくり回転するように処理
	if (ang >= 0.1f)
	{
		// 回転制御
		if (ang > _minDegAng)
		{
			ang = _minDegAng;
		}

		// 外積
		Math::Vector3 c = vec.Cross(nowVec);

		if (c.y >= 0)	// 上
		{
			_owner.m_degAng -= ang;
			if (_owner.m_degAng < 0)
			{
				_owner.m_degAng += 360;
			}
		}
		else			//下
		{
			_owner.m_degAng += ang;
			if (_owner.m_degAng >= 360)
			{
				_owner.m_degAng -= 360;
			}
		}
	}

	// 地面にいたら待機時間を進める
	if (_owner.m_isGround == true)
	{
		_owner.m_stayCount++;
		// カウントが待機時間を超えたらジャンプに移行
		if (_owner.m_stayCount > _owner.m_stayTime)
		{
			_owner.ChangeActionState(std::make_shared<JumpMove>());
		}
	}
}

void BoxEnemy::JumpStay::Exit(BoxEnemy& _owner)
{
	// カウントをリセット
	_owner.m_stayCount = 0;
}

void BoxEnemy::JumpMove::Enter(BoxEnemy& _owner)
{
	// ジャンプのアニメーションをする
	_owner.m_spAnimator->SetAnimation(_owner.m_spEnemyModel->GetData()->GetAnimation("Jump"), false);

	// ジャンプする前の溜め時間リセット
	_owner.m_jumpChargeCount = 0;
}

void BoxEnemy::JumpMove::Update(BoxEnemy& _owner)
{
	// 踏まれていたら箱へ移行
	if (_owner.m_onHitFlg == true)
	{
		_owner.ChangeActionState(std::make_shared<Box>());
		return;
	}

	// ジャンプする前兆用のカウント
	_owner.m_jumpChargeCount++;
	// ジャンプする前兆が終了していないなら終了
	if (_owner.m_jumpChargeCount < _owner.m_jumpChargeTime) return;

	// ジャンプする瞬間
	if (_owner.m_jumpChargeCount == _owner.m_jumpChargeTime)
	{
		// ジャンプ
		_owner.m_gravity = -_owner.m_jumpPow;
		// ジャンプ音
		if (_owner.m_wpJumpSound.expired() == false)
		{
			_owner.m_wpJumpSound.lock()->Play();
		}
		return;
	}

	// 接地していたらジャンプ待機状態へ移行
	if (_owner.m_isGround == true)
	{
		_owner.ChangeActionState(std::make_shared<JumpStay>());
		return;
	}

	std::shared_ptr<Player> spPlayer = _owner.m_wpPlayer.lock();
	if (!spPlayer) return;

	// 敵からプレイヤーへのベクトル
	Math::Vector3 vec = spPlayer->GetPos() - _owner.m_pos;
	vec.y = 0;
	vec.Normalize();

	// キャラクターの正面方向
	Math::Vector3 nowVec = _owner.m_mWorld.Backward();
	nowVec.Normalize();

	// 向きたい方向
	vec.Normalize();

	// 内積
	float d = nowVec.Dot(vec);

	// 丸め誤差
	d = std::clamp(d, -1.0f, 1.0f);

	// アークコサインで角度に変換
	float ang = DirectX::XMConvertToDegrees(acos(d));

	float _minDegAng = 5.0f;

	// ゆっくり回転するように処理
	if (ang >= 0.1f)
	{
		// 回転制御
		if (ang > _minDegAng)
		{
			ang = _minDegAng;
		}

		// 外積
		Math::Vector3 c = vec.Cross(nowVec);

		if (c.y >= 0)	// 上
		{
			_owner.m_degAng -= ang;
			if (_owner.m_degAng < 0)
			{
				_owner.m_degAng += 360;
			}
		}
		else			//下
		{
			_owner.m_degAng += ang;
			if (_owner.m_degAng >= 360)
			{
				_owner.m_degAng -= 360;
			}
		}
	}

	// 移動
	_owner.m_pos += vec * _owner.m_moveSpeed;
}

void BoxEnemy::Box::Update(BoxEnemy& _owner)
{
	// 運ばれる状態になったら運ばれている状態にする
	if (_owner.m_carryFlg == true)
	{
		_owner.ChangeActionState(std::make_shared<Carry>());
		_owner.m_carryFlg = false;
		return;
	}

	// 震える状態へ移行
	_owner.m_enemyCount++;
	if (_owner.m_enemyCount > _owner.m_shakeTime)
	{
		_owner.ChangeActionState(std::make_shared<Shake>());
		return;
	}
}

void BoxEnemy::Carry::Enter(BoxEnemy& _owner)
{
	// 当たり判定無効化
	_owner.m_pCollider->SetEnable("BoxEnemyBox", false);

	// 敵に戻るカウントリセット
	_owner.m_enemyCount = 0;

	// 運び状態を開始する
	_owner.m_isCarry = true;
}

void BoxEnemy::Carry::Update(BoxEnemy& _owner)
{
	// 運ばれている状態
	_owner.m_isCarry = true;

	// プレイヤーの運び状態が変更があったら箱状態にする
	if (_owner.m_carryFlg == true)
	{
		_owner.ChangeActionState(std::make_shared<Box>());
		_owner.m_carryFlg = false;
		_owner.m_isCarry = false;
		// 当たり判定有効化
		_owner.m_pCollider->SetEnable("BoxEnemyBox", true);
		return;
	}

	// 震える状態へ移行
	_owner.m_enemyCount++;
	if (_owner.m_enemyCount > _owner.m_shakeTime)
	{
		_owner.ChangeActionState(std::make_shared<Shake>());
		return;
	}
}

void BoxEnemy::Shake::Enter(BoxEnemy& _owner)
{
	// カウントを震え始める数値にする
	_owner.m_enemyCount = _owner.m_shakeTime;
}

void BoxEnemy::Shake::Update(BoxEnemy& _owner)
{
	// 持たれた状態になっていたら持たれている状態にする
	if (_owner.m_carryFlg)
	{
		_owner.ChangeActionState(std::make_shared<Carry>());
		_owner.m_carryFlg = false;
		return;
	}

	// 角度をランダムで決める
	Math::Vector3 shakeAng = { DirectX::XMConvertToRadians(rand() % _owner.m_maxDegAng - _owner.m_maxDegAng / 2.0f), DirectX::XMConvertToRadians(rand() % _owner.m_maxDegAng - _owner.m_maxDegAng / 2.0f), DirectX::XMConvertToRadians(rand() % _owner.m_maxDegAng - _owner.m_maxDegAng / 2.0f) };
	// 行列作成
	_owner.m_shakeMat = Math::Matrix::CreateFromYawPitchRoll(shakeAng);

	// 敵に戻るカウントを進める
	_owner.m_enemyCount++;
	// 時間を超えたら敵に戻る 追尾なしのジャンプに移行
	if (_owner.m_enemyCount > _owner.m_enemyTime)
	{
		_owner.ChangeActionState(std::make_shared<Jump>());
		return;
	}
}

void BoxEnemy::Shake::Exit(BoxEnemy& _owner)
{
	// 敵に戻るカウントをリセット
	_owner.m_enemyCount = 0;
	// 行列をリセット
	_owner.m_shakeMat = Math::Matrix::Identity;
}

void BoxEnemy::Jump::Enter(BoxEnemy& _owner)
{
	// 当たり判定を切り替える
	_owner.m_pCollider->SetEnable("BoxEnemyEnemy", true);
	_owner.m_pCollider->SetEnable("BoxEnemyBox", false);

	// 少しジャンプ
	_owner.m_gravity = -_owner.m_jumpPow;

	// 運び状態を解除
	_owner.m_carryFlg = false;
	_owner.m_isCarry = false;

	// プレイヤーの運び状態を終了
	if (_owner.m_wpPlayer.expired() == false)
	{
		_owner.m_wpPlayer.lock()->CancelCarryMode();
	}

	// 敵状態にする
	_owner.m_enemyFlg = true;

	// ジャンプ音を鳴らす
	if (_owner.m_wpJumpSound.expired() == false)
	{
		_owner.m_wpJumpSound.lock()->Play();
	}
}

void BoxEnemy::Jump::Update(BoxEnemy& _owner)
{
	std::shared_ptr<Player> spPlayer = _owner.m_wpPlayer.lock();
	if (!spPlayer) return;

	// 敵からプレイヤーへのベクトル
	Math::Vector3 vec = spPlayer->GetPos() - _owner.m_pos;
	vec.y = 0;
	vec.Normalize();

	// キャラクターの正面方向
	Math::Vector3 nowVec = _owner.m_mWorld.Backward();
	nowVec.Normalize();

	// 向きたい方向
	vec.Normalize();

	// 内積
	float d = nowVec.Dot(vec);

	// 丸め誤差
	d = std::clamp(d, -1.0f, 1.0f);

	// アークコサインで角度に変換
	float ang = DirectX::XMConvertToDegrees(acos(d));

	float _minDegAng = 5.0f;

	// ゆっくり回転するように処理
	if (ang >= 0.1f)
	{
		// 回転制御
		if (ang > _minDegAng)
		{
			ang = _minDegAng;
		}

		// 外積
		Math::Vector3 c = vec.Cross(nowVec);

		if (c.y >= 0)	// 上
		{
			_owner.m_degAng -= ang;
			if (_owner.m_degAng < 0)
			{
				_owner.m_degAng += 360;
			}
		}
		else			//下
		{
			_owner.m_degAng += ang;
			if (_owner.m_degAng >= 360)
			{
				_owner.m_degAng -= 360;
			}
		}
	}

	// 踏まれたら箱状態へ移行
	if (_owner.m_onHitFlg)
	{
		_owner.ChangeActionState(std::make_shared<Box>());
		return;
	}
	// 接地したらジャンプ待機状態へ移行
	if (_owner.m_isGround == true)
	{
		_owner.ChangeActionState(std::make_shared<JumpStay>());
		return;
	}
}