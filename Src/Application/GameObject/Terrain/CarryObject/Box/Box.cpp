#include "Box.h"
#include "../../../Character/Player/Player.h"
#include "../../TerrainBase.h"
#include "Application/main.h"
#include "../../../../Scene/SceneManager.h"

#include "../../MoveObjectRideProcess/MoveObjectRideProcess.h"

void Box::Update()
{
	// デバッグモード中は持てるエリアを描画
	if (SceneManager::Instance().GetDebug())
	{
		//m_pDebugWire->AddDebugSphere(m_pos, m_param.area, kGreenColor);
	}

	m_oldPos = m_pos;

	if (m_pauseFlg) return;

	// 運ばれていない時の処理
	if (m_carryFlg == false)
	{
		// 動く床に乗っていた時の処理
		if (m_moveGround.hitFlg == true)
		{
			if (m_wpHitTerrain.expired() == false)
			{
				m_pos = MoveObjectRideProcess::Instance().MoveGroundRide(m_mWorld, m_moveGround.transMat, m_wpHitTerrain.lock()->GetPos());
			}
		}

		// 重力
		m_gravity += m_gravityPow;
		m_pos.y -= m_gravity;
		// 重力の制限
		if (m_gravity >= m_maxGravity)
		{
			m_gravity = m_maxGravity;
		}
	}

	// Y座標が一定以下になるとリスポーン
	if (m_pos.y < m_underLine)
	{
		m_pos = m_param.startPos;
		// 少し上から
		m_pos.y += 1.0f;
		m_gravity = 0;
		m_degAng = 0;
		// 復活音を鳴らす
		if (m_wpRespawnSound.expired() == false)
		{
			m_wpRespawnSound.lock()->Play();
		}
	}

	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);
	m_mWorld = rotMat * transMat;
}

void Box::PostUpdate()
{
	if (m_pauseFlg) return;

	bool landFlg = m_landFlg;
	HitJudge();

	// 着地した瞬間
	if (landFlg == false && m_landFlg == true)
	{
		if (m_firstLandFlg == false)
		{
			m_firstLandFlg = true;
		}
		else
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
	// 復活音
	if (m_wpRespawnSound.expired() == false)
	{
		if (m_wpRespawnSound.lock()->IsPlaying() == true)
		{
			m_wpRespawnSound.lock()->SetPos(m_pos);
		}
	}

	// 運ばれていない時の処理
	if (m_carryFlg == false)
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
	// 運ばれている時の処理
	else
	{
		// プレイヤー
		std::shared_ptr<Player> spPlayer = m_wpPlayer.lock();

		if (spPlayer)
		{
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
			Math::Vector3 carryPos = (m_spModel->FindNode("Carry")->m_worldTransform * mat).Translation();

			m_pos += playerCarryPos - carryPos;
		}
	}

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);

	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));

	m_mWorld = rotMat * transMat;
}

