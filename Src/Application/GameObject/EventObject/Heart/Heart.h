#pragma once

class Heart : public KdGameObject
{
public:
	Heart()						{}
	~Heart()			override{}

	void Update()		override;
	void DrawLit()		override;
	void DrawBright()	override;
	void Init()			override;

	void OnHit()		override;

private:
	std::shared_ptr<KdModelData> m_spModel	= nullptr;
};