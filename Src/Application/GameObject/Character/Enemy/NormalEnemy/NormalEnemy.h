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

	// 当たり判定
	void HitJudge();

	// ターゲットを追尾する
	void TargetHoming();

	void SetParam(Param _param)	override;

	void OnHit()	override;

private:
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

	// ホーミング状態のスピード
	const float m_homingSpeed	= 0.2f;

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
	const float m_jumpPow	= 0.4f;
};