#include "TitleCamera.h"

void TitleCamera::Update()
{
	if (m_animationFlg && m_animationFinishFlg == false)
	{
		Math::Quaternion startQua = Math::Quaternion::CreateFromRotationMatrix(m_startMat);
		Math::Quaternion goalQua = Math::Quaternion::CreateFromRotationMatrix(m_goalMat);
		Math::Quaternion nowQua = Math::Quaternion::Slerp(startQua, goalQua, m_progress);

		Math::Vector3 startPos = m_pos;
		Math::Vector3 goalPos = m_goalMat.Translation();
		Math::Vector3 nowPos = Math::Vector3::Lerp(startPos, goalPos, m_progress);

		// Y座標のみコサインカーブの軌道
		float progress = cos(DirectX::XMConvertToRadians(m_cosCurveYPos.degAng)) * -1 + 1;
		Math::Vector3 y = Math::Vector3::Lerp(startPos, goalPos, progress);
		nowPos.y = y.y;
		m_cosCurveYPos.degAng += m_cosCurveYPos.addDegAng;

		// 移動のコサインカーブ
		m_cosCurveMove.degAng += m_cosCurveMove.addDegAng;
		float speed = (cos(DirectX::XMConvertToRadians(m_cosCurveMove.degAng)) * -1 + 1) / 2;

		m_progress = speed;

		if (m_progress >= 1.0f)
		{
			m_progress = 1.0f;
			m_animationFinishFlg = true;
			m_cosCurveYPos.degAng = m_cosCurveYPos.maxDegAng;
		}

		Math::Matrix mat = Math::Matrix::CreateFromQuaternion(nowQua);
		mat.Translation(nowPos);

		m_mWorld = mat;
	}
	else if(m_animationFlg == false)
	{
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

	// Y座標のコサインカーブ
	m_cosCurveYPos.maxDegAng = 90.0f;									// 上限
	m_cosCurveYPos.addDegAng = m_cosCurveYPos.maxDegAng / (1 / m_speed);// 加算量

	// 移動のコサインカーブ
	m_cosCurveMove.maxDegAng = 180.0f;									// 上限
	m_cosCurveMove.addDegAng = m_cosCurveMove.maxDegAng / (1 / m_speed);// 加算量
}
