#pragma once

#include "../../TerrainBase.h"

class Propeller : public TerrainBase
{
public:
	Propeller() {}
	~Propeller()	override {}

	void Update()	override;
	void Init()		override;

private:

};