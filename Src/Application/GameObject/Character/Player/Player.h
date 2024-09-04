#pragma once
#include "../CharacterBase.h"

class CameraBase;
class TerrainController;

class Player : public CharacterBase
{
public:
	Player()					{}
	~Player()			override{}

	// 更新
	void Update()		override;
	void PostUpdate()	override;

	// 初期化
	void Init()			override;

	// リセット処理
	void Reset()		override;

	// カメラセット
	void SetCamera(const std::shared_ptr<CameraBase>& _spCamera) { m_wpCamera = _spCamera; }

	// クリアしたかどうか
	void SetGoalFlg(bool _goalFlg) { m_goalFlg = _goalFlg; }

	// 動いていいかのフラグのセット
	void SetStopFlg(bool _stopFlg) { m_stopFlg = _stopFlg; }

	// ゴールの座標
	void SetGoalPos(Math::Vector3 _goalPos) { m_goalPos = _goalPos; }

private:
	// 当たり判定
	void HitJudge();
	// 地面との当たり判定
	void HitJudgeGround();
	// 当たったらイベントが発生する系の当たり判定
	void HitJudgeEvent();
	// 敵との当たり判定
	void HitJudgeEnemy();

	// オブジェクトに当たった時の反応
	//void Bound();
	

	// 今の状況
	enum SituationType
	{
		Stop	= 1 << 0,	// 止まっている
		Walk	= 1 << 1,	// 歩いている
		Jump	= 1 << 2,	// ジャンプしている
		Air		= 1 << 3,	// 空中にいる
		WallHit	= 1 << 4,	// 壁に触れている
		Run		= 1 << 5,	// 走っている
	};

	// カメラのウィークポインタ
	std::weak_ptr<CameraBase> m_wpCamera;

	// 今の状況
	UINT m_situationType	= Stop;

	// ジャンプ力
	float m_jumpPow			= 0;

	// 角度
	float m_angle			= 0.0f;

	// 回転行列
	Math::Matrix m_rotMat	= Math::Matrix::Identity;

	// 当たったら一緒に動くような地形に当たった際の処理のための構造体
	struct HitMoveTerrain
	{
		Math::Matrix transMat	= Math::Matrix::Identity;	// 動く前の行列
		Math::Matrix rotMat		= Math::Matrix::Identity;	// 回転行列
		bool hitFlg				= false;					// 当たったかどうか
	};
	
	// 動く床
	HitMoveTerrain m_moveGround;
	// 回る床
	HitMoveTerrain m_rotationGround;

	// アニメーション用
	bool m_walkAnimeDirFlg = false;

	// エフェクシア用フラグ
	bool m_effectFlg = false;

	// ゴールのフラグ
	bool m_goalFlg	= false;

	// ゴールした時の処理
	void GoalProcess();

	// 止めるフラグ
	bool m_stopFlg = false;

	// ゴールの処理
	int m_goalStayCnt	= 0;
	int m_goalStayTime	= 60;
	Math::Vector3 m_goalPos = Math::Vector3::Zero;

	// 足元の煙用
	int m_smokeCount	= 0;
	int m_walkSmokeTime = 15;
	int m_runSmokeTime	= 7;

	// 走り状態の時のスピード
	const float m_runSpeed	= 0.5f;

	// 音
	struct Sound
	{
		std::weak_ptr<KdSoundInstance> wpSound;	// 音
		bool flg = false;						// 既に鳴っているかどうかのフラグ
	};

	enum WalkSound
	{
		Grass,

		Max,
	};

	std::weak_ptr<KdSoundInstance> m_wpWalkSound[WalkSound::Max];
	bool m_walkSoundFlg = false;
};