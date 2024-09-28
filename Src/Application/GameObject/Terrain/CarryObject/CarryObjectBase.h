#pragma once

class Player;
class TerrainController;
class TerrainBase;

class CarryObjectBase : public KdGameObject
{
public:
	CarryObjectBase() {}
	virtual ~CarryObjectBase()					override {}

	virtual void Update()						override {}
	virtual void PostUpdate()					override {}
	virtual void GenerateDepthMapFromLight()	override;
	virtual void DrawLit()						override;
	virtual void Init()							override {}

	// プレイヤーのセット
	void SetPlayer(const std::shared_ptr<Player>& _spPlayer) { m_wpPlayer = _spPlayer; }

	// 運ぶ運ばれていないを切り替える
	virtual void CarryFlg(const bool _carryFlg) { m_carryFlg = _carryFlg; }

	// 少し白くするどうかをセットする
	virtual void SetSelectWhite(const bool _whiteFlg) { m_whiteFlg = _whiteFlg; }

	Math::Vector3 GetPos() const	override { return m_pos; }

	struct Param
	{
		Math::Vector3 startPos;
		float area = 0;
	};

	// パラメータセット関数
	virtual void SetParam([[maybe_unused]] Param _param) {}
	// パラメータゲット関数
	const Param GetParam() const { return m_param; }

	virtual void SetPauseFlg(bool _pauseFlg)	override { m_pauseFlg = _pauseFlg; }

	// TerrainControllerをセットする
	void SetTerrainController(std::shared_ptr<TerrainController> _spTerrainController) { m_wpTerrainController = _spTerrainController; }

	// 当たっている地形を渡す
	const std::weak_ptr<TerrainBase> GetHitTerrain() const { return m_wpHitTerrain; }

	// 乗った際にカメラが追尾すべきかすべきでないか
	virtual bool IsCameraTracking() { return false; }

	// 運べるか運べないかを返す
	virtual const bool IsCarryCheck() const { return true; }

protected:
	//====================================
	// レイ判定
	//====================================
	// 当たったオブジェクトをリストに追加
	bool RayHitJudge(const KdCollider::RayInfo& _rayInfo, Math::Vector3& _hitPos, const bool _debugFlg = false);
	// 地面のみ　当たったオブジェクトを引数に指定した変数に保持
	bool RayHitJudge(const KdCollider::RayInfo& _rayInfo, Math::Vector3& _hitPos, std::weak_ptr<TerrainBase>& _hitObject, const bool _debugFlg = false);

	//====================================
	// スフィア判定
	//====================================
	// どう当たったか欲しい時に使う 複数のオブジェクトに当たった場合、リザルトは入れられない
	bool SphereHitJudge(const KdCollider::SphereInfo& _sphereInfo, KdCollider::CollisionResult& _collisionResult, bool& _multiHit, const bool _debugFlg = false);
	// 当たったか当たってないかだけが欲しいときに使う
	bool SphereHitJudge(const KdCollider::SphereInfo& _sphereInfo, const bool _debugFlg = false);
	// 地面とのスフィア判定
	bool SphereHitGround(const KdCollider::SphereInfo& _sphereInfo, const bool _debugFlg = false);


	// 当たったら一緒に動くような地形に当たった際の処理のための構造体
	struct HitMoveTerrain
	{
		Math::Matrix transMat = Math::Matrix::Identity;	// 動く前の行列
		bool hitFlg = false;							// 当たったかどうか
	};

	// 動く床
	HitMoveTerrain m_moveGround;
	// 回る床
	HitMoveTerrain m_rotationGround;


	// モデル
	std::shared_ptr<KdModelData> m_spModel	= nullptr;

	// パラメータ変数
	Param m_param;

	// 座標
	Math::Vector3 m_pos;

	// プレイヤー
	std::weak_ptr<Player> m_wpPlayer;

	// 重力
	float			m_gravity		= 0;
	const float		m_gravityPow	= 0.005f;
	const float		m_maxGravity	= 0.6f;

	// 持たれているかどうかのフラグ
	bool m_carryFlg = false;

	// 動きを完全に停止させるフラグ
	bool m_pauseFlg = false;

	// TerrainController
	std::weak_ptr<TerrainController> m_wpTerrainController;

	// 当たった地形を保持
	std::weak_ptr<TerrainBase> m_wpHitTerrain;

	// 当たったオブジェクトを保持
	std::list<std::weak_ptr<KdGameObject>> m_wpHitObjectList;

	// 少し白くする
	bool m_whiteFlg = false;
};