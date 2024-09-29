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

	// ターゲットを追尾する
	void TargetHoming();

	// 今の状況
	enum SituationType
	{
		Stop	= 1 << 0,	// 止まっている
		Run		= 1 << 1,	// 走っている
		Jump	= 1 << 2,	// ジャンプしている
		Air		= 1 << 3,	// 空中にいる
		WallHit = 1 << 4,	// 壁に触れている
		Homing	= 1 << 5,	// 追尾している
		Find	= 1 << 6,	// 発見している
		LostTarget	= 1 << 7,	// 見失う
	};

	UINT m_situationType	= SituationType::Stop;

	// 追尾していない時に動くための変数
	struct Move
	{
		Math::Vector3 goalPos = Math::Vector3::Zero;	// ゴール座標
		float degAng = 0;								// 角度
		bool confirmedAngFlg = false;					// 角度を決定させるためのフラグ
		bool rotFlg = false;							// 回転中かのフラグ
		const float minRotAng = 3.0f;					// 回転を制限する
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
		const float viewingAngle	= 60.0f;
		const float rotDegAng		= 5.0f;
		const float speed			= 0.05f;
	};

	HomingStruct m_homing;


	// ターゲットを見つけた時の処理
	void FindTarget();

	// ターゲットを見失った時の処理(左右に見渡す→元の角度に戻る→戻る)
	void LostTargetProcess();
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

	// 歩くモーション用
	struct Walk
	{
		const float jumpPow		= 0.05f;		// ジャンプ力
		const float movePow		= 0.05f;		// 動く量
		const int	stayTime	= 60;		// 待機時間
		int			stayCount	= 0;		// 待機カウント
		bool		stayFlg		= false;	// 待機フラグ
	};

	Walk m_walkMotion;



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

	int cnt = 0;
};