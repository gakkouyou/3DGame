#pragma once

class ResultPlayer : public KdGameObject
{
public:
	ResultPlayer() {}
	~ResultPlayer()	override {}

	void Update()	override;
	void GenerateDepthMapFromLight()	override;
	void DrawLit()	override;
	void Init()		override;

	// 一つめのアニメーションを始める
	void SetFirstAnimationStart()	{ m_animation |= FirstAnimationStart; }
	// 二つ目のアニメーションを始める
	void SetSecondAnimationStart()	{ m_animation |= SecondAnimationStart; }

	// 一つ目のアニメーションが終了したかどうか
	const bool GetFirstAnimationEnd()	const { return m_animation & FirstAnimationEnd; }
	// ドアを開けていいか
	const bool GetOpenDoor()	const { return m_animation & OpenDoor; }
	// ドアを閉めていいか
	const bool GetCloseDoor()	const { return m_animation & CloseDoor; }
	// アニメーションが完全に終了したかどうか
	const bool GetAnimationEnd()	const { return m_animation & ThirdAnimationEnd; }

private:
	// モデル
	std::shared_ptr<KdModelWork> m_spModel = nullptr;

	// アニメーション
	std::shared_ptr<KdAnimator>	m_spAnimator = nullptr;

	// 動くスピード
	const float m_speed = 0.1f;
	// 動く方向
	const Math::Vector3 m_moveVec = { 1, 0, 0 };
	// 回転角度
	float m_degAng = 0;

	// 初期座標
	const Math::Vector3 m_startPos = { -1.5, 0, -10 };
	// ゴールの座標
	const Math::Vector3 m_goalPos = { 15, 0, -10 };

	enum Animation
	{
		FirstAnimationStart		= 1 << 0,
		FirstAnimationEnd		= 1 << 1,
		SecondAnimationStart	= 1 << 2,
		OpenDoor				= 1 << 3,
		CloseDoor				= 1 << 4,
		SecondAnimationEnd		= 1 << 5,
		ThirdAnimationStart		= 1 << 6,
		ThirdAnimationEnd		= 1 << 7,
	};

	UINT m_animation = 0;

	// アニメーションスタートOKになってから実際に動き始めるまでの時間とカウント
	int m_stayCount = 0;
	const int m_stayTime = 60;

	// ３つ目のアニメーションがスタートしてから実際に動き始めるまでの時間とカウント
	const int m_thirdAnimationStayTime = 120;

	// ドアを閉める座標
	const float m_closeDoorPosZ = -6.5;
	// アニメーション狩猟の座標
	const float m_endPosZ = 0;

	// 一つ目のアニメーション
	void FirstAnimation();
	// 二つ目のアニメーション
	void SecondAnimation();
	// 三つ目のアニメーション
	void ThirdAnimation();
};