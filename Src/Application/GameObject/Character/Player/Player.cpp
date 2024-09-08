#include "Player.h"
#include "../../../Scene/SceneManager.h"
#include "../../Camera/CameraBase.h"
#include "../../Terrain/TerrainBase.h"

#include "../../Effect/PlayerSmoke/PlayerSmoke.h"

#include "../../../main.h"

void Player::Update()
{
	// デバッグモード中は更新しない
	if (SceneManager::Instance().GetDebug()) return;
	// ポーズ画面中は更新しない
	if (m_pauseFlg == true) return;

	// Stop以外のフラグが立っていたらStopをおろす
	if ((m_situationType & (~SituationType::Stop)) && (m_situationType & SituationType::Stop))
	{
		m_situationType &= (~SituationType::Stop);
	}
	// 何もフラグが立っていなかったらStopにする
	if (m_situationType == 0)
	{
		m_situationType = SituationType::Stop;
	}
	// 移動初期化
	m_moveVec = Math::Vector3::Zero;

	// 更新前のプレイヤーの状態を保持
	UINT oldSituationType = m_situationType;

	if (m_stopFlg == false)
	{
		// WASDで移動
		if (GetAsyncKeyState('W') & 0x8000)
		{
			m_moveVec.z += 1.0f;
			m_situationType |= SituationType::Walk;
		}
		if (GetAsyncKeyState('A') & 0x8000)
		{
			m_moveVec.x -= 1.0f;
			m_situationType |= SituationType::Walk;
		}
		if (GetAsyncKeyState('S') & 0x8000)
		{
			m_moveVec.z -= 1.0f;
			m_situationType |= SituationType::Walk;
		}
		if (GetAsyncKeyState('D') & 0x8000)
		{
			m_moveVec.x += 1.0f;
			m_situationType |= SituationType::Walk;
		}

		// 歩き状態の時にSHIFTキーを押すと、ダッシュになる
		if (m_situationType & SituationType::Walk)
		{
			if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
			{
				// 空中でないなら走り状態に移行できる
				if ((m_situationType & SituationType::Air) == 0)
				{
					m_situationType |= SituationType::Run;
				}
				// 走り状態なら歩き状態を解除する
				if (m_situationType & SituationType::Run)
				{
					m_situationType &= (~SituationType::Walk);
				}
			}
			else
			{
				m_situationType &= (~SituationType::Run);
			}
		}
	}

	// カメラの向きで移動方向を補正
	m_moveVec = m_moveVec.TransformNormal(m_moveVec, m_wpCamera.lock()->GetRotationYMatrix());
	m_moveVec.Normalize();

	// 動いていなかったら止まっている状態にする。
	if (m_moveVec.LengthSquared() == 0)
	{
		m_situationType &= (~SituationType::Walk);
		m_situationType &= (~SituationType::Run);

		// 歩いている音を止める
		m_walkSoundFlg = false;
		m_nowWalkSoundFlg = false;
	}

	// 空中にいなくて動いている時、足元に煙を出す
	if ((m_situationType & SituationType::Air) == 0)
	{
		// 歩いている時
		if (m_situationType & SituationType::Walk)
		{
			// 一定時間置きに煙を出す
			if (m_smokeCount % m_walkSmokeTime == 0)
			{
				std::shared_ptr<PlayerSmoke> smoke = std::make_shared<PlayerSmoke>();
				smoke->Init();
				smoke->SetPos(m_pos);
				SceneManager::Instance().AddObject(smoke);
				// 煙を出すカウントをリセット
				m_smokeCount = 0;
			}
			// 煙を出すカウント
			m_smokeCount++;
		}
		// 走っている時
		else if (m_situationType & SituationType::Run)
		{
			// 一定時間置きに煙を出す
			if (m_smokeCount % m_runSmokeTime == 0)
			{
				// 煙を生み出す
				std::shared_ptr<PlayerSmoke> smoke = std::make_shared<PlayerSmoke>();
				smoke->Init();
				smoke->SetPos(m_pos);
				SceneManager::Instance().AddObject(smoke);
				// 煙を出すカウントをリセット
				m_smokeCount = 0;
			}
			// 煙を出すカウント
			m_smokeCount++;
		}
	}
	else
	{
		m_smokeCount = 0;
		// 歩いている音を止める
		m_walkSoundFlg = false;
	}

	if (m_stopFlg == false)
	{
		// ジャンプ
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			// 空中じゃなければジャンプする
			if (!(m_situationType & SituationType::Air))
			{
				m_situationType |= SituationType::Jump;
				m_gravity = -m_jumpPow;
				m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("Jump"), false);

				// ジャンプ音を鳴らす
				if (m_jumpSound.flg == false)
				{
					if (!m_jumpSound.wpSound.expired())
					{
						m_jumpSound.wpSound.lock()->Play();
						m_jumpSound.flg = true;
					}
				}
			}
		}
	}

	if (m_situationType != oldSituationType)
	{
		// 止まっている状態→歩きor走り
		if ((m_situationType & SituationType::Walk) || (m_situationType & SituationType::Run))
		{
			if (oldSituationType & SituationType::Stop)
			{
				// 空中ではアニメーションしない
				if (!(m_situationType & SituationType::Air))
				{
					m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("RightWalk"), false);

					// 歩く音を鳴らす状態にする
					m_walkSoundFlg = true;
				}
			}
		}
	}

	if (m_spAnimator->IsAnimationEnd())
	{
		// 歩く音を止める
		if (m_wpWalkSound[WalkSoundType::Grass].expired() == false)
		{
			m_wpWalkSound[WalkSoundType::Grass].lock()->Stop();
			m_nowWalkSoundFlg = false;
		}
		// 歩いている状態or走っている状態の時
		// 空中にいたらアニメーションしない
		if (((m_situationType & SituationType::Walk) || (m_situationType & SituationType::Run)) && !(m_situationType & SituationType::Air))
		{
			// 右足を出す
			if (m_walkAnimeDirFlg == true)
			{
				// アニメーションセット
				m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("RightWalk"), false);
				// 左足を出す状態にする
				m_walkAnimeDirFlg = false;

				// 歩く音を鳴らす状態にする
				m_walkSoundFlg = true;
			}
			// 左足を出す
			else
			{
				// アニメーションセット
				m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("LeftWalk"), false);
				// 右足を出す状態にする
				m_walkAnimeDirFlg = true;

				// 歩く音を鳴らす状態にする
				m_walkSoundFlg = true;
			}
		}
		else
		{
			// 歩いていなかったら歩きアニメーションの方向フラグをfalseにする
			m_walkAnimeDirFlg = false;
		}

		// ジャンプ状態の時
		if (m_situationType & SituationType::Jump)
		{
			m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("Jump"), false);
		}

		// アニメーションが終わった時に止まっていたら止まっているアニメーションをする
		if (m_situationType & SituationType::Stop)
		{
			m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("Stop"), true);
		}
	}

	// 移動
	// 歩き
	if (m_situationType & SituationType::Walk)
	{
		m_pos += m_moveVec * m_moveSpeed;
	}
	// 走り
	else if (m_situationType & SituationType::Run)
	{
		m_pos += m_moveVec * m_runSpeed;
	}

	// 重力
	m_gravity += m_gravityPow;
	m_pos.y -= m_gravity;
	// 重力の制限
	if (m_gravity >= m_maxGravity)
	{
		m_gravity = m_maxGravity;
	}

	// 落下死
	if (m_pos.y < -15.0f)
	{
		m_aliveFlg = false;
	}

	// 移動中
	// 回転処理
	if (m_situationType & SituationType::Walk || m_situationType & SituationType::Run)
	{
		// 回転
		RotationCharacter(m_angle, m_moveVec);

		// 回転行列
		m_rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));
	}

	// エフェクシア座標追尾===========================================================
	if (!m_wpEffekseer.expired())
	{
		m_wpEffekseer.lock()->SetPos(m_pos);
		if (!m_wpEffekseer.lock()->IsPlaying() == true)
		{
			m_wpEffekseer.reset();
		}
	}
	// ===============================================================================


	// 当たり判定
	HitJudge();

	// ダメージを受けた時の処理
	if (m_damage.nowDamageFlg == true)
	{
		DamageProcess();
	}

	// 着地した瞬間
	if ((m_situationType & SituationType::Air) == 0)
	{
		if (oldSituationType & SituationType::Air)
		{
			// ジャンプ音のフラグをfalseに
			m_jumpSound.flg = false;
		}
	}

	// 何の地面に乗っているかによって、音を変える
	if (!m_wpHitTerrain.expired())
	{
		ObjectType type = m_wpHitTerrain.lock()->GetObjectType();
		switch (type)
		{
		// 草の音
		case ObjectType::NormalGround:
			m_walkSoundType = WalkSoundType::Grass;
			break;

		// コツコツ見たいな音
		case ObjectType::FenceBar:
		case ObjectType::FencePillar:
		case ObjectType::MoveGround:
		case ObjectType::DropGround:
			m_walkSoundType = WalkSoundType::Tile;
			break;
		}
	}

	// 歩いた時の音を鳴らす
	if (m_walkSoundFlg)
	{
		if (m_nowWalkSoundFlg == false)
		{
			if (m_wpWalkSound[m_walkSoundType].expired() == false)
			{
				m_wpWalkSound[m_walkSoundType].lock()->Play();
				m_nowWalkSoundFlg = true;
			}
		}
	}
	// 止める
	else
	{
		if (m_wpWalkSound[m_walkSoundType].expired() == false)
		{
			m_wpWalkSound[m_walkSoundType].lock()->Stop();
		}
	}

	if (m_goalFlg)
	{
		GoalProcess();
	}
}

