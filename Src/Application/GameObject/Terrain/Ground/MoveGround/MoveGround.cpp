#include "MoveGround.h"
#include "../../../../main.h"
#include "../../../../Scene/SceneManager.h"

void MoveGround::Update()
{
	// ポーズ画面中は更新しない
	if (m_pauseFlg == true) return;

	if (!m_stopFlg)
	{
		Math::Vector3 moveVec = Math::Vector3::Zero;

		if (m_moveFlg)
		{
			// サインカーブ用の角度
			m_degAng += 1;
			// 0～1にする
			m_progress = (sin(DirectX::XMConvertToRadians(m_degAng)));

			// 角度が上限まで行ったら、進行度を１にする
			if (m_degAng >= 360)
			{
				m_progress = 1.0f;
			}

			static float a = 0;
			static float d = 0.1;
			a += d;
			if (a > 1 || a < 0)
			{
				d *= -1;
			}

			// スタート→ゴール
			if (m_moveDirFlg == false)
			{
				moveVec = m_param.goalPos - m_param.pos;
				// 実際の移動
				// 進行度が１になったらゴール座標にする
				if (m_progress >= 1.0f)
				{
					m_param.pos = m_param.goalPos;
					m_moveFlg = false;
					m_moveDirFlg = true;
					m_degAng = m_startDegAng;
					m_progress = 0;
				}
				// 大きかったらスピード分進める
				else
				{
					moveVec.Normalize();
					m_param.pos = Math::Vector3::Lerp(m_param.startPos, m_param.goalPos, m_progress);
					//m_param.pos += moveVec * m_param.speed;
				}
			}
			// ゴール→スタート
			else
			{
				moveVec = m_param.startPos - m_param.pos;
				// 実際の移動
				// 進行度が１になったらスタート座標にする
				if (m_progress >= 1.0f)
				{
					m_param.pos = m_param.startPos;
					m_moveFlg = false;
					m_moveDirFlg = false;
					m_degAng = m_startDegAng;
					m_progress = 0;
				}
				// 大きかったらスピード分進める
				else
				{
					moveVec.Normalize();
					m_param.pos = Math::Vector3::Lerp(m_param.goalPos, m_param.startPos, m_progress);
					//m_param.pos += moveVec * m_param.speed;
				}
			}
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
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/Ground/MoveGround/moveGround.gltf");
	}

	// 当たり判定
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("MoveBlockCollision", m_spModel, KdCollider::TypeGround);

	m_moveFlg = true;

	// オブジェクトのタイプ
	m_objectType = ObjectType::MoveGround;

	TerrainBase::Init();

	// 描画タイプ
	m_drawType = eDrawTypeLit;

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

	// サインカーブ用の角度に足しこむ数値
	m_addDegAng = 360.0f / moveFrame;
}