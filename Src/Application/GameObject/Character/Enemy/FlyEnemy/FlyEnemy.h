#pragma once

#include "../EnemyBase.h"

class FlyEnemy : public EnemyBase
{
public:
	FlyEnemy() {}
	~FlyEnemy()	override {}

	void Update()						override;
	void Init()							override;

	Math::Vector3 GetPos() const override;

	void SetParam(const Param _param)	override;

	void OnHit()				override;

private:
	// sinカーブ用の角度
	float m_angle	= 0;

	// 死亡モーション用
	int m_deathCount = 0;
	const int m_deathTime = 45;
};