void Player::PostUpdate()
{
	// ポーズ画面中は更新しない
	if (m_pauseFlg == true) return;

	Math::Matrix transMat = Math::Matrix::Identity;
	// 動く床に当たっていた時の処理
	if (m_moveGround.hitFlg)
	{
		// 動く床の動く前の逆行列
		Math::Matrix inverseMatrix = DirectX::XMMatrixInverse(nullptr, m_moveGround.transMat);
		// 動く床から見たプレイヤーの座標行列
		Math::Matrix playerMat = Math::Matrix::CreateTranslation(m_pos) * inverseMatrix;

		// 動く床を探す
		std::shared_ptr<TerrainBase> spHitObject	= m_wpHitTerrain.lock();

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

	transMat = Math::Matrix::CreateTranslation(m_pos);
	
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_scale);

	m_mWorld = scaleMat * m_rotMat * transMat;

	// アニメーションの更新
	// 止まっていたらアニメーションしない
	if (m_situationType bitand SituationType::Run)
	{
		m_spAnimator->AdvanceTime(m_spModel->WorkNodes(), 2.0f);
	}
	else
	{
		m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
	}
	m_spModel->CalcNodeMatrices();
}

void Player::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		if (m_damage.drawFlg)
		{
			KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
		}
	}
}

void Player::DrawLit()
{
	if (m_spModel)
	{
		if (m_damage.drawFlg)
		{
			KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
		}
	}
}

