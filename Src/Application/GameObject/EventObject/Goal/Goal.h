#pragma once

class Goal : public KdGameObject
{
public:
	Goal()									{}
	~Goal()							override{}

	void Update()					override;
	void DrawUnLit()				override;
	void Init()						override;

	void OnHit()	override;

	// リセット処理
	void Reset()	override;

private:
	std::shared_ptr<KdModelData> m_spModel;

	// 少し回転させる
	float m_moveDegAng = 1.0f;
	float m_degAng = 0;

	// 上下にふよふよさせる
	float m_sinAngle = 0;
};