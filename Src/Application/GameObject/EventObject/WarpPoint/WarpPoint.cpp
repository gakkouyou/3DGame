#include "WarpPoint.h"

void WarpPoint::Update()
{
	if (m_pauseFlg == true) return;

	m_degAng += m_addDegAng;
	if (m_degAng <= 360)
	{
		m_degAng -= 360;
	}
}

void WarpPoint::DrawUnLit()
{
	if (m_spOutModel)
	{
		Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));
		//KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spOutModel, rotMat * m_mWorld);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spOutModel, m_outMat);
	}
	if (m_spInModel)
	{
		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);
		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);

		Math::Matrix scaleMat = Math::Matrix::CreateScale(0.98f);
		Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(-m_degAng));
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spInModel, scaleMat * rotMat * m_mWorld);

		rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spInModel, rotMat * m_mWorld);

		KdShaderManager::Instance().UndoRasterizerState();
		KdShaderManager::Instance().UndoBlendState();
	}
}

void WarpPoint::DrawBright()
{
	if (m_spOutModel)
	{
		Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));
		Math::Color color = { 0.3, 0.3, 0.3, 1 };
		//KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spOutModel, m_mWorld, color);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spOutModel, m_outMat, color);
	}
	if (m_spInModel)
	{
		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);
		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
		Math::Color color = { 0.1, 0.1, 0.1, 1 };

		Math::Matrix scaleMat = Math::Matrix::CreateScale(0.98f);
		Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(-m_degAng));
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spInModel, scaleMat * rotMat * m_mWorld, color);
		rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spInModel, rotMat * m_mWorld, color);
		KdShaderManager::Instance().UndoRasterizerState();
		KdShaderManager::Instance().UndoBlendState();
	}
}

void WarpPoint::Init()
{
	EventObjectBase::Init();
	
	m_spInModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/EventObject/WarpPoint/In/in.gltf");

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("WarpPoint", m_spInModel, KdCollider::TypeEvent | KdCollider::TypeDebug);

	m_objectType = ObjectType::WarpPoint;

	//KdEffekseerManager::GetInstance().Play("warp.efkefc", m_inPos, { 0, 0, 0 }, 1.0f, 1.0f, true);
}

void WarpPoint::OnHit()
{
}

void WarpPoint::SetParam(const Param& _param)
{
	m_param = _param;

	// ワープの入り口の座標
	m_inPos		= m_param.basePos;

	m_mWorld.Translation(m_inPos);
}
