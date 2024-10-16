#pragma once

class ResultPlayer : public KdGameObject
{
public:
	ResultPlayer() {}
	~ResultPlayer()	override {}

	void Update()	override;
	void GenerateDepthMapFromLight()	override;
	void DrawLit()	override;
	void Init()		override;

	void SetStartAnimation() { m_startAnimationFlg = true; }

	const bool GetEndAnimation() const { return m_endAnimationFlg; }

private:
	// モデル
	std::shared_ptr<KdModelWork> m_spModel = nullptr;

	// アニメーション
	std::shared_ptr<KdAnimator>	m_spAnimator = nullptr;

	// 動くスピード
	const float m_speed = 0.1f;
	// 動く方向
	const Math::Vector3 m_moveVec = { 1, 0, 0 };
	// 回転角度
	float m_degAng = 0;

	// 初期座標
	const Math::Vector3 m_startPos = { -2, 0, -10 };
	// ゴールの座標
	const Math::Vector3 m_goalPos = { 15, 0, -5 };

	// アニメーションがスタートしたか
	bool m_startAnimationFlg = false;

	// アニメーションスタートOKになってから実際に動き始めるまでの時間とカウント
	int m_stayCount = 0;
	const int m_stayTime = 60;

	// アニメーションが終わったかどうか
	bool m_endAnimationFlg = false;
};