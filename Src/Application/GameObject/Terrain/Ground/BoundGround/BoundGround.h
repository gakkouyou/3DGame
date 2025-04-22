#pragma once
#include "../../TerrainBase.h"

class BoundGround : public TerrainBase
{
public:
	BoundGround()				{}
	~BoundGround()	override	{}

	void Update()	override;
	void Init()		override;

	void OnHit()	override;

	void SetParam(const Param& _param) override;

private:
	bool m_onHitFlg = false;

	float m_scale = 0;						// 拡縮
	const float m_defaultAddScale = 0.3f;	// 拡縮の加算値の元
	float m_addScale = 0.3f;				// 拡縮の加算値
	const float m_addAddScale = 0.005f;		// 拡縮の加算値の加算値(大きくしていく値を大きくする)
	const float m_minScale = 0.2f;			// 上限の拡縮
	const float m_sumScale = 0.025f;		// 拡縮の減算値
	bool m_sumFlg = false;					// 大きくするかどうか
	int m_stayCount = 0;					// 待機カウント
	const int m_stayTime = 60;				// 待機時間
};