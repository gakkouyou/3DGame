#pragma once
#include "../../TerrainBase.h"

class BoundGround : public TerrainBase
{
public:
	BoundGround()				{}
	~BoundGround()	override	{}

	//void Update()	override;
	void Init()		override;

	void OnHit()	override;

	void SetParam(Param _param) override;

private:
};