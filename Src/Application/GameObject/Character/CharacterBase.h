#pragma once

class TerrainController;
class TerrainBase;
class CarryObjectController;
class CarryObjectBase;

class CharacterBase : public KdGameObject
{
public:
	CharacterBase()											{}
	virtual ~CharacterBase()					override	{}

	// 初期化
	virtual void Init()							override {}

	// 更新
	virtual void Update()						override {}
	virtual void PostUpdate()					override {}

	// 描画
	virtual void GenerateDepthMapFromLight()	override;
	virtual void DrawLit()						override;

	// 生存フラグ
	const bool GetAliveFlg() const { return m_aliveFlg; }

	// 座標セット
	void SetPos(const Math::Vector3& _pos)	override { m_pos = _pos; }
	void SetPos(const float _x, const float _y, const float _z) { m_pos = { _x, _y, _z }; }
	// 座標ゲット
	Math::Vector3 GetPos() const	override { return m_pos; }

	// 重力をセットする
	void SetGravity(const float _gravity) { m_gravity = _gravity; }

	// 当たった時の処理
	void OnHit()		override {}

	// リセット処理
	void Reset()		override;

	virtual void SetPauseFlg(bool _pauseFlg)	override { m_pauseFlg = _pauseFlg; }

	// TerrainControllerをセットする
	void SetTerrainController(const std::weak_ptr<TerrainController>& _wpTerrainController) { m_wpTerrainController = _wpTerrainController; }

	// 運べるオブジェクトのコントローラーをセット
	void SetCarryObjectContoller(const std::shared_ptr<CarryObjectController>& _spCarryObjectController) { m_wpCarryObjectController = _spCarryObjectController; }

protected:

	//====================================
	// レイ判定
	//====================================
	// 当たったオブジェクトをリストに追加
	bool RayHitJudge(const KdCollider::RayInfo& _rayInfo, Math::Vector3& _hitPos, const bool _debugFlg = false);
	// 地面のみ　当たったオブジェクトを引数に指定した変数に保持
	bool RayHitJudge(const KdCollider::RayInfo& _rayInfo, Math::Vector3& _hitPos, std::weak_ptr<TerrainBase>& _hitObject, const bool _debugFlg = false);
	// 運べるオブジェクトのみ　当たったオブジェクトを引数に指定した変数に保持
	bool RayHitJudge(const KdCollider::RayInfo& _rayInfo, Math::Vector3& _hitPos, std::weak_ptr<CarryObjectBase>& _hitObject, const bool _debugFlg = false);

	//====================================
	// スフィア判定
	//====================================
	// どう当たったか欲しい時に使う 複数のオブジェクトに当たった場合、リザルトは入れられない
	bool SphereHitJudge(const KdCollider::SphereInfo& _sphereInfo, KdCollider::CollisionResult& _collisionResult, bool& _multiHit, const bool _debugFlg = false);
	// 当たったか当たってないかだけが欲しいときに使う
	bool SphereHitJudge(const KdCollider::SphereInfo& _sphereInfo, const bool _debugFlg = false);

	// キャラを向かせたい方向に向かせる 回転したらtrue (回転角度, 向きたいベクトル, 回転制御(必要なら))
	bool RotationCharacter(float& _degAng, Math::Vector3 _toVec, const float _minDegAng = 360);

	// モデル
	std::shared_ptr<KdModelWork>	m_spModel		= nullptr;
	// アニメーション
	std::shared_ptr<KdAnimator>		m_spAnimator	= nullptr;

	// 生存フラグ
	bool m_aliveFlg									= true;

	// 座標
	Math::Vector3	m_pos							= Math::Vector3::Zero;
	// 移動
	Math::Vector3	m_moveVec						= Math::Vector3::Zero;
	// 移動量
	float			m_moveSpeed						= 0;

	// 重力
	float			m_gravity						= 0;
	float			m_gravityPow					= 0.005f;
	const float		m_maxGravity					= 0.6f;

	// 当たり判定の段差許容範囲
	const float		m_enableStepHeight				= 0.125f;

	// 当たったオブジェクトを保持
	std::list<std::weak_ptr<KdGameObject>> m_wpHitObjectList;

	bool m_pauseFlg = false;

	// TerrainController
	std::weak_ptr<TerrainController> m_wpTerrainController;

	// 当たった地形を保持
	std::weak_ptr<TerrainBase> m_wpHitTerrain;

	// 運べるオブジェクトコントローラー
	std::weak_ptr<CarryObjectController> m_wpCarryObjectController;

	// 運べるオブジェクト当たった奴
	std::weak_ptr<CarryObjectBase> m_wpHitCarryObject;
};