void Player::Init()
{
	if (!m_spModel)
	{
		// モデル
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Character/Player/player.gltf");

		// 初期のアニメーション
		m_spAnimator = std::make_shared<KdAnimator>();
		//m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("Stop"));
	}

	// オブジェクトのタイプ
	m_objectType = ObjectType::Player;

	m_baseObjectType = BaseObjectType::Player;

	// 移動速度
	m_moveSpeed = 0.1f;

	m_pos = { 0, 0.3f, 0 };

	// ジャンプ力
	m_jumpPow = 0.125f;

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	// 移動が出来なくなるフラグ
	m_stopFlg = true;
	
	// ライフ
	m_life = m_maxLife;














	// 音
	// 草の上を歩いた音
	m_wpWalkSound[WalkSoundType::Grass] = KdAudioManager::Instance().Play("Asset/Sounds/SE/grassWalk.wav", false);
	if (!m_wpWalkSound[WalkSoundType::Grass].expired())
	{
		m_wpWalkSound[WalkSoundType::Grass].lock()->SetVolume(0.1f);
		m_wpWalkSound[WalkSoundType::Grass].lock()->Stop();
	}
	m_nowWalkSoundFlg = false;
	// かたい地面を歩いた音
	m_wpWalkSound[WalkSoundType::Tile] = KdAudioManager::Instance().Play("Asset/Sounds/SE/tileWalk.wav", false);
	if (!m_wpWalkSound[WalkSoundType::Tile].expired())
	{
		m_wpWalkSound[WalkSoundType::Tile].lock()->SetVolume(0.1f);
		m_wpWalkSound[WalkSoundType::Tile].lock()->Stop();
	}

	// ジャンプ音
	m_jumpSound.wpSound = KdAudioManager::Instance().Play("Asset/Sounds/SE/jump.wav", false);
	if (!m_jumpSound.wpSound.expired())
	{
		m_jumpSound.wpSound.lock()->SetVolume(0.02f);
		m_jumpSound.wpSound.lock()->Stop();
	}
	m_jumpSound.flg = false;

	// 敵を踏んだ時の音
	m_stampSound.wpSound = KdAudioManager::Instance().Play("Asset/Sounds/SE/stamp.wav", false);
	if (!m_stampSound.wpSound.expired())
	{
		m_stampSound.wpSound.lock()->SetVolume(0.02f);
		m_stampSound.wpSound.lock()->Stop();
	}
	m_stampSound.flg = false;

	// キノコではねた時の音
	m_boundSound.wpSound = KdAudioManager::Instance().Play("Asset/Sounds/SE/bound.wav", false);
	if (!m_boundSound.wpSound.expired())
	{
		m_boundSound.wpSound.lock()->SetVolume(0.02f);
		m_boundSound.wpSound.lock()->Stop();
	}
	m_boundSound.flg = false;
}

