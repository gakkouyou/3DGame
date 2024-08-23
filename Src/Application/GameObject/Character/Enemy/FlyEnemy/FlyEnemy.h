#pragma once

#include "../EnemyBase.h"

class FlyEnemy : public EnemyBase
{
public:
	FlyEnemy() {}
	~FlyEnemy()	override {}

	void Update()		override;
	void PostUpdate()	override;
	void Init()			override;

private:

};