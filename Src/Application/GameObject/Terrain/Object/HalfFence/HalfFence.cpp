#include "HalfFence.h"

void HalfFence::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/Object/HalfFence/halfFence.gltf");
	}

	// 描画タイプ
	m_drawType |= eDrawTypeLit;
	m_drawType |= eDrawTypeDepthOfShadow;

	// 当たり判定
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Fence", m_spModel, KdCollider::TypeGround);

	// オブジェクトのタイプ
	m_objectType = KdGameObject::ObjectType::HalfFence;

	TerrainBase::Init();
}

void HalfFence::SetParam(Math::Vector3 _startPos, Math::Vector3 _goalPos, Math::Vector3 _scale, float _speed, int _stayTime, Math::Vector3 _degAng)
{
	m_param.pos = _startPos;
	m_param.startPos = _startPos;
	m_param.degAng = _degAng;
	m_param.scale	= _scale;

	Math::Matrix rotMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(_degAng.y), DirectX::XMConvertToRadians(_degAng.x), DirectX::XMConvertToRadians(_degAng.z));
	Math::Matrix transMat = Math::Matrix::CreateTranslation(_startPos);
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_param.scale);

	m_mWorld = scaleMat * rotMat * transMat;
}