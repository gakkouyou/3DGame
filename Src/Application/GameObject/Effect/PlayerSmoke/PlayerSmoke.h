﻿#pragma once

class PlayerSmoke : public KdGameObject
{
public:
	PlayerSmoke() {}
	~PlayerSmoke()	override {}

	void Update()	override;
	void DrawLit()	override;
	void Init()		override;

	void SetPauseFlg(const bool _pauseFlg)	override { m_pauseFlg = _pauseFlg; }

	void SetPos(const Math::Vector3& _basePos) override { m_basePos = _basePos; }

	// タイプ
	enum class SmokeType
	{
		WalkSmoke,	// 歩くときなどの煙
		StampSmoke,	// 着地したときなどの煙
		DeathSmoke,	// 死んだときの煙
	};

	void SetSmokeType(SmokeType _smokeType);

private:

	void WalkSmokeUpdate();
	void StampSmokeUpdate();
	void DeathSmokeUpdate();

	// モデル
	std::shared_ptr<KdModelWork> m_spModel = nullptr;

	// 拡縮
	float m_scale = 0;

	// ポーズ画面かどうか
	bool m_pauseFlg = false;

	// 煙のタイプ
	SmokeType m_smokeType;

	// 煙を出す個数
	int m_smokeNum = 0;

	// 指定された座標
	Math::Vector3 m_basePos;

	// 煙の座標
	std::list<Math::Vector3> m_posList;

	struct WalkSmoke
	{
		const float subScale = 0.02f;	// 縮小の量
		const int	smokeNum = 1;		// 煙の数
	};
	WalkSmoke m_walkSmoke;

	struct StampSmoke
	{
		const float subScale	= 0.04f;	// 縮小の量
		const int	smokeNum	= 8;		// 煙の数
		const int	subTime		= 10;		// 縮小するまでの時間
		int			subCount	= 0;		// 縮小するまでのカウント
		Math::Vector3 moveVec = { 0, 0, 1 };	// 角度0のやつがどの方向に動くか
		const float movePow		= 1.0f;
	};
	StampSmoke m_stampSmoke;

	struct DeathSmoke
	{
		const float subScale = 0.04f;	// 縮小の量
		const int	smokeNum = 4;		// 煙の数
		const int	subTime = 10;		// 縮小するまでの時間
		int			subCount = 0;		// 縮小するまでのカウント
		Math::Vector3 moveVec = { 0, 0, 1 };	// 角度0のやつがどの方向に動くか
		const float movePow = 1.0f;
	};
	DeathSmoke m_deathSmoke;
};