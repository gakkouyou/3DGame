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
	void SetGoal() override { m_goalFlg = true; }
	// ゴールの座標をセット
	void SetGoalPos(const Math::Vector3& _goalPos) { m_goalProcess.targetPos = _goalPos; }
	const bool GetGoalProcessFinish() const { return m_goalProcess.moveEndFlg; }

	void SetPauseFlg(bool _pauseFlg)override;

	void SetFirstClearFlg(const bool _firstClearFlg);
	const bool IsFirstClearProcess() const { return m_firstClear & FirstClear::StartProgressMax; }
	const bool IsFirstClearEndProcess() const { return m_firstClear & FirstClear::EndProgressMax; }

private:
	// マップエディタモードの際に使用する座標
	Math::Vector3 m_debugPos	= Math::Vector3::Zero;
	// 移動ベクトル
	Math::Vector3 m_moveVec		= Math::Vector3::Zero;
	const float m_moveSpeed		= 0.15f;

	// キーフラグ
	bool m_shiftFlg				= false;

	// ゴールフラグ
	bool m_goalFlg				= false;

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

	enum FirstClear
	{
		NowStartProcess = 1 << 0,
		StartProgressMax = 1 << 1,
		NowEndProcess = 1 << 2,
		EndProgressMax = 1 << 3,
	};

	// 初めてステージをクリアしたときの処理
	void FirstClearProcess();
	UINT m_firstClear = 0;
	// スタートの行列
	Math::Matrix m_startMat;
	// ゴールの座標
	Math::Matrix m_goalMat;
	float m_progress = 0;
	const float m_addProgress = 0.01f;
	Math::Matrix m_targetMat;
};