#pragma once

class Candy : public KdGameObject
{
public:
	Candy()										{}
	~Candy()							override{}

	void Update()						override;
	void GenerateDepthMapFromLight()	override;
	void DrawLit()						override;
	void Init()							override;

	// 座標
	void SetPos(const Math::Vector3& _pos) override { m_pos = _pos; }
	Math::Vector3 GetPos() const override { return m_pos; }

	void OnHit()	override;

private:
	// モデル
	std::shared_ptr<KdModelData> m_spModel = nullptr;

	// 座標
	Math::Vector3 m_pos;

	// 拡縮
	const float m_scale = 1.0f;

	// 角度
	float m_degAng = 0;
	// 回転角度
	const float m_moveDegAng = 2.0f;
};