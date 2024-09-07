﻿#pragma once

// ゲーム上に存在するすべてのオブジェクトの基底となるクラス
class KdGameObject : public std::enable_shared_from_this<KdGameObject>
{
public:

	// どのような描画を行うのかを設定するTypeID：Bitフラグで複数指定可能
	enum
	{
		eDrawTypeLit = 1 << 0,
		eDrawTypeUnLit = 1 << 1,
		eDrawTypeBright = 1 << 2,
		eDrawTypeUI = 1 << 3,
		eDrawTypeDepthOfShadow = 1 << 4,
	};

	KdGameObject() {}
	virtual ~KdGameObject() { Release(); }

	// 生成される全てに共通するパラメータに対する初期化のみ
	virtual void Init() {}

	virtual void PreUpdate() {}
	virtual void Update() {}
	virtual void PostUpdate() {}

	// それぞれの状況で描画する関数
	virtual void GenerateDepthMapFromLight() {}
	virtual void PreDraw() {}
	virtual void DrawLit() {}
	virtual void DrawUnLit() {}
	virtual void DrawBright() {}
	virtual void DrawSprite() {}

	virtual void DrawDebug();

	virtual void SetAsset(const std::string&) {}

	virtual void SetPos(const Math::Vector3& pos) { m_mWorld.Translation(pos); }
	virtual Math::Vector3 GetPos() const { return m_mWorld.Translation(); }

	// 拡大率を変更する関数
	void SetScale(float scalar);
	virtual void SetScale(const Math::Vector3& scale);
	virtual Math::Vector3 GetScale() const;

	const Math::Matrix& GetMatrix() const { return m_mWorld; }

	virtual bool IsExpired() const { return m_isExpired; }
	virtual void SetExpired(const bool _isExpired) { m_isExpired = _isExpired; }

	virtual bool IsVisible()	const { return false; }
	virtual bool IsRideable()	const { return false; }

	// 視錐台範囲内に入っているかどうか
	virtual bool CheckInScreen(const DirectX::BoundingFrustum&) const { return false; }

	// カメラからの距離を計算
	virtual void CalcDistSqrFromCamera(const Math::Vector3& camPos);

	float GetDistSqrFromCamera() const { return m_distSqrFromCamera; }

	UINT GetDrawType() const { return m_drawType; }

	bool Intersects(const KdCollider::SphereInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults);
	bool Intersects(const KdCollider::BoxInfo& targetBox, std::list<KdCollider::CollisionResult>* pResults);
	bool Intersects(const KdCollider::RayInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults);

	// 当たった時の処理
	virtual void OnHit() {}

	// リセット処理
	virtual void Reset() {}

	// オブジェクトのタイプ
	enum class ObjectType
	{
		Player,			// プレイヤー
		NormalGround,	// 触れても何も起きない地形
		BoundGround,	// 触れたら跳ねる地形
		NormalWall,		// 触れても何も起きない壁
		MoveGround,		// 動く床
		RotationGround,	// 回転する床
		FenceBar,			// 柵
		FencePillar,		// 片方柵
		DropGround,		// 落ちる床

		NormalEnemy,	// 敵
		FlyEnemy,		// 飛ぶ敵

		None,			// ない
	};

	// オブジェクトのおおまかな種類
	enum class BaseObjectType
	{
		Player,		// プレイヤー
		Enemy,		// エネミー
		Ground,		// 地形
		Event,		// 触れたらイベントが起こる

		None,		// ない
	};

	// オブジェクトのタイプをゲットする
	ObjectType GetObjectType() const { return m_objectType; }

	// 大まかなオブジェクトのタイプをゲットする
	BaseObjectType GetBaseObjectType() const { return m_baseObjectType; }

	// オブジェクトの名前をセットする
	void SetObjectName(const std::string _objectName) { m_objectName = _objectName; }
	// オブジェクトの名前をゲットする
	const std::string GetObjectName() const { return m_objectName; }

	// 更新を全て止める
	virtual void SetPauseFlg(bool _pauseFlg) {}

protected:

	void Release() {}

	// 描画タイプ・何の描画を行うのかを決める / 最適な描画リスト作成用
	UINT m_drawType = 0;

	// カメラからの距離
	float m_distSqrFromCamera = 0;

	// 存在消滅フラグ
	bool m_isExpired = false;

	// 3D空間に存在する機能
	Math::Matrix	m_mWorld;

	// 当たり判定クラス
	std::unique_ptr<KdCollider> m_pCollider = nullptr;

	// デバッグ情報クラス
	std::unique_ptr<KdDebugWireFrame> m_pDebugWire = nullptr;

	// オブジェクトのタイプ
	ObjectType m_objectType		= ObjectType::None;

	// 大まかなオブジェクトのタイプ
	BaseObjectType m_baseObjectType = BaseObjectType::None;

	// オブジェクトの名前
	std::string m_objectName	= "None";
};
