#include "BoxEnemy.h"
#include "../../../Character/Player/Player.h"
#include "../../TerrainBase.h"
#include "Application/main.h"
#include "../../../../Scene/SceneManager.h"

void BoxEnemy::Update()
{
	m_oldPos = m_pos;

	if (m_pauseFlg) return;
	// 運ばれていない時の処理
	if (m_carryFlg == false)
	{
		// 重力
		m_gravity += m_gravityPow;
		m_pos.y -= m_gravity;
		// 重力の制限
		if (m_gravity >= m_maxGravity)
		{
			m_gravity = m_maxGravity;
		}

		Math::Vector3 carryPos = (m_spModel->FindNode("Carry")->m_worldTransform * m_mWorld).Translation();
		//m_pDebugWire->AddDebugSphere(carryPos, 0.1f, kRedColor);
	}

	if (SceneManager::Instance().GetDebug())
	{
		m_pDebugWire->AddDebugSphere(m_pos, m_param.area, kGreenColor);
	}

	HitJudge();

	// Y座標が一定以下になるとリスポーン
	if (m_pos.y < m_underLine)
	{
		m_pos = m_param.startPos;
		m_pos.y += 1.0f;
		m_gravity = 0;
	}
}

void BoxEnemy::PostUpdate()
{
	if (m_pauseFlg) return;
	// 運ばれていない時の処理
	if (m_carryFlg == false)
	{
		Math::Matrix transMat = Math::Matrix::Identity;
		// 動く床に当たっていた時の処理
		if (m_moveGround.hitFlg)
		{
			// 動く床の動く前の逆行列
			Math::Matrix inverseMatrix = DirectX::XMMatrixInverse(nullptr, m_moveGround.transMat);
			// 動く床から見たプレイヤーの座標行列
			Math::Matrix playerMat = Math::Matrix::CreateTranslation(m_pos) * inverseMatrix;

			// 動く床を探す
			std::shared_ptr<TerrainBase> spHitObject = m_wpHitTerrain.lock();

			if (spHitObject)
			{
				// 動く床の動いた後の行列
				Math::Matrix afterMoveGroundMat = Math::Matrix::CreateTranslation(spHitObject->GetMatrix().Translation());

				// 座標を確定
				m_pos = afterMoveGroundMat.Translation() + playerMat.Translation();
			}
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
			// 回転床が回転する角度
			Math::Vector3 lotDegAng = spHitTerrain->GetParam().degAng;

			// Z軸回転の場合
			if (lotDegAng.z != 0)
			{
				vec.z = 0;
				float length = vec.Length();
				// 移動する前の回転床から見た箱の角度
				float degAng = DirectX::XMConvertToDegrees(atan2(vec.x, vec.y));

				Application::Instance().m_log.Clear();
				Application::Instance().m_log.AddLog("%.2f", degAng);

				if (degAng <= 90 && degAng >= -90)
				{
					degAng -= 90;
					if (degAng < 0)
					{
						degAng += 360;
					}
					degAng = 360 - degAng;

					degAng += lotDegAng.z;
					m_pos.x = spHitTerrain->GetPos().x + length * cos(DirectX::XMConvertToRadians(degAng));
					m_pos.y = spHitTerrain->GetPos().y + length * sin(DirectX::XMConvertToRadians(degAng));
				}
			}
			// Y軸回転の場合
			else if (lotDegAng.y != 0)
			{
				vec = m_pos - spHitTerrain->GetPos();
				vec.y = 0;
				float length = vec.Length();
				// 移動する前の回転床から見たプレイヤーの角度
				float degAng = DirectX::XMConvertToDegrees(atan2(vec.x, vec.z));

				//if (degAng <= 90 && degAng >= -90)
				{
					degAng -= 90;
					if (degAng < 0)
					{
						degAng += 360;
					}
					if (degAng >= 360)
					{
						degAng -= 360;
					}
					degAng = 360 - degAng;

					// 回転床が回転する角度
					degAng -= lotDegAng.y;
					m_pos.x = spHitTerrain->GetPos().x + length * cos(DirectX::XMConvertToRadians(degAng));
					m_pos.z = spHitTerrain->GetPos().z + length * sin(DirectX::XMConvertToRadians(degAng));
				}
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

			// 頂点の座標を回転
			for (int i = 0; i < Max; i++)
			{
				m_edgePos[i] = (Math::Matrix::CreateTranslation(m_edgeBasePos[i]) * m_mWorld).Translation();
			}

			Math::Vector3 carryPos = (m_spModel->FindNode("Carry")->m_worldTransform * m_mWorld).Translation();

			//m_pDebugWire->AddDebugSphere(carryPos, 0.1f, kRedColor);

			playerCarryPos += spPlayer->GetPos();
			//m_pDebugWire->AddDebugSphere(playerCarryPos, 0.1f, kBlackColor);

			m_pos += playerCarryPos - carryPos;
		}
	}

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);

	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));

	m_mWorld = rotMat * transMat;
}

