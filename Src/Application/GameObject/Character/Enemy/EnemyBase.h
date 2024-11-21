#pragma once
#include "../CharacterBase.h"

class Player;

class EnemyBase : public CharacterBase
{
public:
	EnemyBase()							{}
	~EnemyBase()				override{}

	virtual void Update()		override {}
	virtual void PostUpdate()	override {}

	virtual void Init()			override;

	// リセット処理
	virtual void Reset()		override {}

	// ターゲットをセットする
	void SetTarget(const std::shared_ptr<Player>& _spTarget) { m_wpTarget = _spTarget; }

	// これ以上上で当たると倒せる座標
	Math::Vector3 GetPos() const override = 0;

	// 情報
	struct Param
	{
		Math::Vector3 startPos	= Math::Vector3::Zero;	// 最初の座標
		float moveArea			= 0;					// 行動範囲
		float searchArea		= 0;					// 索敵範囲
		float rotDegAng			= 0;					// 回転角度
	};

	// パラメータをセットする
	virtual void SetParam(Param _param) { m_param = _param; }
	// パラメータをゲットする
	virtual const Param GetParam()const { return m_param; }

	// ゴールした時の処理
	void SetGoal() override { m_isExpired = true; }

protected:
	// 死んだときの処理
	virtual void DeathProcess() {}

	// ターゲット
	std::weak_ptr<Player> m_wpTarget;

	// 情報
	Param m_param;
};