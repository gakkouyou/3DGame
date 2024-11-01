#pragma once
#include "../EnemyBase.h"

class NormalEnemy : public EnemyBase
{
public:
	NormalEnemy() {}
	~NormalEnemy()	override {}

	void Update()		override;
	void PostUpdate()	override;

	void Init()			override;

	// リセット処理
	void Reset()		override;

	void SetParam(const Param _param)	override;

	void OnHit()	override;

private:
	// 当たり判定
	void HitJudge();
	// 地面との当たり判定
	void HitGround();
	// 敵同士での当たり判定
	void HitEnemy();

	// 追尾していない時に動くための変数
	struct Move
	{
		Math::Vector3 goalPos = Math::Vector3::Zero;	// ゴール座標
		float degAng = 0;								// 角度
		bool rotFlg = false;							// 回転中かのフラグ
		const float minRotAng = 3.0f;					// 回転を制限する

		const float jumpPow = 0.05f;					// ジャンプ力
		const float movePow = 0.05f;					// 動く量
		const int	stayTime = 60;						// 待機時間
		int			stayCount = 0;						// 待機カウント
		bool		stayFlg = false;					// 待機フラグ
	};
	Move m_move;

	// 回転角度
	float m_degAng = 0;
	// 回転行列
	Math::Matrix m_rotMat	= Math::Matrix::Identity;

	// SetParamに入ったかどうかのフラグ
	bool m_setParamFlg = false;

	// ホーミング用構造体
	struct HomingStruct
	{
		const float viewingAngle	= 60.0f;// 視野角
		const float minRotDegAng	= 5.0f;	// 回転する際の制限
		const float speed			= 0.05f;// 移動量
		const float jumpPow			= 0.05f;// ジャンプ力
	};

	HomingStruct m_homing;

	struct LostTargetAnimation
	{
		const float maxLotAng	= 45.0f;	// 左右に見渡す？時の角度の上限
		const float lotAng		= 1.5f;		// 見渡すときに動く角度
		float sumLotAng			= 0;		// 動いた角度の合計
		const float stayTime	= 60;		// 各モーションごとの待機時間
		float stayCount			= 0;		// 待機時間のカウント
		bool stayFlg			= false;	// 待機フラグ
		float oldDegAng			= 0;		// 元の角度を保持するため
		enum Dir
		{
			Left,	// 左を向く
			Right,	// 右を向く
			Base,	// 元に戻る
			End,	// 終了
		};
		Dir dir					= Dir::Left;	// 方向
	};
	LostTargetAnimation m_lostTarget;

	// ジャンプ力
	const float m_findJumpPow	= 0.1f;



	// 当たったら一緒に動くような地形に当たった際の処理のための構造体
	struct HitMoveTerrain
	{
		Math::Matrix transMat = Math::Matrix::Identity;	// 動く前の行列
		Math::Matrix rotMat = Math::Matrix::Identity;	// 回転行列
		bool hitFlg = false;					// 当たったかどうか
	};

	// 動く床
	HitMoveTerrain m_moveGround;
	// 回る床
	HitMoveTerrain m_rotationGround;

	// 更新前の座標
	Math::Vector3 m_oldPos;

	// 死亡モーション用
	int m_deathCount = 0;
	const int m_deathTime = 45;

	// 地面にいるかどうか
	bool m_isGround = false;

	// ターゲットが視野角内にいるかどうかの判定
	bool TargetViewingAngleCheck();

	// ステートパターン
private:
	class StateBase
	{
	public:
		virtual ~StateBase() {}

		virtual void Enter	([[maybe_unused]] NormalEnemy& _owner)	{}
		virtual void Update	([[maybe_unused]] NormalEnemy& _owner)	{}
		virtual void Exit	([[maybe_unused]] NormalEnemy& _owner)	{}
	};

	// 待機中
	class Idle : public StateBase
	{
	public:
		~Idle()	override {}

		void Enter	(NormalEnemy& _owner)	override;
		void Update	(NormalEnemy& _owner)	override;
		void Exit	(NormalEnemy& _owner)	override;
	};
	
	// ジャンプ移動
	class JumpMove : public StateBase
	{
	public:
		~JumpMove()	override {}

		void Enter	(NormalEnemy& _owner)	override;
		void Update	(NormalEnemy& _owner)	override;
	};

	// 見つけた時の動き
	class Find : public StateBase
	{
	public:
		~Find()	override {}

		void Enter	(NormalEnemy& _owner)	override;
		void Update	(NormalEnemy& _owner)	override;
	};

	// 追尾
	class Homing : public StateBase
	{
	public:
		~Homing()	override {}

		void Update	(NormalEnemy& _owner)	override;
	};

	// 見失った時の動き
	class LostTarget : public StateBase
	{
	public:
		~LostTarget()	override {}

		void Enter	(NormalEnemy& _owner)	override;
		void Update	(NormalEnemy& _owner)	override;
		void Exit	(NormalEnemy& _owner)	override;
	};
	void ChangeActionState(std::shared_ptr<StateBase> _nextState);
	std::shared_ptr<StateBase> m_nowAction = nullptr;
};