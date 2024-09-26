#pragma once

#include "../../../TerrainBase.h"

class SwitchBaseModel : public TerrainBase
{
public:
	SwitchBaseModel() {}
	~SwitchBaseModel()	override {}

	void Init()	override;

	void SetParam(const Param _param)	override;

	void SetSwitch(const std::weak_ptr<TerrainBase> _wpSwitch) { m_wpSwitch = _wpSwitch; }

private:
	std::weak_ptr<TerrainBase> m_wpSwitch;
};