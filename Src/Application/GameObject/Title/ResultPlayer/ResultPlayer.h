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
	void MorningAnimationStart() { ChangeActionState(std::make_shared<FirstAnimation>()); }
	// 二つ目のアニメーションを始める
	void EveningAnimationStart()	{ m_animation |= ThirdAnimationStart; }

	// 一つ目のアニメーションが終了したかどうか
	const bool GetSecondAnimationEnd()	const { return m_animation & SecondAnimationEnd; }
	// ドアを開けていいか
	const bool GetOpenDoor()	const { return m_animation & OpenDoor; }
	// ドアを閉めていいか
	const bool GetCloseDoor()	const { return m_animation & CloseDoor; }
	// アニメーションが完全に終了したかどうか
	const bool GetAnimationEnd()	const { return m_animation & FourthAnimationEnd; }

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
	const Math::Vector3 m_startPos = { -1.5, 0, -2 };
	// ゴールの座標
	const Math::Vector3 m_goalPos = { 16, 0, -10 };

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
		FourthAnimationStart	= 1 << 8,
		FourthAnimationEnd		= 1 << 9,
	};

	UINT m_animation = 0;

	// アニメーションスタートOKになってから実際に動き始めるまでの時間とカウント
	int m_stayCount = 0;
	const int m_stayTime = 60;

	// ３つ目のアニメーションがスタートしてから実際に動き始めるまでの時間とカウント
	const int m_thirdAnimationStayTime = 120;

	// ドアを閉める座標
	const float m_closeDoorPosZ = -6.5;
	// アニメーション終了の座標
	const float m_endPosZ = -2;

	// 一つ目のアニメーション
	//void FirstAnimation();
	// 二つ目のアニメーション
	void SecondAnimation();
	// 三つ目のアニメーション
	void ThirdAnimation();
	// 四つ目のアニメーション
	void FourthAnimation();

	std::weak_ptr<KdSoundInstance> m_wpSound;	// 歩く音
	bool m_soundFlg = false;						// 既に鳴っているかどうかのフラグ

	int m_runSmokeTime = 16;
	int m_smokeCount = 0;

	// JSONファイルのパス
	std::string_view m_path = "Asset/Data/Json/Title/ResultPlayer/ResultPlayer.json";

	// JSONのデータをロードする
	void DataLoad();

// ステートパターン
private:
	class StateBase
	{
	public:
		virtual ~StateBase() {}

		virtual void Enter	(ResultPlayer&)	{}
		virtual void Update	(ResultPlayer&)	{}
		virtual void Exit	(ResultPlayer&)	{}
	};

	// １つ目のアニメーション
	class FirstAnimation : public StateBase
	{
	public:
		~FirstAnimation()	override {}

		void Enter	(ResultPlayer& _owner)	override;
		void Update	(ResultPlayer& _owner)	override;
		void Exit	(ResultPlayer& _owner)	override;
	};
	
	// ２つ目のアニメーション
	class SecondAnimation : public StateBase
	{
	public:
		~SecondAnimation()	override {}

		void Enter	(ResultPlayer& _owner)	override;
		void Update	(ResultPlayer& _owner)	override;
	};

	// ３つ目のアニメーション
	class ThirdAnimation : public StateBase
	{
	public:
		~ThirdAnimation()	override {}

		void Enter	(ResultPlayer& _owner)	override;
		void Update	(ResultPlayer& _owner)	override;
	};

	// ４つ目のアニメーション
	class FourthAnimation : public StateBase
	{
	public:
		~FourthAnimation()	override {}

		void Update	(ResultPlayer& _owner)	override;
	};
	void ChangeActionState(std::shared_ptr<StateBase> _nextState);
	std::shared_ptr<StateBase> m_nowAction = nullptr;
};