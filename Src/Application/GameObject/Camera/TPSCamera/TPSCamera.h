#pragma once
#include "../CameraBase.h"

class TPSCamera : public CameraBase
{
public:
	TPSCamera()							{}
	~TPSCamera()			override	{}

	void PostUpdate()		override;
	void Init()				override;

	void Reset()			override;

	// ゴールした時の処理
	void SetGoal() override { ChangeActionState(std::make_shared<Goal>()); }
	// ゴールの座標をセット
	void SetGoalPos(const Math::Vector3& _goalPos) { m_goalProcess.targetPos = _goalPos; }
	const bool GetGoalProcessFinish() const { return m_goalProcess.moveEndFlg; }

	void SetPauseFlg(bool _pauseFlg)override;
	
	// 最終ゴールの演出開始
	void SetFinalGoalProcess()	{ ChangeActionState(std::make_shared<FinalGoal>()); }
	// 新たな道の演出開始
	void SetSlopeProcess()		{ ChangeActionState(std::make_shared<NewRoad>()); }
	// 元に戻る
	void SetBackProcess()		{ ChangeActionState(std::make_shared<Back>()); }
	// 最終ゴールに到着したかどうか
	const bool GetFinalGoalProcessEnd()		const { return m_firstClear == FirstClear::FinalGoalProcessEnd; }
	// 坂に到着したかどうか
	const bool GetSlopeProcessEnd()			const { return m_firstClear == FirstClear::SlopeProcessEnd; }
	// 元に戻ったかどうか
	const bool GetFirstClearProcessEnd()	const { return m_firstClear == FirstClear::FirstClearEnd; }

private:
	// マップエディタモードの際に使用する座標
	Math::Vector3 m_debugPos	= Math::Vector3::Zero;
	// 移動ベクトル
	Math::Vector3 m_moveVec		= Math::Vector3::Zero;
	const float m_moveSpeed		= 0.15f;

	// キーフラグ
	bool m_shiftFlg				= false;

	// 視点移動する際に使用する構造体
	struct Move
	{
		bool			moveEndFlg	= false;				// 動き終わったかどうかのフラグ
		Math::Vector3	startPos	= Math::Vector3::Zero;	// 最初のローカル座標
		Math::Vector3	endPos		= Math::Vector3::Zero;	// 最終的なローカル座標
		float			speed		= 0;					// 動く速度
		float			progress	= 0;					// 進行度
		Math::Vector3	startDegAng	= Math::Vector3::Zero;	// 最初の角度
		Math::Vector3	endDegAng	= Math::Vector3::Zero;	// 最終的な角度
		float			stayFrame	= 0;					// 動き終わりから次の処理へ移行する際の待機フレーム数
		float			stayCount	= 0;					// 待機カウント
		bool			endFlg		= false;				// 待機が終了したかどうかのフラグ
		Math::Vector3	targetPos	= Math::Vector3::Zero;	// ターゲットの座標
	};
	// ゴールの処理の時用
	Move m_goalProcess;
	// 最終ゴールをターゲットとした時の視点移動
	Move m_finalGoalProcess;
	// 新しい道をターゲットとした時の視点移動
	Move m_newRoadProcess;

	void GoalProcess();

	// ポーズ画面かどうか
	bool m_pauseFlg = false;
	bool m_oldPauseFlg = false;
	// ポーズ画面の時のFocus距離？
	const float m_pauseFocus = 0;
	// 普通のFocus処理
	const float m_defaultFocus = 2000;

	// プレイヤーの最後に地面にいた時の座標
	Math::Vector3 m_oldPlayerPos;

	const int	m_moveTime	= 60;
	int			m_moveCount	= 0;

	const float m_trackingLerp	= 0.1f;	// 追尾時の線形補間の数値
	const float m_trackingUp	= 2.0f;	// これ以上上に上がったら追尾
	const float m_trackingDown	= 0.0f;	// これ以上下に下がったら追尾

	enum class FirstClear
	{
		FinalGoalProcessEnd,
		SlopeProcessEnd,
		FirstClearEnd,
		None,
	};

	// 初めてステージをクリアしたときの処理
	void FirstClearProcess();
	FirstClear m_firstClear = FirstClear::None;
	// スタートの行列
	Math::Matrix m_startMat;
	// ゴールの座標
	Math::Matrix m_goalMat;
	// スタートのクォータニオン
	Math::Quaternion m_startQua;
	// ゴールのクォータニオン
	Math::Quaternion m_goalQua;
	// ゴールのローカル行列
	Math::Matrix m_localGoalMat;
	// 最終ゴールの演出の際のローカル行列
	Math::Matrix m_finalGoalMat;
	float m_progress = 0;
	const float m_addProgress = 0.01f;
	Math::Matrix m_targetMat;

// ステートパターン
private:
	class StateBase
	{
	public:
		virtual ~StateBase() {}

		virtual void Enter	(TPSCamera&) {}
		virtual void Update	(TPSCamera&) {}
		virtual void Exit	(TPSCamera&) {}
	};

	// 追尾中
	class Tracking : public StateBase
	{
	public:
		~Tracking()	override {}

		void Enter	(TPSCamera& _owner)	override;
		void Update	(TPSCamera& _owner)	override;
	};

	// ゴールした時
	class Goal : public StateBase
	{
	public:
		~Goal()	override {}

		void Update	(TPSCamera& _owner)	override;
		void Exit	(TPSCamera& _owner)	override;
	};

	// 最終ゴールを見る
	class FinalGoal : public StateBase
	{
	public:
		~FinalGoal()	override {}

		void Enter	(TPSCamera& _owner)	override;
		void Update	(TPSCamera& _owner)	override;
	};

	// 新たな道を見る
	class NewRoad : public StateBase
	{
	public:
		~NewRoad()	override {}

		void Enter	(TPSCamera& _owner)	override;
		void Update	(TPSCamera& _owner)	override;
	};

	// 元に戻る
	class Back : public StateBase
	{
	public:
		~Back()	override {}

		void Enter	(TPSCamera& _owner)	override;
		void Update	(TPSCamera& _owner)	override;
	};

	// ポーズ
	class Pause : public StateBase
	{
	public:
		~Pause()	override {}

		void Enter	(TPSCamera& _owner)	override;
		void Update	(TPSCamera& _owner)	override;
		void Exit	(TPSCamera& _owner)	override;
	};

	// デバッグ
	class Debug : public StateBase
	{
	public:
		~Debug()	override {}

		void Enter	(TPSCamera& _owner)	override;
		void Update	(TPSCamera& _owner)	override;
		void Exit	(TPSCamera& _owner) override;
	};

	void ChangeActionState(std::shared_ptr<StateBase> _nextState);
	std::shared_ptr<StateBase> m_nowAction = nullptr;
};