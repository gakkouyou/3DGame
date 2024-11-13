#include "BoxEnemy.h"
#include "../../../Character/Player/Player.h"
#include "../../TerrainBase.h"
#include "Application/main.h"
#include "../../../../Scene/SceneManager.h"

#include <../Library/tinygltf/json.hpp>

void BoxEnemy::Update()
{
	m_oldPos = m_pos;

	if (m_pauseFlg) return;
	// 敵状態での更新
	if(m_enemyFlg == true)
	{
		EnemyUpdate();
	}

	// 運ばれていない時の処理
	if (m_carryFlg == false)
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
		m_pDebugWire->AddDebugSphere(m_pos, m_param.area, kGreenColor);
	}



	// 音座標更新
	for (int i = 0; i < LandSoundType::MaxNum; i++)
	{
		if (m_wpLandSound[i].lock()->IsPlaying() == true)
		{
			m_wpLandSound[i].lock()->SetPos(m_pos);
		}
	}

	// Y座標が一定以下になると死亡
	if (m_pos.y < m_underLine)
	{
		// リスポーン
		m_pos = m_param.startPos;
		// 少し上から
		m_pos.y += 1.0f;
		m_gravity = 0;
		// 箱状態にする
		m_enemyFlg = false;
		// プレイヤーが近くに来たら震える状態にする
		m_aliveFlg = false;
		// 角度をリセット
		m_degAng = 180;

		// 当たり判定の切り替え
		m_pCollider->SetEnable("BoxEnemyEnemy", false);
		m_pCollider->SetEnable("BoxEnemyBox", true);
	}

	// プレイヤーが近くに来たら震えて敵状態になる
	if (m_aliveFlg == false)
	{
		// 地面にいる時
		if (m_airFlg == false)
		{
			float length = (m_wpPlayer.lock()->GetPos() - m_param.startPos).Length();

			if (length < m_enemyChangeLength)
			{
				// 敵状態にする
				m_enemyCount = m_shakeTime;
				// 追尾しないようにする
				m_homingFlg = false;
				m_aliveFlg = true;
			}
		}
	}

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);

	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));

	m_mWorld = rotMat * transMat;
}