void BoxEnemy::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/CarryObject/Box/Box.gltf");
	}

	// 角っこの座標
	Math::Vector3 rightBackUp = m_spModel->FindNode("RightBackUp")->m_worldTransform.Translation();
	Math::Vector3 leftFrontDown = m_spModel->FindNode("LeftFrontDown")->m_worldTransform.Translation();
	m_edgeBasePos[RightBack] = { rightBackUp.x, 0, rightBackUp.z };		// 右後ろ
	m_edgeBasePos[RightFront] = { rightBackUp.x, 0, leftFrontDown.z };	// 右前
	m_edgeBasePos[LeftBack] = { leftFrontDown.x, 0, rightBackUp.z };	// 左後ろ
	m_edgeBasePos[LeftFront] = { leftFrontDown.x, 0, leftFrontDown.z };	// 左前
	m_edgeBasePos[Up] = { 0, rightBackUp.y, 0 };					// 上
	m_edgeBasePos[Down] = { 0, leftFrontDown.y, 0 };				// 下

	for (int i = 0; i < Max; i++)
	{
		m_edgePos[i] = m_edgeBasePos[i] + m_pos;
	}

	// 中心座標
	m_centerPos = m_spModel->FindNode("Center")->m_worldTransform.Translation();

	// コライダー
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Box", m_spModel, KdCollider::TypeGround);

	// オブジェクトのタイプ
	m_objectType = ObjectType::Box;

	// ベースのオブジェクトのタイプ
	m_baseObjectType = BaseObjectType::CarryObject;

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
}

void BoxEnemy::CarryFlg(bool _carryFlg)
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

void BoxEnemy::SetParam(Param _param)
{
	m_param = _param;
	m_pos = m_param.startPos;
}

void BoxEnemy::HitJudge()
{
	// 運ばれている状態なら当たり判定をしない
	//if (m_carryFlg == true)return;

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
			const float enableStepHeight = 0.1f;

			// 当たった座標
			Math::Vector3 hitPos = Math::Vector3::Zero;
			// ノードの座標
			Math::Vector3 nodePos = Math::Vector3::Zero;

			// レイの情報
			KdCollider::RayInfo rayInfo;
			// レイの長さ
			rayInfo.m_range = m_gravity + enableStepHeight;
			// レイの座標(キャラの中心)
			rayInfo.m_pos.y = enableStepHeight;
			rayInfo.m_pos += m_pos;
			// レイの方向
			rayInfo.m_dir = Math::Vector3::Down;
			// レイのタイプ
			rayInfo.m_type = KdCollider::TypeGround;

			// レイ判定
			hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitTerrain, false);


			float right = m_edgePos[RightBack].x;
			float back = m_edgePos[RightBack].z;
			float left = m_edgePos[LeftFront].x;
			float front = m_edgePos[LeftFront].z;

			// 右前から
			if (!hitFlg)
			{
				Math::Vector3 pos = rayInfo.m_pos;
				pos.x = right;
				pos.z = front;
				// レイ判定
				hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitTerrain, false);
			}
			// 左前から
			if (!hitFlg)
			{
				Math::Vector3 pos = rayInfo.m_pos;
				pos.x = left;
				pos.z = front;
				// レイ判定
				hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitTerrain, false);
			}
			// 左後ろから
			if (!hitFlg)
			{
				Math::Vector3 pos = rayInfo.m_pos;
				pos.x = left;
				pos.z = back;
				// レイ判定
				hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitTerrain, false);
			}
			// 右後ろから
			if (!hitFlg)
			{
				Math::Vector3 pos = rayInfo.m_pos;
				pos.x = right;
				pos.z = back;
				// レイ判定
				hitFlg = RayHitJudge(rayInfo, hitPos, m_wpHitTerrain, false);
			}

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

						// 跳ねた時の音を鳴らす
						//if (m_boundSound.flg == false)
						//{
						//	if (!m_boundSound.wpSound.expired())
						//	{
						//		m_boundSound.wpSound.lock()->Play();
						//		m_boundSound.flg = true;
						//	}
						//}
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
		}
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
