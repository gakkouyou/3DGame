﻿#include "DropGround.h"

void DropGround::Update()
{
	// ポーズ画面中は更新しない
	if (m_pauseFlg == true) return;

	// 触れられていなかったらリセット
	if (!m_onHitFlg)
	{
		m_stayCount = 0;
		if (!m_dropFlg)
		{
			m_param.pos = m_param.startPos;
		}
	}
	m_onHitFlg = false;

	// 落ちる処理
	if (m_dropFlg)
	{
		m_param.pos.y -= m_param.speed;
		m_param.pos.x = m_param.pos.x;
		m_param.pos.z = m_param.pos.z;

		// 消えるまでのカウント
		m_vanishCount++;
		// 消えた時の処理
		if (m_vanishCount > m_vanishTime)
		{
			m_aliveFlg = false;	// 生存フラグ
			m_drawType = 0;		// 描画しないようにする
			m_pCollider->SetEnable("DropGroundCollision", false);	// 当たり判定を無効にする
			m_vanishCount = 0;	// 消えるまでのカウントをリセット
			m_dropFlg = false;	// 落ちているフラグをリセット
		}
	}

	// 消えている時の処理
	if (m_aliveFlg == false)
	{
		// リスポーンのカウント
		m_respawnCount++;
		// リスポーン処理
		if (m_respawnCount > m_respawnTime)
		{
			m_aliveFlg = true;	// 生存フラグ
			m_drawType = eDrawTypeLit;	// 描画するようにする
			m_pCollider->SetEnable("DropGroundCollision", true);	// 当たり判定を有効にする
			m_respawnCount = 0;	// リスポーンのカウントをリセット
			m_param.pos = m_param.startPos;	// 座標をリセット
		}
	}

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_param.pos.x, m_param.pos.y, m_param.pos.z);
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_param.scale);

	m_mWorld = scaleMat * transMat;
}

void DropGround::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/Ground/DropGround/dropGround.gltf");
	}

	// 当たり判定
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("DropGroundCollision", m_spModel, KdCollider::TypeGround);

	// オブジェクトのタイプ
	m_objectType = ObjectType::DropGround;

	TerrainBase::Init();

	// 描画タイプ
	m_drawType = eDrawTypeLit;

	m_param.stayTime = 60;

	m_aliveFlg = true;
}

void DropGround::OnHit()
{
	m_onHitFlg = true;
	m_stayCount++;
	if (m_stayCount > m_param.stayTime)
	{
		m_dropFlg = true;
	}

	if (m_dropFlg == false)
	{
		int add = rand() % 2;

		if (add == 0)
		{
			m_param.pos.x = m_param.startPos.x + 0.2f;
		}
		else
		{
			m_param.pos.x = m_param.startPos.x - 0.2f;
		}

		Math::Matrix transMat;
		transMat = Math::Matrix::CreateTranslation(m_param.pos.x, m_param.pos.y, m_param.pos.z);

		m_mWorld = transMat;
	}
}

void DropGround::SetParam(Math::Vector3 _startPos, Math::Vector3 _goalPos, Math::Vector3 _scale, float _speed, int _stayTime, Math::Vector3 _degAng)
{
	m_param.startPos	= _startPos;
	m_param.pos			= _startPos;
	m_param.scale		= _scale;
	m_param.stayTime	= _stayTime;
	m_param.speed		= _speed;

	Math::Matrix transMat	= Math::Matrix::CreateTranslation(m_param.pos);
	Math::Matrix scaleMat	= Math::Matrix::CreateScale(m_param.scale);

	m_mWorld = scaleMat * transMat;
}