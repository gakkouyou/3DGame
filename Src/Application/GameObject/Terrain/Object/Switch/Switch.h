#pragma once

#include "../../TerrainBase.h"

class TerrainController;
class SwitchBaseModel;

class Switch : public TerrainBase
{
public:
	Switch() {}
	~Switch()							override {}

	void Update()						override;
	void Init()							override;

	void OnHit()						override;

	void SetParam(const Param& _param)			override;

	const std::weak_ptr<SwitchBaseModel>& GetSwitchBaseModel() const { return m_wpBase; }

	bool IsCameraTracking() override { return false; }

private:
	// 上がり下がりする量
	const float m_movePow = 0.01f;
	// 下がる限界値
	const float m_maxMove = 0.15f;

	// OnHitに入ったかどうかをチェックするフラグ
	bool m_onHitFlg = false;

	// スイッチの土台？
	std::weak_ptr<SwitchBaseModel> m_wpBase;
};