void BoxEnemy::PostUpdate()
{
	if (m_pauseFlg) return;

	bool airFlg = m_airFlg;

	HitJudge();

	// 着地した瞬間
	if (airFlg == false && m_airFlg == true)
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

				if (degAng < -90)
				{
					m_rayDownFlg = true;
				}

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
				m_degAng += lotDegAng.y;
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

			Math::Vector3 carryPos = (m_spModel->FindNode("Carry")->m_worldTransform * mat).Translation();

			//m_pDebugWire->AddDebugSphere(carryPos, 0.1f, kRedColor);

			//m_pDebugWire->AddDebugSphere(playerCarryPos, 0.1f, kBlackColor);

			m_pos += playerCarryPos - carryPos;
		}
	}

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);

	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));



	// 震えている状態に入ったかどうかのフラグ
	bool flg = false;
	// 箱状態の時にカウントを進めていく
	if (m_enemyFlg == false && m_aliveFlg == true)
	{
		m_enemyCount++;
		// 一定時間たったら敵に戻る
		if (m_enemyCount > m_enemyTime)
		{
			// 敵にする
			m_enemyFlg = true;

			// 追尾しないようにする
			m_homingFlg = false;
			
			// 当たり判定を切り替える
			m_pCollider->SetEnable("BoxEnemyEnemy", true);
			m_pCollider->SetEnable("BoxEnemyBox", false);
			
			// 少しジャンプ
			m_gravity -= m_jumpPow;

			// カウントをリセット
			m_enemyCount = 0;

			// 運ばれている状態を終了
			m_carryFlg = false;

			// プレイヤーの運び状態を終了
			if (m_wpPlayer.expired() == false)
			{
				m_wpPlayer.lock()->CancelCarryMode();
			}

			// 生存フラグ
			m_aliveFlg = true;

			// ジャンプのカウントリセット
			m_stayCount = 0;
		}
		// 一定時間たったら震え始める
		else if (m_enemyCount > m_shakeTime)
		{
			Math::Vector3 shakeAng = { DirectX::XMConvertToRadians(rand() % m_maxDegAng - m_maxDegAng / 2.0f), DirectX::XMConvertToRadians(rand() % m_maxDegAng - m_maxDegAng / 2.0f), DirectX::XMConvertToRadians(rand() % m_maxDegAng - m_maxDegAng / 2.0f) };
			Math::Matrix shakeMat = Math::Matrix::CreateFromYawPitchRoll(shakeAng);
			m_mWorld = rotMat * shakeMat * transMat;
			flg = true;
		}
	}
	if(flg == false)
	{
		m_mWorld = rotMat * transMat;
	}
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
	DataLoad();
	DataSave();

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
		m_spEnemyModel = std::make_shared<KdModelData>();
		m_spEnemyModel->Load("Asset/Models/Terrain/CarryObject/BoxEnemy/boxEnemy.gltf");
	}

	m_pCollider->RegisterCollisionShape("BoxEnemyEnemy", m_spEnemyModel, KdCollider::TypeDamage | KdCollider::TypeDebug);

	// 音
	// 草の上に着地した音
	m_wpLandSound[LandSoundType::Grass] = KdAudioManager::Instance().Play3D("Asset/Sounds/SE/grassWalk.wav", m_pos, false);
	if (!m_wpLandSound[LandSoundType::Grass].expired())
	{
		m_wpLandSound[LandSoundType::Grass].lock()->SetVolume(0.06f);
		m_wpLandSound[LandSoundType::Grass].lock()->Stop();
	}
	// かたい地面を着地した音
	m_wpLandSound[LandSoundType::Tile] = KdAudioManager::Instance().Play3D("Asset/Sounds/SE/tileWalk.wav", m_pos, false);
	if (!m_wpLandSound[LandSoundType::Tile].expired())
	{
		m_wpLandSound[LandSoundType::Tile].lock()->SetVolume(0.06f);
		m_wpLandSound[LandSoundType::Tile].lock()->Stop();
	}
	// キノコではねた時の音
	m_wpLandSound[LandSoundType::Bound] = KdAudioManager::Instance().Play3D("Asset/Sounds/SE/bound.wav", m_pos, false);
	if (!m_wpLandSound[LandSoundType::Bound].expired())
	{
		m_wpLandSound[LandSoundType::Bound].lock()->SetVolume(0.06f);
		m_wpLandSound[LandSoundType::Bound].lock()->Stop();
	}

	// 最初の当たり判定は箱のみ
	m_pCollider->SetEnable("BoxEnemyEnemy", false);
}

void BoxEnemy::CarryFlg(bool _carryFlg)
{
	m_carryFlg = _carryFlg;
	if (m_carryFlg == true)
	{
		m_pCollider->SetEnable("BoxEnemyBox", false);
		m_gravity = 0;
		
		// 敵に戻るカウントをリセット
		m_enemyCount = 0;
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
	// 箱状態にする
	m_enemyFlg = false;

	// 当たり判定の切り替え
	m_pCollider->SetEnable("BoxEnemyEnemy", false);
	m_pCollider->SetEnable("BoxEnemyBox", true);
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

	// 生存フラグ
	m_aliveFlg = false;

	// 敵状態か箱状態かのフラグ
	m_enemyFlg = false;

	// 追尾しないようにする
	m_homingFlg = false;

	// 地面に付いた瞬間かどうかを判断するフラグ
	m_landFlg = true;

	// 角度
	m_degAng = 180;
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
			const float enableStepHeight = 0.03f;

			// 当たった座標
			Math::Vector3 hitPos = Math::Vector3::Zero;
			// ノードの座標
			Math::Vector3 nodePos = Math::Vector3::Zero;

			// レイの情報
			KdCollider::RayInfo rayInfo;
			//// レイの長さ
			//rayInfo.m_range = m_gravity + enableStepHeight;
			//// レイの座標(キャラの中心)
			//rayInfo.m_pos.y = enableStepHeight;
			//rayInfo.m_pos += m_pos;
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
						// 空中にいない
						m_airFlg = false;
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
						// 空中にいない
						m_airFlg = false;
						break;

					default:
						// 座標
						m_pos.y = hitPos.y;
						// 重力
						m_gravity = 0;
						// 空中にいない
						m_airFlg = false;
						break;
					}
				}
			}
			else
			{
				m_airFlg = true;
			}
		}
		m_rayDownFlg = false;
	}

	// 地面(壁)とのスフィア判定
	if (m_carryFlg == true)
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

		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			obj->Intersects(sphereInfo, &resultList);
		}

		// めり込み具合
		float maxOverLap = 0;
		// 当たったかどうかのフラグ
		bool hitFlg = false;
		// 当たった方向
		Math::Vector3 hitDir;

		for (auto& result : resultList)
		{
			if (result.m_overlapDistance > maxOverLap)
			{
				maxOverLap = result.m_overlapDistance;
				hitDir = result.m_hitDir;
				hitFlg = true;
			}
		}

		if (hitFlg == true)
		{
			hitDir.y = 0;
			hitDir.Normalize();
			m_pos += hitDir * maxOverLap;
		}
	}
}

