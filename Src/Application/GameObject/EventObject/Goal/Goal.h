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

	void SetPos(const Math::Vector3& _pos)	override { m_pos = _pos; }
	Math::Vector3 GetPos() const			override { return m_pos; }

	const bool GetGoalFlg() const { return m_goalFlg; }

private:
	std::shared_ptr<KdModelData> m_spModel;

	Math::Vector3 m_pos	= Math::Vector3::Zero;

	// スケール
	const float m_scale = 0.5f;

	// 少し回転させる
	float m_moveDegAng = 1.0f;
	float m_degAng = 0;

	// 上下にふよふよさせる
	float m_sinAngle = 0;

	// ゴールフラグ
	bool m_goalFlg	= false;

	int m_goalStayCnt	= 0;
	int m_goalStayTime	= 60;

	std::weak_ptr<KdSoundInstance> m_wpSound;
	bool m_flg = false;
};