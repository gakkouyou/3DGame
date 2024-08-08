#pragma once
#include "../../TerrainBase.h"

class NormalWall : public TerrainBase
{
public:
	NormalWall()				{}
	~NormalWall()	override	{}

	void Init()		override;
};