#pragma once

class SavePoint : public KdGameObject
{
public:
	SavePoint()									{}
	~SavePoint()						override{}

	void Update()						override;
	void GenerateDepthMapFromLight()	override;
	void DrawLit()						override;
	void DrawUnLit()					override;
	void Init()							override;

	void SetPos(const Math::Vector3& _pos) override;
	Math::Vector3 GetPos() const		override { return m_respawnPos; }

	void OnHit()	override;

private:
	// モデル
	std::shared_ptr<KdModelData> m_spModel = nullptr;

	// 座標
	Math::Vector3 m_pos;

	// リスポーン座標
	Math::Vector3 m_respawnPos;
};