void Box::Init()
{
	CarryObjectBase::Init();

	DataLoad();

	m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Terrain/CarryObject/Box/Box.gltf");

	// 角っこの座標
	Math::Vector3 rightBackUp	= m_spModel->FindNode("RightBackUp")->m_worldTransform.Translation();
	Math::Vector3 leftFrontDown = m_spModel->FindNode("LeftFrontDown")->m_worldTransform.Translation();
	m_edgeBasePos[RightBack]	= { rightBackUp.x, 0, rightBackUp.z };		// 右後ろ
	m_edgeBasePos[RightFront]	= { rightBackUp.x, 0, leftFrontDown.z };	// 右前
	m_edgeBasePos[LeftBack]		= { leftFrontDown.x, 0, rightBackUp.z };	// 左後ろ
	m_edgeBasePos[LeftFront]	= { leftFrontDown.x, 0, leftFrontDown.z };	// 左前
	m_edgeBasePos[Up]			= { 0, rightBackUp.y, 0 };					// 上
	m_edgeBasePos[Down]			= { 0, leftFrontDown.y, 0 };				// 下

	for (int i = 0; i < Max; i++)
	{
		m_edgePos[i] = m_edgeBasePos[i];
	}

	// 中心座標
	m_centerPos = m_spModel->FindNode("Center")->m_worldTransform.Translation();

	// コライダー
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Box", m_spModel, KdCollider::TypeGround | KdCollider::TypeDebug);

	// オブジェクトのタイプ
	m_objectType = ObjectType::Box;

	// ベースのオブジェクトのタイプ
	m_baseObjectType = BaseObjectType::CarryObject;

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	// 音
	// 草の上に着地した音
	m_wpLandSound[LandSoundType::Grass] = KdAudioManager::Instance().Play3D("Asset/Sounds/SE/grassWalk.wav", m_pos, false);
	if (!m_wpLandSound[LandSoundType::Grass].expired())
	{
		m_wpLandSound[LandSoundType::Grass].lock()->SetVolume(2.0f);
		m_wpLandSound[LandSoundType::Grass].lock()->Stop();
	}
	// かたい地面を着地した音
	m_wpLandSound[LandSoundType::Tile] = KdAudioManager::Instance().Play3D("Asset/Sounds/SE/tileWalk.wav", m_pos, false);
	if (!m_wpLandSound[LandSoundType::Tile].expired())
	{
		m_wpLandSound[LandSoundType::Tile].lock()->SetVolume(1.0f);
		m_wpLandSound[LandSoundType::Tile].lock()->Stop();
	}
	// キノコではねた時の音
	m_wpLandSound[LandSoundType::Bound] = KdAudioManager::Instance().Play3D("Asset/Sounds/SE/bound.wav", m_pos, false);
	if (!m_wpLandSound[LandSoundType::Bound].expired())
	{
		m_wpLandSound[LandSoundType::Bound].lock()->SetVolume(0.06f);
		m_wpLandSound[LandSoundType::Bound].lock()->Stop();
	}
	// 復活音
	m_wpRespawnSound = KdAudioManager::Instance().Play3D("Asset/Sounds/SE/respawn.wav", m_pos, false);
	if (!m_wpRespawnSound.expired())
	{
		m_wpRespawnSound.lock()->SetVolume(1.0f);
		m_wpRespawnSound.lock()->Stop();
		m_wpRespawnSound.lock()->SetCurveDistanceScaler(0.05f);
	}
}

void Box::CarryFlg(bool _carryFlg)
{
	m_carryFlg = _carryFlg;
	if (m_carryFlg == true)
	{
		m_pCollider->SetEnable("Box", false);
		m_gravity = 0;
	}
	else
	{
		m_pCollider->SetEnable("Box", true);
	}
}

void Box::SetParam(Param _param)
{
	m_param = _param;
	m_pos = m_param.startPos;
}

void Box::Reset()
{
	CarryObjectBase::Reset();

	m_degAng = 0;

	m_pCollider->SetEnable("Box", true);

	m_landFlg = false;
	m_firstLandFlg = false;

	m_mWorld = Math::Matrix::CreateTranslation(m_pos);
}

void Box::HitJudge()
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

	if (m_carryFlg == false)
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
				rayInfo.m_pos.x = m_edgePos[RightFront].x;
				rayInfo.m_pos.z = m_edgePos[RightFront].z;
				// レイ判定
				hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitTerrain);
			}
			// 左前から
			if (!hitFlg)
			{
				rayInfo.m_pos.x = m_edgePos[LeftFront].x;
				rayInfo.m_pos.z = m_edgePos[LeftFront].z;
				// レイ判定
				hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitTerrain);
			}
			// 左後ろから
			if (!hitFlg)
			{
				rayInfo.m_pos.x = m_edgePos[LeftBack].x;
				rayInfo.m_pos.z = m_edgePos[LeftBack].z;
				// レイ判定
				hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitTerrain);
			}
			// 右後ろから
			if (!hitFlg)
			{
				rayInfo.m_pos.x = m_edgePos[RightBack].x;
				rayInfo.m_pos.z = m_edgePos[RightBack].z;
				// レイ判定
				hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitTerrain);
			}

			// 当たっていた時の処理
			if (hitFlg)
			{
				// 当たったオブジェクト
				std::shared_ptr<TerrainBase> spHitObject = m_wpHitTerrain.lock();

				m_landFlg = true;

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
						break;

					default:
						// 座標
						m_pos.y = hitPos.y;
						// 重力
						m_gravity = 0;
						break;
					}
				}
			}
			// 当たっていなかった時の処理
			else
			{
				m_landFlg = false;
			}
		}
		m_rayDownFlg = false;
	}

	// 地面(壁)とのスフィア判定
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

		if (m_carryFlg)
		{
			if (hitFlg == true)
			{
				if (m_wpPlayer.expired() == false)
				{
					m_wpPlayer.lock()->BackPos();
				}
			}
		}
	}
}

void Box::DataLoad()
{
	// JSONファイルを読み込む
	std::ifstream file(m_path.data());
	if (!file.is_open()) return;

	nlohmann::json data;
	file >> data;

	// JSONデータを格納していく
	m_underLine = data["Box"]["m_underLine"];
}
