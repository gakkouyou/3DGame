#include "TitleCamera.h"
#include "Application/main.h"

void TitleCamera::Update()
{
	Math::Matrix debugMat = Math::Matrix::Identity;
	m_moveVec = Math::Vector3::Zero;

	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		if (!m_shiftFlg)
		{
			m_shiftFlg = true;
			SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);
		}
		else
		{
			UpdateRotateByMouse();
		}
	}
	else
	{
		m_shiftFlg = false;
	}

	// WASDで移動
	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_moveVec.z += 1.0f;
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_moveVec.x -= 1.0f;
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_moveVec.z -= 1.0f;
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_moveVec.x += 1.0f;
	}

	// カメラの向きで移動方向を補正
	Math::Matrix rotMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_DegAng.y), 0, DirectX::XMConvertToRadians(m_DegAng.z));
	m_moveVec = m_moveVec.TransformNormal(m_moveVec, rotMat);
	m_moveVec.Normalize();
	m_moveVec.y = 0;

	// スペースで上に行く
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		m_moveVec.y += 1.0f;
	}
	// controlで下に行く
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		m_moveVec.y -= 1.0f;
	}


	m_debugPos += m_moveVec * m_moveSpeed;

	debugMat = Math::Matrix::CreateTranslation(m_debugPos);

	// カメラの回転
	m_mRotation = GetRotationMatrix();

	static float angle = 0;
	angle += 0.5f;
	if (angle >= 360)
	{
		angle -= 360;
	}

	// ローカル行列変更
	Math::Matrix transMat = Math::Matrix::CreateTranslation(0 + (sin(DirectX::XMConvertToRadians(angle))) * 0.5f, 10.0f, -20.0f);

	m_mWorld = m_mRotation * transMat * debugMat;

	Application::Instance().m_log.AddLog("%.2f, %.2f, %.2f\n", GetPos().x, GetPos().y, GetPos().z);
}

void TitleCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	m_DegAng = { 20.0f, 0, 0 };
}
