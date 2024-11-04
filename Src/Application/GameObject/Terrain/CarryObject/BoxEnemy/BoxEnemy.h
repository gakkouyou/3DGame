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

private:
	// 当たり判定
	void HitJudge();

	// 角度
	float m_degAng = 180;

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
	float m_underLine = -30.0f;

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
	const int m_maxDegAng = 10;

	// 生存フラグ
	bool m_aliveFlg = false;

	// ↓敵の状態の時用
	// モデル
	std::shared_ptr<KdModelData> m_spEnemyModel = nullptr;

	// 敵状態か箱状態かのフラグ
	bool m_enemyFlg = false;

	// 移動系
	float		m_jumpPow	= 0;	// ジャンプ力
	float		m_moveSpeed	= 0;	// 移動速度
	const int	m_stayTime	= 30;	// ジャンプの待機時間
	int			m_stayCount = 0;	// ジャンプの待機のカウント
	bool		m_airFlg	= false;// 空中にいるかどうか

	// 敵の時の更新関数
	void EnemyUpdate();

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

	// 地面に付いた瞬間かどうかを判断するフラグ
	bool m_landFlg = true;

	bool m_setParamFlg = false;

	// 箱→敵になる、プレイヤーとの距離
	float m_enemyChangeLength = 0;

	// JSONファイルのパス
	std::string_view m_path = "Asset/Data/BoxEnemy.json";

	std::string m_name = "BoxEnemy";
	// JSONのデータをロードする
	void DataLoad();
	// JSONのデータをセーブする
	void DataSave();
};