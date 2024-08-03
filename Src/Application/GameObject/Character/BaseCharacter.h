﻿#pragma once

class BaseCharacter : public KdGameObject
{
public:
	BaseCharacter()											{}
	virtual ~BaseCharacter()					override	{}

	// 初期化
	virtual void Init()							override {}

	// 更新
	virtual void Update()						override {}
	virtual void PostUpdate()					override {}

	// 描画
	virtual void GenerateDepthMapFromLight()	override {}
	virtual void DrawLit()						override {}

	// 生存フラグ
	const bool GetAlive() const { return m_aliveFlg; }



	// 座標セット
	void SetPos(const Math::Vector3 _pos) { m_pos = _pos; }
	void SetPos(const float _x, const float _y, const float _z) { m_pos = { _x, _y, _z }; }

	// 重力をセットする
	void SetGravity(const float _gravity) { m_gravity = _gravity; }

protected:
	// レイ判定　当たったらtrueを返す
	bool RayHitJudge(const Math::Vector3 _startPos, Math::Vector3& _hitPos, const Math::Vector3 _dir, const float _range, const KdCollider::Type _type, const bool _debugFlg = false);

	// スフィア判定　当たったらtrueを返す
	bool SphereHitJudge(const Math::Vector3 _centerPos, const float _radius, const KdCollider::Type _type, Math::Vector3& _hitDir, float& _maxOverLap, const bool _debugFlg = false);

	// モデル
	std::shared_ptr<KdModelWork>	m_spModel		= nullptr;
	// アニメーション
	std::shared_ptr<KdAnimator>		m_spAnimator	= nullptr;

	// 生存フラグ
	bool m_aliveFlg									= true;

	// 座標
	Math::Vector3	m_pos							= Math::Vector3::Zero;
	// 移動
	Math::Vector3	m_moveVec						= Math::Vector3::Zero;
	// 移動量
	float			m_moveSpeed						= 0;

	// 重力
	float			m_gravity						= 0;
	float			m_gravityPow					= 0.02f;
	const float		m_maxGravity					= 4.0f;

	// 当たり判定の段差許容範囲
	const float		m_enableStepHeight				= 0.1f;

	// 当たったオブジェクトを保持
	std::weak_ptr<KdGameObject> m_wpHitObject;
};