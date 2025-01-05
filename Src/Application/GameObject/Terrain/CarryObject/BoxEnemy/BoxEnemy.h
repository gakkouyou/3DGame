#pragma once

#include "../CarryObjectBase.h"

class BoxEnemy : public CarryObjectBase
{
public:	
	BoxEnemy()									{}
	~BoxEnemy()							override{}

	void Update()						override;
	void PostUpdate()					override;
	void GenerateDepthMapFromLight()	override;
	void DrawLit()						override;
	void Init()							override;

	void CarryFlg(bool _carryFlg)		override;

	// これ以上上に当たったら死亡判定
	Math::Vector3 GetPos() const override;

	void SetParam(const Param _param)	override;

	void OnHit()						override;
	
	// 運べるか運べないかを返す
	const bool IsCarryCheck() const		override { return !m_enemyFlg; }

	void Reset();

	// ゴールした時の処理
	void SetGoal() override { m_isExpired = true; }

private:
	// 当たり判定
	void HitJudge();

	// 角度
	float m_degAng = 0;
	float m_baseDegAng = 0;

	enum Dir
	{
		RightFront,
		RightBack,
		LeftFront,
		LeftBack,
		Up,
		Down,
		Max
	};
	Math::Vector3 m_edgeBasePos[Max] = {};
	Math::Vector3 m_edgePos[Max] = {};

	Math::Vector3 m_centerPos;

	// 下に落ちたら、リスポーンさせる
	float m_underLine = 0;

	// レイを少し上から出す処理をやめるフラグ
	bool m_rayDownFlg = false;

	Math::Vector3 m_oldPos;

	// 箱から敵に戻るまでの時間
	int m_enemyTime = 0;
	// ガタガタし始めるまでの時間
	int m_shakeTime = 0;
	// カウント
	int m_enemyCount = 0;
	// ガタガタの角度制限
	int m_maxDegAng	= 0;
	// ガタガタの行列
	Math::Matrix m_shakeMat;

	// ↓敵の状態の時用
	// モデル
	std::shared_ptr<KdModelWork> m_spEnemyModel = nullptr;

	// 敵状態か箱状態かのフラグ
	bool m_enemyFlg = false;

	// 移動系
	float	m_jumpPow	= 0;	// ジャンプ力
	float	m_moveSpeed	= 0;	// 移動速度
	int		m_stayTime	= 0;	// ジャンプの待機時間
	int		m_stayCount = 0;	// ジャンプの待機のカウント
	bool	m_isGround	= false;// 地面にいるかどうか
	int		m_jumpChargeCount	= 0;	// ジャンプの前兆用のカウント
	int		m_jumpChargeTime	= 0;	// ジャンプの前兆用の時間


	// 追尾していいかのフラグ
	bool m_homingFlg = false;

	enum LandSoundType
	{
		Grass,
		Tile,
		Bound,
		MaxNum,
	};

	// 歩く音の配列
	std::weak_ptr<KdSoundInstance3D> m_wpLandSound[LandSoundType::MaxNum];
	// ジャンプ音
	std::weak_ptr<KdSoundInstance3D> m_wpJumpSound;
	// 復活音
	std::weak_ptr<KdSoundInstance3D> m_wpRespawnSound;

	bool m_setParamFlg = false;

	// 箱→敵になる、プレイヤーとの距離
	float m_enemyChangeLength = 0;

	// OnHitに入ったかどうかのフラグ
	bool m_onHitFlg = false;

	// 重力の処理をするかどうか
	bool m_isCarry = false;

	// アニメーション
	std::shared_ptr<KdAnimator>		m_spAnimator = nullptr;

	// JSONファイルのパス
	std::string_view m_path = "Asset/Data/Json/Terrain/CarryObject/BoxEnemy/BoxEnemy.json";

	// JSONのデータをロードする
	void DataLoad();

// ステートパターン
private:
	class StateBase
	{
	public:
		virtual ~StateBase() {}

		virtual void Enter	(BoxEnemy&) {}
		virtual void Update	(BoxEnemy&) {}
		virtual void Exit	(BoxEnemy&) {}
	};

	// 待機中
	class Idle : public StateBase
	{
	public:
		~Idle()	override {}

		void Enter	(BoxEnemy& _owner)	override;
		void Update	(BoxEnemy& _owner)	override;
	};

	// ジャンプ待機中
	class JumpStay : public StateBase
	{
	public:
		~JumpStay()	override {}

		void Update	(BoxEnemy& _owner)	override;
		void Exit	(BoxEnemy& _owner)	override;
	};

	// ジャンプ移動
	class JumpMove : public StateBase
	{
	public:
		~JumpMove()	override {}

		void Enter	(BoxEnemy& _owner)	override;
		void Update	(BoxEnemy& _owner)	override;
	};

	// 箱の時
	class Box : public StateBase
	{
	public:
		~Box()	override {}

		void Update	(BoxEnemy& _owner)	override;
	};

	// 運ばれている時
	class Carry : public StateBase
	{
	public:
		~Carry()	override {}

		void Enter	(BoxEnemy& _owner)	override;
		void Update	(BoxEnemy& _owner)	override;
	};

	// 震える状態
	class Shake : public StateBase
	{
	public:
		~Shake()	override {}

		void Enter	(BoxEnemy& _owner)	override;
		void Update	(BoxEnemy& _owner)	override;
		void Exit	(BoxEnemy& _owner)	override;
	};

	// 敵に戻った時のただのジャンプ
	class Jump : public StateBase
	{
	public:
		~Jump()	override {}

		void Enter	(BoxEnemy& _owner)	override;
		void Update	(BoxEnemy& _owner)	override;
	};

	void ChangeActionState(std::shared_ptr<StateBase> _nextState);
	std::shared_ptr<StateBase> m_nowAction = nullptr;
};