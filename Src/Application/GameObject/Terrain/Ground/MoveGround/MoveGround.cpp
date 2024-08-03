#include "MoveGround.h"
#include "../../../../main.h"
#include "../../../../Scene/SceneManager.h"

void MoveGround::Update()
{
	if (!m_stopFlg)
	{
		/*if (m_goal > m_goalMax)
		{
			m_goal = 0;
			m_moveVec *= -1;
			m_moveFlg = false;
		}
		if (m_moveFlg && !m_stopFlg)
		{
			m_info.pos += m_moveVec * m_info.speed;
			m_goal += m_info.speed;
		}*/

		Math::Vector3 moveVec = Math::Vector3::Zero;

		if (m_moveFlg)
		{
			// スタート→ゴール
			if (m_moveDirFlg == false)
			{
				moveVec = m_info.goalPos - m_info.pos;
				// 実際の移動
				// もしゴールまでの距離がスピードより小さかったら、座標をゴール座標にする
				if (moveVec.Length() < m_info.speed)
				{
					m_info.pos = m_info.goalPos;
					m_moveFlg = false;
					m_moveDirFlg = true;
				}
				// 大きかったらスピード分進める
				else
				{
					moveVec.Normalize();
					m_info.pos += moveVec * m_info.speed;
				}
			}
			// ゴール→スタート
			else
			{
				moveVec = m_info.startPos - m_info.pos;
				// 実際の移動
				// もしゴールまでの距離がスピードより小さかったら、座標をゴール座標にする
				if (moveVec.Length() < m_info.speed)
				{
					m_info.pos = m_info.startPos;
					m_moveFlg = false;
					m_moveDirFlg = !m_moveDirFlg;
				}
				// 大きかったらスピード分進める
				else
				{
					moveVec.Normalize();
					m_info.pos += moveVec * m_info.speed;
				}
			}
		}

		// 待機カウントを進める
		if (!m_stopFlg && !m_moveFlg)
		{
			m_stayCnt++;
		}

		// 待機が終了したら進める
		if (m_stayCnt > m_info.stayTime)
		{
			m_moveFlg = true;
			m_stayCnt = 0;
		}
	}

	Math::Matrix transMat;
	transMat = Math::Matrix::CreateTranslation(m_info.pos.x, m_info.pos.y, m_info.pos.z);

	m_mWorld = transMat;
}

void MoveGround::PostUpdate()
{
	
}

void MoveGround::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/Ground/NormalGround/normalGround.gltf");
	}

	// 当たり判定
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("MoveBlockCollision", m_spModel, KdCollider::TypeGround);

	m_moveFlg = true;

	// オブジェクトのタイプ
	m_objectType = KdGameObject::ObjectType::MoveGround;

	// 描画タイプ
	m_drawType = eDrawTypeLit;

	m_info.pos		= Math::Vector3::Zero;
	m_info.startPos = Math::Vector3::Zero;
	m_info.goalPos	= Math::Vector3::Zero;
	m_info.speed	= 0;
}

void MoveGround::SetInfo(Math::Vector3 _startPos, Math::Vector3 _goalPos, float _speed, int _stayTime)
{
	m_info.startPos = _startPos;
	m_info.pos = _startPos;
	m_info.goalPos = _goalPos;
	m_info.speed = _speed;
	m_info.stayTime = _stayTime;
}

void MoveGround::Reset()
{
	m_info.pos = m_info.startPos;

	m_moveFlg = true;

	m_stayCnt = 0;
}