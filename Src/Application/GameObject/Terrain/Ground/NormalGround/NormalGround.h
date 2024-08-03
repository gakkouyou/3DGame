#pragma once
#include "../../BaseTerrain.h"

class NormalGround : public BaseTerrain
{
public:
	NormalGround()				{}
	~NormalGround()	override	{}

	void Init()		override;
};