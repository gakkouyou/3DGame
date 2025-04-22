#include "CameraChange.h"
#include "../../Tool/DebugWindow/DebugWindow.h"

void CameraChange::Update()
{
	bool oldFlg = m_cameraChangeModeFlg;
	m_cameraChangeModeFlg = DebugWindow::Instance().GetCameraChangeMode();

	if (m_pCollider)
	{
		if (oldFlg != m_cameraChangeModeFlg)
		{
			// カメラチェンジモードになった瞬間当たり判定を付与
			if (m_cameraChangeModeFlg == true)
			{
				m_pCollider->SetEnable("CameraChange", true);
			}
			// カメラチェンジモードじゃなくなった瞬間当たり判定を削除
			else
			{
				m_pCollider->SetEnable("CameraChange", false);
			}
		}
	}

	if (m_cameraChangeModeFlg == false) return;

	WriteDebugWire();
}

void CameraChange::Init()
{
	m_pCollider = std::make_unique<KdCollider>();

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	m_objectType = KdGameObject::ObjectType::CameraChange;
}

void CameraChange::SetParam(Param& _param)
{
	m_param = _param;

	DirectX::BoundingSphere sphere;
	sphere.Center = m_param.pos;
	sphere.Radius = m_param.area;
	m_pCollider->RegisterCollisionShape("CameraChange", sphere, KdCollider::TypeDebug);

	if (m_cameraChangeModeFlg == false) m_pCollider->SetEnable("CameraChange", false);

}

void CameraChange::WriteDebugWire()
{
	if (!m_pDebugWire) return;

	m_pDebugWire->AddDebugSphere(m_param.pos, m_param.area);

	m_pDebugWire->AddDebugSphere(m_param.targetPos, 1.0f, kRedColor);
}
