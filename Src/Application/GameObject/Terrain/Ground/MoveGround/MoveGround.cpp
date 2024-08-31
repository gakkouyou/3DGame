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
			// スタート→ゴール
			if (m_moveDirFlg == false)
			{
				moveVec = m_param.goalPos - m_param.pos;
				// 実際の移動
				// もしゴールまでの距離がスピードより小さかったら、座標をゴール座標にする
				if (moveVec.Length() < m_param.speed)
				{
					m_param.pos = m_param.goalPos;
					m_moveFlg = false;
					m_moveDirFlg = true;
				}
				// 大きかったらスピード分進める
				else
				{
					moveVec.Normalize();
					m_param.pos += moveVec * m_param.speed;
				}
			}
			// ゴール→スタート
			else
			{
				moveVec = m_param.startPos - m_param.pos;
				// 実際の移動
				// もしゴールまでの距離がスピードより小さかったら、座標をゴール座標にする
				if (moveVec.Length() < m_param.speed)
				{
					m_param.pos = m_param.startPos;
					m_moveFlg = false;
					m_moveDirFlg = !m_moveDirFlg;
				}
				// 大きかったらスピード分進める
				else
				{
					moveVec.Normalize();
					m_param.pos += moveVec * m_param.speed;
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

void MoveGround::SetParam(Math::Vector3 _startPos, Math::Vector3 _goalPos, Math::Vector3 _scale, float _speed, int _stayTime, Math::Vector3 _degAng)
{
	m_param.startPos	= _startPos;
	m_param.pos			= _startPos;
	m_param.goalPos		= _goalPos;
	m_param.speed		= _speed;
	m_param.stayTime	= _stayTime;
	m_param.scale		= _scale;

	m_stopFlg = true;
	m_setParamFlg = true;

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_param.pos);
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_param.scale);

	m_mWorld = scaleMat * transMat;
}

void MoveGround::Reset()
{
	m_param.pos = m_param.startPos;

	m_stayCnt = 0;

	m_moveFlg = false;
	m_stayCnt = 0;
	m_moveDirFlg = false;
	m_param.pos = m_param.startPos;
	m_stopFlg = false;
}