#pragma once

class TitlePlayer : public KdGameObject
{
public:
	TitlePlayer() {}
	~TitlePlayer()	override {}

	void Update()	override;
	void GenerateDepthMapFromLight()	override;
	void DrawLit()	override;
	void Init()		override;

	void SetBedPos(const Math::Vector3& _bedPos) { m_bedPos = _bedPos; }

private:
	// モデル
	std::shared_ptr<KdModelWork> m_spModel = nullptr;

	// ベッドの座標
	Math::Vector3 m_bedPos;

	// アニメーション
	std::shared_ptr<KdAnimator>	m_spAnimator = nullptr;
};