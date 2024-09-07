#pragma once
#include "../../TerrainBase.h"

class NormalGround : public TerrainBase
{
public:
	NormalGround()				{}
	~NormalGround()	override	{}
	
	//void Update()	override;
	void Init()		override;

	void SetParam(Param _param) override;
};