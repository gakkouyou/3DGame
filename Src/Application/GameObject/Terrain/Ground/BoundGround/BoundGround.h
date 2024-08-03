#pragma once
#include "../../BaseTerrain.h"

class BaseCharacter;

class BoundGround : public BaseTerrain
{
public:
	BoundGround()				{}
	~BoundGround()	override	{}

	void Init()		override;

	void OnHit()	override;

private:
};