#include "TitleCamera.h"
#include "Application/main.h"

void TitleCamera::Update()
{
	//Math::Matrix debugMat = Math::Matrix::Identity;
	//m_moveVec = Math::Vector3::Zero;

	//if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	//{
	//	if (!m_shiftFlg)
	//	{
	//		m_shiftFlg = true;
	//		SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);
	//	}
	//	else
	//	{
	//		UpdateRotateByMouse();
	//	}
	//}
	//else
	//{
	//	m_shiftFlg = false;
	//}

	//// WASDで移動
	//if (GetAsyncKeyState('W') & 0x8000)
	//{
	//	m_moveVec.z += 1.0f;
	//}
	//if (GetAsyncKeyState('A') & 0x8000)
	//{
	//	m_moveVec.x -= 1.0f;
	//}
	//if (GetAsyncKeyState('S') & 0x8000)
	//{
	//	m_moveVec.z -= 1.0f;
	//}
	//if (GetAsyncKeyState('D') & 0x8000)
	//{
	//	m_moveVec.x += 1.0f;
	//}

	//// カメラの向きで移動方向を補正
	//Math::Matrix rotMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_DegAng.y), 0, DirectX::XMConvertToRadians(m_DegAng.z));
	//m_moveVec = m_moveVec.TransformNormal(m_moveVec, rotMat);
	//m_moveVec.Normalize();
	//m_moveVec.y = 0;

	//// スペースで上に行く
	//if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	//{
	//	m_moveVec.y += 1.0f;
	//}
	//// controlで下に行く
	//if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	//{
	//	m_moveVec.y -= 1.0f;
	//}


	//m_debugPos += m_moveVec * m_moveSpeed;

	//debugMat = Math::Matrix::CreateTranslation(m_debugPos);

	//// カメラの回転
	//m_mRotation = GetRotationMatrix();

	//static float angle = 0;
	//angle += 0.5f;
	//if (angle >= 360)
	//{
	//	angle -= 360;
	//}

	//// ローカル行列変更
	//Math::Matrix transMat = Math::Matrix::CreateTranslation(0 + (sin(DirectX::XMConvertToRadians(angle))) * 0.5f, 10.0f, -20.0f);

	//m_mWorld = m_mRotation * transMat * debugMat;

	//Application::Instance().m_log.AddLog("%.2f, %.2f, %.2f\n", GetPos().x, GetPos().y, GetPos().z);





	if (m_animationFlg)
	{
		static float cnt = 0;

		Math::Quaternion startQua = Math::Quaternion::CreateFromRotationMatrix(m_startMat);
		Math::Quaternion goalQua = Math::Quaternion::CreateFromRotationMatrix(m_goalMat);
		Math::Quaternion nowQua = Math::Quaternion::Slerp(startQua, goalQua, cnt);

		Math::Vector3 startPos = m_pos;
		Math::Vector3 goalPos = m_goalMat.Translation();
		Math::Vector3 nowPos = Math::Vector3::Lerp(startPos, goalPos, cnt);

		cnt += 0.005f;

		if (cnt > 1.0f)
		{
			cnt = 1.0f;
			m_animationFinishFlg = true;
		}

		Math::Matrix mat = Math::Matrix::CreateFromQuaternion(nowQua);
		mat.Translation(nowPos);

		m_mWorld = mat;
	}
	else
	{
		//static float angle = 0;
		//angle += 0.5f;
		//if (angle >= 360)
		//{
		//	angle -= 360;
		//}

		//m_pos.x = sin(DirectX::XMConvertToRadians(angle)) * 0.5f;
		//m_startMat.Translation(m_pos);

		m_mWorld = m_startMat;

		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			m_animationFlg = true;
		}
	}
}

void TitleCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	// 初期の行列の設定
	m_pos = { 0.0f, 10.0f, -20.0f };
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix rotMat = Math::Matrix::CreateFromYawPitchRoll(0, DirectX::XMConvertToRadians(20.0f), 0);
	m_startMat = rotMat * transMat;

	// ゴールの行列の設定
	Math::Vector3 targetPos = m_wpTarget.lock()->GetPos();
	targetPos.y += 0.5f;
	targetPos.z += 1.0f;
	transMat = Math::Matrix::CreateTranslation(targetPos);
	rotMat = Math::Matrix::CreateFromYawPitchRoll(0, DirectX::XMConvertToRadians(90.0f), 0);
	m_goalMat = rotMat * transMat;

	m_DegAng = { 20.0f, 0, 0 };
}
