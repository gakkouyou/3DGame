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

	void SetGoalFlg(bool _goalFlg) { m_goalFlg = _goalFlg; }

	void SetGoalPos(Math::Vector3 _goalPos) { m_goalProcess.targetPos = _goalPos; }

	const bool GetGoalProcessFinish() const { return m_goalProcess.moveEndFlg; }

	void SetPauseFlg(bool _pauseFlg)override;

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
		float			moveFrame	= 0;					// 何フレームで終えるか
		float			frameCount	= 0;					// フレームのカウント
		bool			moveEndFlg	= false;				// 動き終わったかどうかのフラグ
		Math::Vector3	startPos	= Math::Vector3::Zero;	// 最初のローカル座標
		Math::Vector3	endPos		= Math::Vector3::Zero;	// 最終的なローカル座標
		Math::Vector3	nowPos		= Math::Vector3::Zero;	// 今のローカル座標
		Math::Vector3	moveVec		= Math::Vector3::Zero;	// 移動ベクトル
		float			speed		= 0;					// 動く速度
		Math::Vector3	startDegAng	= Math::Vector3::Zero;	// 最初の角度
		Math::Vector3	endDegAng	= Math::Vector3::Zero;	// 最終的な角度
		Math::Vector3	moveDegAng	= Math::Vector3::Zero;	// 動く角度
		Math::Vector3	nowDegAng	= Math::Vector3::Zero;	// 今の角度
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
	float m_pauseFocus = 0;
	// 普通のFocus処理
	float m_defaultFocus = 2000;

	UINT m_oldPlayerSituationType = 0;

	Math::Vector3 m_oldPos;

	bool m_airFlg = false;

	const int	m_moveTime	= 60;
	int			m_moveCount	= 0;
};