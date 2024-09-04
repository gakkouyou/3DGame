#pragma once

#include "../EnemyBase.h"

class FlyEnemy : public EnemyBase
{
public:
	FlyEnemy() {}
	~FlyEnemy()	override {}

	void Update()						override;
	void PostUpdate()					override;
	void Init()							override;

	void SetParam(Param _param)	override;

	void OnHit()				override;

private:
	// sinカーブ用の角度
	float m_angle	= 0;
};