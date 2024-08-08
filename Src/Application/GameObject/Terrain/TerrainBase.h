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

	struct Info
	{
		Math::Vector3 pos		= Math::Vector3::Zero;
		Math::Vector3 startPos	= Math::Vector3::Zero;
		Math::Vector3 goalPos	= Math::Vector3::Zero;
		float speed				= 0;
		int stayTime			= 0;
		Math::Vector3 degAng	= Math::Vector3::Zero;
	};

	// 動く地形用の情報セット関数
	virtual void SetInfo(Math::Vector3 _startPos, Math::Vector3 _goalPos = Math::Vector3::Zero, float _speed = 0, int _stayTime = 0, Math::Vector3 _degAng = Math::Vector3::Zero) {}
	const Info GetInfo() const { return m_info; }

protected:
	// モデルデータ
	std::shared_ptr<KdModelData>	m_spModel	= nullptr;

	// 動く地形用の情報変数
	Info m_info;
};