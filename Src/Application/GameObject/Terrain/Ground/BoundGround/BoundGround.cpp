#include "BoundGround.h"

void BoundGround::Update()
{
	if (m_onHitFlg == false) return;

	if (m_sumFlg == false)
	{
		// 小さくしていく
		m_scale -= m_addScale;
		// 減算値も大きくしていく
		m_addScale -= m_addAddScale;
		// 下限を下回ったら大きくしていく
		if (m_scale < m_param.scale.y * m_minScale)
		{
			m_scale = m_minScale;
			m_sumFlg = true;
		}
	}
	else
	{
		// 大きくしていく
		m_scale += m_sumScale;
		// 上限まで行ったら終了
		if (m_scale > m_param.scale.y)
		{
			m_scale = m_param.scale.y;
			m_stayCount++;
			if (m_stayCount > m_stayTime)
			{
				m_onHitFlg = false;
				m_sumFlg = false;
				m_addScale = m_defaultAddScale;
			}
		}
	}

	// 行列確定
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_param.pos);
	Math::Matrix scaleMat = Math::Matrix::CreateScale({ 1.0f, m_scale, 1.0f });

	m_mWorld = scaleMat * transMat;
}

void BoundGround::Init()
{
	// モデルセット
	m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Terrain/Ground/BoundGround/boundGround.gltf");

	// DrawLit
	m_drawType = eDrawTypeLit | eDrawTypeDepthOfShadow;

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("BoundGround", m_spModel, KdCollider::TypeGround | KdCollider::TypeDebug);

	// オブジェクトタイプ
	m_objectType = ObjectType::BoundGround;

	TerrainBase::Init();
}

void BoundGround::OnHit()
{
	m_onHitFlg = true;
	m_sumFlg = false;
	m_addScale = m_defaultAddScale;
}

void BoundGround::SetParam(const Param& _param)
{
	m_param.pos			= _param.startPos;
	m_param.startPos	= _param.startPos;
	m_param.scale		= _param.scale;

	m_scale = m_param.scale.y;

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_param.pos);
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_param.scale);

	m_mWorld = scaleMat * transMat;

	m_setParamFlg = true;
}
