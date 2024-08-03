#pragma once
#include "../../BaseTerrain.h"

class NormalWall : public BaseTerrain
{
public:
	NormalWall()				{}
	~NormalWall()	override	{}

	void Init()		override;
};