void Player::Reset()
{
	CharacterBase::Reset();

	// 座標
	m_pos = { 0, 0.3f, 0 };
	
	// 生存フラグ
	m_aliveFlg = true;

	// 状態
	m_situationType = SituationType::Stop;

	// 角度
	m_angle = 0;
	m_rotMat = Math::Matrix::CreateRotationY(m_angle);

	// アニメーションフラグ
	m_walkAnimeDirFlg = false;

	m_stopFlg = true;

	// ライフ
	m_life = m_maxLife;

	// ダメージ受けた時の変数をリセット
	m_damage.nowDamageFlg = false;
	m_damage.damageCount = 0;
	m_damage.drawFlg = true;
}

// 当たり判定
void Player::HitJudge()
{
	// 当たったオブジェクトリストをリセット
	m_wpHitObjectList.clear();

	// 地面との当たり判定
	HitJudgeGround();

	if (m_stopFlg == false)
	{
		// 触れたらイベントが発生する
		HitJudgeEvent();

		// 敵との当たり判定
		HitJudgeEnemy();
	}
}

void Player::HitJudgeGround()
{
	// 動く床関連をリセット
	// 動く床
	m_moveGround.transMat = Math::Matrix::Identity;
	m_moveGround.hitFlg = false;
	// 回る床
	m_rotationGround.transMat = Math::Matrix::Identity;
	m_rotationGround.rotMat = Math::Matrix::Identity;
	m_rotationGround.hitFlg = false;

	// 当たった地面をリセット
	m_wpHitTerrain.reset();

	// 地面とのレイ当たり判定
	{
		// 当たった座標
		Math::Vector3 hitPos = Math::Vector3::Zero;
		// レイのスタート座標
		Math::Vector3 startPos = Math::Vector3::Zero;
		// ノードの座標
		Math::Vector3 nodePos = Math::Vector3::Zero;
		bool hitFlg = false;

		// 真ん中からレイ判定
		startPos.y = m_enableStepHeight;
		startPos += m_pos;
		hitFlg = RayHitGround(startPos, hitPos, Math::Vector3::Down, m_gravity + m_enableStepHeight);

		// 当たっていなかったら右足からのレイ判定をする
		if (hitFlg == false)
		{
			// 右足の場所
			nodePos = m_spModel->FindNode("RightLeg")->m_localTransform.Translation();
			nodePos *= m_scale;
			// 回転処理
			startPos.x = nodePos.x * cos(DirectX::XMConvertToRadians(-m_angle));
			startPos.z = nodePos.x * sin(DirectX::XMConvertToRadians(-m_angle));
			// 段差許容
			startPos.y = m_enableStepHeight;
			// 座標足しこみ
			startPos += m_pos;
			// 右足からのレイ判定
			hitFlg = RayHitGround(startPos, hitPos, Math::Vector3::Down, m_gravity + m_enableStepHeight);

			m_pDebugWire->AddDebugSphere(startPos, 0.5f);
		}


		// 当たっていなかったら左足からのレイ判定をする
		if (hitFlg == false)
		{
			// 左足の場所
			nodePos = m_spModel->FindNode("LeftLeg")->m_localTransform.Translation();
			nodePos *= m_scale;
			// 回転処理
			startPos.x = nodePos.x * cos(DirectX::XMConvertToRadians(m_angle));
			startPos.z = nodePos.x * sin(DirectX::XMConvertToRadians(m_angle));
			// 段差許容
			startPos.y = m_enableStepHeight;
			// 座標足しこみ
			startPos += m_pos;
			// 左足からのレイ判定
			hitFlg = RayHitGround(startPos, hitPos, Math::Vector3::Down, m_gravity + m_enableStepHeight);
			m_pDebugWire->AddDebugSphere(startPos, 0.5f, kBlueColor);
		}
		if (hitFlg)
		{
			// 当たったオブジェクト
			std::shared_ptr<TerrainBase> spHitObject = m_wpHitTerrain.lock();
			Application::Instance().m_log.Clear();

			// 音のフラグをリセットする
			m_boundSound.flg = false;	// きのこで跳ねた時の音

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
					//// 空中にいない
					Application::Instance().m_log.AddLog("Bound\n");

					// 跳ねた時の音を鳴らす
					if (m_boundSound.flg == false)
					{
						if (!m_boundSound.wpSound.expired())
						{
							m_boundSound.wpSound.lock()->Play();
							m_boundSound.flg = true;
						}
					}
					break;

					// 動く床に乗った場合
				case ObjectType::MoveGround:
				case ObjectType::DropGround:
					// 座標
					m_pos.y = hitPos.y;
					// 重力
					m_gravity = 0;
					// 空中にいない
					m_situationType &= (~SituationType::Air);
					m_situationType &= (~SituationType::Jump);
					// 動く床の動く前の行列
					m_moveGround.transMat = Math::Matrix::CreateTranslation(spHitObject->GetPos());
					// 動く床に当たったかどうかのフラグ
					m_moveGround.hitFlg = true;
					Application::Instance().m_log.AddLog("Move\n");
					break;

					// 回る床に乗った場合
				case ObjectType::RotationGround:
					// 座標
					m_pos.y = hitPos.y;
					// 重力
					m_gravity = 0;
					// 空中にいない
					m_situationType &= (~SituationType::Air);
					m_situationType &= (~SituationType::Jump);
					// 当たったかどうか
					m_rotationGround.hitFlg = true;
					Application::Instance().m_log.AddLog("Rot\n");
					break;

				default:
					// 座標
					m_pos.y = hitPos.y;
					// 重力
					m_gravity = 0;
					// 空中にいない
					m_situationType &= (~SituationType::Air);
					m_situationType &= (~SituationType::Jump);
					Application::Instance().m_log.AddLog("Normal\n");
					break;
				}
			}
		}
		else
		{
			// 空中にいる
			m_situationType |= SituationType::Air;
		}
	}

	// 地面(壁)とのスフィア判定
	{
		// 方向
		std::list<Math::Vector3> hitDirList;
		// スフィアの中心座標
		Math::Vector3 centerPos = m_pos;
		// スフィアの半径
		float radius = 0.25f;
		centerPos.y += radius + 0.75f;

		// 当たったかどうかのフラグ
		bool hitFlg = false;
		// めり込んだ距離
		float maxOverLap = 0.0f;

		hitFlg = SphereHitJudge(centerPos, radius, KdCollider::TypeGround, hitDirList, maxOverLap, false);

		if (hitFlg == false)
		{
			centerPos = m_pos;
			centerPos.y += radius + 0.25f;
			hitFlg = SphereHitJudge(centerPos, radius, KdCollider::TypeGround, hitDirList, maxOverLap, false);
		}

		Application::Instance().m_log.AddLog("%d\n", hitDirList.size());

		// 当たった場合
		if (hitFlg)
		{
			Math::Vector3 dir;

			for (auto& hitDir : hitDirList)
			{
				dir += hitDir;
			}

			// Y軸の補正はなし
			dir.y = 0;
			dir.Normalize();
			m_pos += dir * maxOverLap;
		}
	}
}

