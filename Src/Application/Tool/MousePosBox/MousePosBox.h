#pragma once

class TPSCamera;

class MousePosBox : public KdGameObject
{
public:
	MousePosBox() {}
	~MousePosBox()		override {}

	void Update()		override;
	void Init()			override;

	// カメラセット
	void SetCamera(const std::weak_ptr<TPSCamera>& _wpCamera) { m_wpCamera = _wpCamera; }

	void DrawLit()		override;

private:
	// モデル
	std::shared_ptr<KdModelData>	m_spModel	= nullptr;

	// ローカル行列
	Math::Matrix					m_localMat	= Math::Matrix::Identity;

	// カメラ
	std::weak_ptr<TPSCamera>		m_wpCamera;
};