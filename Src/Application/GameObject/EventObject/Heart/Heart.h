#pragma once

class Heart : public KdGameObject
{
public:
	Heart()						{}
	~Heart()			override{}

	void Update()		override;
	void GenerateDepthMapFromLight()	override;
	void DrawLit()		override;
	void DrawBright()	override;
	void Init()			override;

	void OnHit()		override;

	void Reset()		override;

private:
	std::shared_ptr<KdModelData> m_spModel	= nullptr;

	bool m_aliveFlg = true;
};