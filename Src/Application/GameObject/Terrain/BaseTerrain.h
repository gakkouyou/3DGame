#pragma once

class BaseCharacter;

class BaseTerrain : public KdGameObject
{
public:
	BaseTerrain()									{}
	virtual ~BaseTerrain()				override	{}

	// 更新
	virtual void Update()				override	{}
	virtual void PostUpdate()			override	{}

	// 描画
	void GenerateDepthMapFromLight()	override;
	void DrawLit()						override;
	void DrawBright()					override;

	// 初期化
	virtual void Init()							override	{}

protected:
	// モデルデータ
	std::shared_ptr<KdModelData>	m_spModel	= nullptr;
};