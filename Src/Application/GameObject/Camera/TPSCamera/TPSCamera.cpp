#include "TPSCamera.h"
#include "../../../Scene/SceneManager.h"

void TPSCamera::PostUpdate()
{
	// ターゲットの行列(有効な場合利用する)
	Math::Matrix								targetMat	= Math::Matrix::Identity;
	const std::shared_ptr<const KdGameObject>	spTarget	= m_wpTarget.lock();
	if (spTarget)
	{
		targetMat = Math::Matrix::CreateTranslation(spTarget->GetPos());
	}

	Math::Matrix debugMat = Math::Matrix::Identity;

	if (SceneManager::Instance().GetDebug())
	{
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
		m_moveVec = m_moveVec.TransformNormal(m_moveVec, GetRotationMatrix());
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

		// ローカル行列変更
		Math::Matrix transMat = Math::Matrix::CreateTranslation(0, 12.0f, -20.0f);

		m_mWorld = m_mRotation * transMat * targetMat * debugMat;
	}
	else
	{
		m_debugPos = Math::Vector3::Zero;
		debugMat = Math::Matrix::Identity;
		m_DegAng = Math::Vector3::Zero;
		m_mWorld = m_mLocalPos * targetMat;
	}
	CameraBase::Update();
}

void TPSCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();


	// 注視点
	Math::Matrix transMat = Math::Matrix::CreateTranslation(0, 12.0f, -20.0f);
	Math::Matrix rotMat = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(20.0f));
	m_mLocalPos = rotMat * transMat;

	SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);
}