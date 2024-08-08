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

	// 当たり判定
	void HitJudge();

	// ターゲットを追尾する
	void TargetHoming();

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
	};

	UINT m_situationType;

	// 追尾していない時に動くための変数
	Math::Vector3 m_goalPos = Math::Vector3::Zero;	// ゴール座標
	float m_degAng			= 0;					// 角度
	bool m_stayFlg			= false;				// 待機中かのフラグ
	float m_stayCnt			= 0;					// 待機カウント
	const float m_stayTime = 60;					// 待機時間
};