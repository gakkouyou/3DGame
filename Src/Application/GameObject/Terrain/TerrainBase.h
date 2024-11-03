#pragma once

class TerrainBase : public KdGameObject
{
public:
	TerrainBase()										 {}
	virtual ~TerrainBase()						override {}

	// 更新
	virtual void Update()						override {}
	virtual void PostUpdate()					override {}
	// 描画
	virtual void GenerateDepthMapFromLight()	override;
	virtual void DrawLit()						override;
	virtual void DrawBright()					override;
	// 初期化
	virtual void Init()							override;

	virtual void OnHit()						override {}

	virtual void Reset()						override {}

	virtual void Active() {}

	virtual const bool IsActive()	const { return false; }

	bool CheckInScreen(const DirectX::BoundingFrustum& frustum) override;

	struct Param
	{
		Math::Vector3	pos			= Math::Vector3::Zero;
		Math::Vector3	startPos	= Math::Vector3::Zero;
		Math::Vector3	goalPos		= Math::Vector3::Zero;
		Math::Vector3	scale		= Math::Vector3::Zero;
		float			speed		= 0;
		int				stayTime	= 0;
		Math::Vector3	degAng		= Math::Vector3::Zero;
		std::string		targetName;
		bool			yetActive	= false;
	};

	// 地形のパラメータセット関数
	virtual void SetParam(const Param& _param) { m_param = _param; }
	// 地形のパラメータゲット関数
	const Param GetParam() const { return m_param; }

	void SetPauseFlg(const bool _pauseFlg)	override { m_pauseFlg = _pauseFlg; }

	void SetTarget(const std::weak_ptr<TerrainBase>& _wpTarget);

	// 乗った際にカメラが追尾すべきかすべきでないか
	virtual bool IsCameraTracking() { return true; }

protected:
	// モデルデータ
	std::shared_ptr<KdModelData>	m_spModel	= nullptr;

	// パラメータ変数
	Param m_param;

	// ポーズ画面かどうか
	bool m_pauseFlg = false;

	// ターゲット
	std::weak_ptr<TerrainBase> m_wpTarget;

	// SetParamに入ったかどうかのフラグ
	bool m_setParamFlg = false;

	// ディザ抜きするかどうか
	bool m_ditherFlg = false;
};