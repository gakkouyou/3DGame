#include "RotationGround.h"

void RotationGround::Update()
{
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

	Math::Vector3 radAng;
	radAng.x = DirectX::XMConvertToRadians(m_rotDegAng.x);
	radAng.y = DirectX::XMConvertToRadians(m_rotDegAng.y);
	radAng.z = DirectX::XMConvertToRadians(m_rotDegAng.z);
	
	// 回転行列
	m_rotMat = Math::Matrix::CreateFromYawPitchRoll({ radAng.x, radAng.y, radAng.z });
	// 座標行列
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_param.pos);
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_param.scale);
	
	m_mWorld = scaleMat * m_rotMat * transMat;
}

void RotationGround::Init()
{
	// モデルロード
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		//m_spModel->Load("Asset/Models/Terrain/Ground/NormalGround/normalGround.gltf");
		m_spModel->Load("Asset/Models/Terrain/Ground/RotationGround/rotationGround.gltf");
	}

	// 当たり判定付与
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("RotationGround", m_spModel, KdCollider::TypeGround);

	// タイプ
	m_objectType = ObjectType::RotationGround;

	TerrainBase::Init();

	// 描画
	m_drawType = eDrawTypeLit;
}

void RotationGround::SetParam(const Param& _param)
{
	// 座標
	m_param.pos			= _param.startPos;
	m_param.startPos	= _param.startPos;
	m_param.scale		= _param.scale;

	// 回転角度
	m_param.degAng = _param.degAng;

	m_setParamFlg = true;
}

void RotationGround::Reset()
{
	// 角度
	m_rotDegAng = Math::Vector3::Zero;
}
