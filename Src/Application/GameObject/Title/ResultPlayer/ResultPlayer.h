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

	// 朝のアニメーションを始める
	void MorningAnimationStart() { m_morningAnimationStart = true; }
	// 夕方のアニメーションを始める
	void EveningAnimationStart()	{ m_eveningAnimationStart = true; }

	// 朝のアニメーションが終了したかどうか
	const bool GetMorningAnimationEnd()	const { return m_animation & MorningAnimationEnd; }
	// ドアを開けていいか
	const bool GetOpenDoor()	const { return m_animation & OpenDoor; }
	// ドアを閉めていいか
	const bool GetCloseDoor()	const { return m_animation & CloseDoor; }
	// 夕方のアニメーションが終了したかどうか
	const bool GetEveningAnimationEnd()	const { return m_animation & EveningAnimationEnd; }

private:
	// モデル
	std::shared_ptr<KdModelWork> m_spModel = nullptr;

	// アニメーション
	std::shared_ptr<KdAnimator>	m_spAnimator = nullptr;

	// 動くスピード
	float m_moveSpeed = 0;
	// 回転角度
	float m_degAng = 0;
	
	// 拡縮
	float m_scale = 0;
	Math::Matrix m_scaleMat;

	// 初期座標
	Math::Vector3 m_startPos;
	// ゴールの座標
	Math::Vector3 m_goalPos;

	enum Animation
	{
		OpenDoor				= 1 << 0,
		CloseDoor				= 1 << 1,
		MorningAnimationEnd		= 1 << 2,
		EveningAnimationEnd		= 1 << 3,
	};

	UINT m_animation = 0;

	// アニメーションスタートOKになってから実際に動き始めるまでの時間とカウント
	int m_stayCount = 0;
	int m_stayTime = 0;

	// ドアを閉める座標
	float m_closeDoorPosZ = 0;
	// アニメーション終了の座標
	float m_animationEndPosZ = 0;

	std::weak_ptr<KdSoundInstance> m_wpSound;	// 歩く音
	bool m_soundFlg = false;						// 既に鳴っているかどうかのフラグ

	int m_runSmokeTime = 0;
	int m_smokeCount = 0;

	// 朝のアニメーションを開始するフラグ
	bool m_morningAnimationStart = false;

	// 夕方のアニメーションを開始するフラグ
	bool m_eveningAnimationStart = false;

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

	// 待機中
	class Stay : public StateBase
	{
	public:
		~Stay()	override {}

		void Update	(ResultPlayer& _owner)	override;
	};

	// １つ目のアニメーション
	class FirstAnimation : public StateBase
	{
	public:
		~FirstAnimation()	override {}

		void Update	(ResultPlayer& _owner)	override;
		void Exit	(ResultPlayer& _owner)	override;
	};
	
	// ２つ目のアニメーション
	class SecondAnimation : public StateBase
	{
	public:
		~SecondAnimation()	override {}

		void Update	(ResultPlayer& _owner)	override;
		void Exit	(ResultPlayer& _owner)	override;
	};

	// ３つ目のアニメーション
	class ThirdAnimation : public StateBase
	{
	public:
		~ThirdAnimation()	override {}

		void Update	(ResultPlayer& _owner)	override;
		void Exit	(ResultPlayer& _owner)	override;
	};

	// ４つ目のアニメーション
	class FourthAnimation : public StateBase
	{
	public:
		~FourthAnimation()	override {}

		void Update	(ResultPlayer& _owner)	override;
		void Exit	(ResultPlayer& _owner)	override;
	};
	void ChangeActionState(std::shared_ptr<StateBase> _nextState);
	std::shared_ptr<StateBase> m_nowAction = nullptr;
};