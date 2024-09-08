#pragma once

class Player;

class CameraBase : public KdGameObject
{
public:
	CameraBase()						{}
	virtual ~CameraBase()	override	{}

	void PostUpdate()		override	{}
	void PreDraw()			override;
	void Init()				override;

	void SetTarget(const std::shared_ptr<Player>& target);

	// 「絶対変更しません！見るだけ！」な書き方
	const std::shared_ptr<KdCamera>& GetCamera() const
	{
		return m_spCamera;
	}

	// 「中身弄るかもね」な書き方
	std::shared_ptr<KdCamera> WorkCamera() const
	{
		return m_spCamera;
	}

	const Math::Matrix GetRotationMatrix()const
	{
		return Math::Matrix::CreateFromYawPitchRoll(
		       DirectX::XMConvertToRadians(m_DegAng.y),
		       DirectX::XMConvertToRadians(m_DegAng.x),
		       DirectX::XMConvertToRadians(m_DegAng.z));
	}

	const Math::Matrix GetRotationYMatrix() const
	{
		return Math::Matrix::CreateRotationY(
			   DirectX::XMConvertToRadians(m_DegAng.y));
	}

private:

protected:

	// カメラ回転用角度
	Math::Vector3				m_DegAng = Math::Vector3::Zero;

	void UpdateRotateByMouse();

	std::shared_ptr<KdCamera>	m_spCamera		= nullptr;
	std::weak_ptr<Player>		m_wpTarget;

	Math::Matrix				m_mLocalPos		= Math::Matrix::Identity;
	Math::Matrix				m_mRotation		= Math::Matrix::Identity;

	// カメラ回転用マウス座標の差分
	POINT						m_FixMousePos{};
};