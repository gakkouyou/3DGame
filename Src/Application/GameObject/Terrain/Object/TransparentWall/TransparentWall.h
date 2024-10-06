#pragma once

#include "../../TerrainBase.h"

class TransparentWall : public TerrainBase
{
public:
	TransparentWall() {}
	~TransparentWall()	override {}

	void DrawLit()	override;
	void Init()		override;

	void SetParam(const Param& _param)	override;

private:

};