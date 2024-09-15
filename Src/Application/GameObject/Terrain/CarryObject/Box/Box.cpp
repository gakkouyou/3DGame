#include "Box.h"
#include "../../../Character/Player/Player.h"

void Box::Update()
{
	// 運ばれていない時の処理
	if (m_carryFlg == false)
	{
		// 重力
		m_gravity += m_gravityPow;
		m_pos.y -= m_gravity;
		// 重力の制限
		if (m_gravity >= m_maxGravity)
		{
			m_gravity = m_maxGravity;
		}
	}
	// 運ばれている時の処理
	else
	{
		// プレイヤー
		std::shared_ptr<Player> spPlayer = m_wpPlayer.lock();

		if (spPlayer)
		{
			// プレイヤーのモデル
			const std::shared_ptr<KdModelWork> spPlayerModel = spPlayer->GetModel();
			// プレイヤーの角度
			const float playerDegAng = spPlayer->GetAngle();
			// 運ぶオブジェクトを置く座標の計算
			Math::Vector3 pos = spPlayerModel->FindNode("CarryPoint")->m_worldTransform.Translation();
			// 回転処理
			pos.x = pos.z * sin(DirectX::XMConvertToRadians(playerDegAng));
			pos.z = pos.z * cos(DirectX::XMConvertToRadians(-playerDegAng));
			m_pDebugWire->AddDebugSphere(pos + m_pos, 0.1f, kRedColor);

			m_pos = pos;
		}
	}

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);

	m_mWorld = transMat;
}

void Box::PostUpdate()
{
}

void Box::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/CarryObject/Box/Box.gltf");
	}

	// コライダー
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Box", m_spModel, KdCollider::TypeGround);

	// オブジェクトのタイプ
	m_objectType = ObjectType::Box;

	// ベースのオブジェクトのタイプ
	m_baseObjectType = BaseObjectType::CarryObject;
}

void Box::SetParam(Param _param)
{
	m_param = _param;
	m_pos = m_param.startPos;
}

void Box::HitJudge()
{
	
}
