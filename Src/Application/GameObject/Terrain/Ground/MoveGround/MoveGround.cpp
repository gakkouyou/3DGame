#include "MoveGround.h"
#include "../../../../main.h"
#include "../../../../Scene/SceneManager.h"
#include "Application/../Framework/Math/KdEasing.h"

void MoveGround::Update()
{
	// ポーズ画面中は更新しない
	if (m_pauseFlg == true) return;

	if (!m_stopFlg)
	{
		if (m_moveFlg)
		{
			// 進行度を進める
			m_progress += m_speed;

			// イージング
			float progress = -(cos(DirectX::XM_PI * m_progress) - 1.0f) * 0.5f;

			// スタート→ゴール
			if (m_moveDirFlg == false)
			{
				// ゴールに着いた時の処理
				if (m_progress >= 1.0f)
				{
					// 座標をゴール座標にする
					m_param.pos = m_param.goalPos;
					// 止める
					m_moveFlg = false;
					// 反転
					m_moveDirFlg = true;
					m_progress = 1.0f;
					m_speed *= -1;
				}
			}
			// ゴール→スタート
			else
			{
				// スタートに着いた時の処理
				if (m_progress <= 0.0f)
				{
					// 座標をスタート座標にする
					m_param.pos = m_param.startPos;
					// 止める
					m_moveFlg = false;
					// 反転
					m_moveDirFlg = false;
					m_progress = 0.0f;
					m_speed *= -1;
				}
			}
			// 座標
			m_param.pos = Math::Vector3::Lerp(m_param.startPos, m_param.goalPos, progress);
		}

		// 待機カウントを進める
		if (!m_stopFlg && !m_moveFlg)
		{
			m_stayCnt++;
		}

		// 待機が終了したら進める
		if (m_stayCnt > m_param.stayTime)
		{
			m_moveFlg = true;
			m_stayCnt = 0;
		}
	}
	// ストップフラグがtrueの時、全てリセットする
	else
	{
		m_moveFlg = false;
		m_stayCnt = 0;
		m_moveDirFlg = false;
		m_param.pos = m_param.startPos;
		m_stopFlg = false;
	}

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_param.pos);
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_param.scale);
	

	m_mWorld = scaleMat * transMat;
}

void MoveGround::PostUpdate()
{
	
}

void MoveGround::Init()
{
	m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Terrain/Ground/MoveGround/moveGround.gltf");

	// 当たり判定
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("MoveBlockCollision", m_spModel, KdCollider::TypeGround | KdCollider::TypeDebug);

	m_moveFlg = true;

	// オブジェクトのタイプ
	m_objectType = ObjectType::MoveGround;

	TerrainBase::Init();

	// 描画タイプ
	m_drawType = eDrawTypeLit | eDrawTypeDepthOfShadow;

	m_param.pos		= Math::Vector3::Zero;
	m_param.startPos = Math::Vector3::Zero;
	m_param.goalPos	= Math::Vector3::Zero;
	m_param.speed	= 0;
}

void MoveGround::Reset()
{
	m_param.pos = m_param.startPos;

	m_moveFlg = true;
	m_stayCnt = 0;
	m_moveDirFlg = false;
	m_param.pos = m_param.startPos;
	m_stopFlg = false;

	m_pauseFlg = false;

	// 等速で動いていた場合の、スタートからゴールまで行くフレーム数
	float moveFrame = (m_param.startPos - m_param.goalPos).Length() / m_param.speed;

	// 進行度を進める値を決める
	m_speed = 1 / moveFrame;

	m_progress = 0;
}

void MoveGround::SetParam(const Param& _param)
{
	m_param.startPos	= _param.startPos;	// スタートの座標
	m_param.pos			= _param.startPos;	// 座標
	m_param.goalPos		= _param.goalPos;	// ゴールの座標
	m_param.speed		= _param.speed;		// 速度
	m_param.stayTime	= _param.stayTime;	// 待機時間
	m_param.scale		= _param.scale;		// 拡縮

	m_stopFlg = true;
	m_setParamFlg = true;

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_param.pos);
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_param.scale);

	m_mWorld = scaleMat * transMat;

	// 等速で動いていた場合の、スタートからゴールまで行くフレーム数
	float moveFrame = (m_param.startPos - m_param.goalPos).Length() / m_param.speed;

	// 進行度を進める値を決める
	m_speed = 1 / moveFrame;
}