void BoxEnemy::EnemyUpdate()
{
	if (SceneManager::Instance().GetDebug() == true) return;

	if (m_aliveFlg == false) return;

	std::shared_ptr<Player> spPlayer = m_wpPlayer.lock();
	if (!spPlayer) return;

	// 敵からプレイヤーへのベクトル
	Math::Vector3 vec = spPlayer->GetPos() - m_pos;
	vec.y = 0;
	vec.Normalize();

	// キャラクターの正面方向
	Math::Vector3 nowVec = m_mWorld.Backward();
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
			m_degAng -= ang;
			if (m_degAng < 0)
			{
				m_degAng += 360;
			}
		}
		else			//下
		{
			m_degAng += ang;
			if (m_degAng >= 360)
			{
				m_degAng -= 360;
			}
		}
	}

	// 空中にいて、追尾していい場合移動
	if (m_airFlg == true && m_homingFlg == true)
	{
		m_pos += vec * m_moveSpeed;
	}
	// 地面にいる場合カウントを進める
	else if(m_airFlg == false)
	{
		m_stayCount++;
		// カウントが待機時間を超えたらジャンプさせる
		if (m_stayCount > m_stayTime)
		{
			m_gravity = -m_jumpPow;
			m_stayCount = 0;
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
	for (const auto& objData : data["GameObject"])
	{
		m_moveSpeed			= objData["m_moveSpeed"];			// 移動量
		m_jumpPow			= objData["m_jumpPow"];				// ジャンプ力
		m_enemyTime			= objData["m_enemyTime"];			// 敵に戻るまでの時間
		m_shakeTime			= objData["m_shakeTime"];			// 震えるまでの時間
		m_enemyChangeLength = objData["m_enemyChangeLength"];	// 箱から敵になる、プレイヤーからの距離
	}
}

void BoxEnemy::DataSave()
{
	nlohmann::json objData;

	// リストごと
	nlohmann::json objStat;
	objStat["m_moveSpeed"]			= m_moveSpeed;			// 移動量
	objStat["m_jumpPow"]			= m_jumpPow;			// ジャンプ力
	objStat["m_enemyTime"]			= m_enemyTime;			// 敵に戻るまでの時間
	objStat["m_shakeTime"]			= m_shakeTime;			// 震えるまでの時間
	objStat["m_enemyChangeLength"]	= m_enemyChangeLength;	// 箱から敵になる、プレイヤーからの距離

	objStat["name"] = m_name.data();

	// ゲームオブジェクトに追加
	objData["GameObject"][m_name.data()] = objStat;

	// ファイルに書き込む
	std::ofstream file(m_path.data());
	if (!file.is_open()) return;

	// JSONデータをファイルに書き込む
	file << std::setw(4) << objData << std::endl;	//Pretty print with 4-space indent
	file.close();
}