#pragma once

class TerrainBase : public KdGameObject
{
public:
	TerrainBase()									{}
	virtual ~TerrainBase()				override	{}

	// 更新
	virtual void Update()				override	{}
	virtual void PostUpdate()			override	{}

	// 描画
	void GenerateDepthMapFromLight()	override;
	void DrawLit()						override;
	void DrawBright()					override;

	// 初期化
	virtual void Init()							override	{}

	struct Param
	{
		Math::Vector3 pos		= Math::Vector3::Zero;
		Math::Vector3 startPos	= Math::Vector3::Zero;
		Math::Vector3 goalPos	= Math::Vector3::Zero;
		float speed				= 0;
		int stayTime			= 0;
		Math::Vector3 degAng	= Math::Vector3::Zero;
	};

	// 地形のパラメータセット関数
	virtual void SetParam(Math::Vector3 _startPos, Math::Vector3 _goalPos = Math::Vector3::Zero, float _speed = 0, int _stayTime = 0, Math::Vector3 _degAng = Math::Vector3::Zero) {}
	// 地形のパラメータゲット関数
	const Param GetParam() const { return m_param; }

protected:
	// モデルデータ
	std::shared_ptr<KdModelData>	m_spModel	= nullptr;

	// 動く地形用のパラメータ変数
	Param m_param;
};