#include "ResultCamera.h"

void ResultCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	// 初期の行列の設定
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix rotMat = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(m_degAng));
	m_mWorld = rotMat * transMat;
}