void Player::HitJudgeEvent()
{
	Math::Vector3 spherePos = m_pos;
	spherePos.y += 1.0f;

	bool hitFlg = false;
	hitFlg = SphereHitJudge(spherePos, 0.25f, KdCollider::TypeEvent, true);

	if (hitFlg)
	{
		// 当たったオブジェクト
		std::shared_ptr<KdGameObject> spHitObject;

		// 探す
		for (auto& hitObject : m_wpHitObjectList)
		{
			if (!hitObject.expired())
			{
				if (hitObject.lock()->GetBaseObjectType() == BaseObjectType::Event)
				{
					spHitObject = hitObject.lock();
					break;
				}
			}
		}
		if (spHitObject->GetObjectType() == ObjectType::HealItem)
		{
			m_life += 1;
			// 回復エフェクト
			if (!m_effectFlg)
			{
				m_effectFlg = true;
				m_wpEffekseer = KdEffekseerManager::GetInstance().Play("Heal/heal.efkefc", m_pos, { 0.0f, 0.0f, 0 }, 0.3f, 1.0f, false);
			}
		}
		if (spHitObject)
		{
			spHitObject->OnHit();
		}
	}
}

void Player::HitJudgeEnemy()
{
	// 方向
	Math::Vector3 hitDir = Math::Vector3::Zero;
	// スフィアの中心座標
	Math::Vector3 centerPos = m_pos;
	// スフィアの半径
	float radius = 0.25f;
	centerPos.y += radius;

	// 当たったかどうかのフラグ
	bool hitFlg = false;
	// めり込んだ距離
	float maxOverLap = 0.0f;

	hitFlg = SphereHitJudge(centerPos, radius, KdCollider::TypeDamage, hitDir, maxOverLap, true);

	// 当たっていた時の処理
	if (hitFlg)
	{
		// 当たったオブジェクト
		std::shared_ptr<KdGameObject> spHitObject;

		// 音のフラグをリセット
		m_stampSound.flg = false;	// 敵を踏んだ時の音

		// 敵を探す
		for (auto& hitObject : m_wpHitObjectList)
		{
			if (!hitObject.expired())
			{
				if (hitObject.lock()->GetBaseObjectType() == BaseObjectType::Enemy)
				{
					spHitObject = hitObject.lock();
					break;
				}
			}
		}

		// 敵の座標
		Math::Vector3 enemyPos = spHitObject->GetPos();
		enemyPos.y += 0.5f;
		// 敵からプレイヤーのベクトル
		Math::Vector3 vec = m_pos - enemyPos;
		vec.z = 0;
		// 敵から見たプレイヤーの角度
		float degAng = DirectX::XMConvertToDegrees(atan2(vec.x, vec.y));
		// 上半分なら倒す
		if (degAng < 90 && degAng > -90)
		{
			// 敵を踏んだ時の処理
			spHitObject->OnHit();
			m_gravity = -0.15f;

			// 敵を踏んだ時の音
			if (m_stampSound.flg == false)
			{
				if (!m_stampSound.wpSound.expired())
				{
					m_stampSound.wpSound.lock()->Play();
					m_stampSound.flg = true;
				}
			}
		}
		// 下半分ならダメージを受ける
		else
		{
			if (m_damage.nowDamageFlg == false)
			{
				// ダメージ処理をする
				m_damage.nowDamageFlg = true;
				// ライフを減らす
				m_life -= 1;
				// ライフが０以下になったら死亡
				if (m_life <= 0)
				{
					m_aliveFlg = false;
				}
			}
		}
	}
}

void Player::DamageProcess()
{
	// 生きている時だけ
	if (m_aliveFlg == true)
	{
		m_damage.damageCount++;
		if (m_damage.damageCount % m_damage.blinkingTime == 0)
		{
			m_damage.drawFlg = !m_damage.drawFlg;
		}

		if (m_damage.damageCount > m_damage.damageTime)
		{
			m_damage.nowDamageFlg = false;
			m_damage.damageCount = 0;
			m_damage.drawFlg = true;
		}
	}
}

void Player::GoalProcess()
{
	m_goalStayCnt++;
	if (m_goalStayCnt > m_goalStayTime)
	{
		m_pos.x = m_goalPos.x;
		m_pos.z = m_goalPos.z;
		m_angle = 180;
		m_rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));
		m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("Goal"), true);
	}
}