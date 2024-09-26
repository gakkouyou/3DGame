#pragma once
#include "../../TerrainBase.h"

class NormalGround : public TerrainBase
{
public:
	NormalGround()				{}
	~NormalGround()	override	{}
	
	void Init()		override;

	void SetParam(const Param _param) override;
};