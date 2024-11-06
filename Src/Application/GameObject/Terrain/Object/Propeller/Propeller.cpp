#include "Propeller.h"

void Propeller::Update()
{
	if (m_pauseFlg == true)return;

	// 回転させる
	m_rotDegAng += m_param.degAng;

	// 角度を0～360に留める
	if (m_rotDegAng.x > 360.0f)
	{
		m_rotDegAng.x -= 360.0f;
	}
	if (m_rotDegAng.x < 0.0f)
	{
		m_rotDegAng.x += 360.0f;
	}
	if (m_rotDegAng.y > 360.0f)
	{
		m_rotDegAng.y -= 360.0f;
	}
	if (m_rotDegAng.y < 0.0f)
	{
		m_rotDegAng.y += 360.0f;
	}
	if (m_rotDegAng.z > 360.0f)
	{
		m_rotDegAng.z -= 360.0f;
	}
	if (m_rotDegAng.z < 0.0f)
	{
		m_rotDegAng.z += 360.0f;
	}

	if (m_param.degAng.x == 0)
	{
		m_rotDegAng.x = 0;
	}
	if (m_param.degAng.y == 0)
	{
		m_rotDegAng.y = 0;
	}
	if (m_param.degAng.z == 0)
	{
		m_rotDegAng.z = 0;
	}

	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_param.scale);
	Math::Matrix rotMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_rotDegAng.y), DirectX::XMConvertToRadians(m_rotDegAng.x), DirectX::XMConvertToRadians(m_rotDegAng.z));
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_param.pos);

	m_mWorld = scaleMat * rotMat * transMat;
}

void Propeller::Init()
{
	m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Terrain/Object/Propeller/propeller.gltf");

	m_drawType = eDrawTypeLit | eDrawTypeDepthOfShadow;

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Propeller", m_spModel, KdCollider::TypeGround | KdCollider::TypeDebug);

	// オブジェクトタイプ
	m_objectType = ObjectType::Propeller;

	TerrainBase::Init();
}

void Propeller::SetParam(const Param& _param)
{
	m_param.pos			= _param.startPos;
	m_param.startPos	= _param.startPos;
	m_param.scale		= _param.scale;
	m_param.degAng		= _param.degAng;

	m_setParamFlg = true;
}

void Propeller::Reset()
{
	m_rotDegAng = Math::Vector3::Zero;

	m_pauseFlg = false;
}
