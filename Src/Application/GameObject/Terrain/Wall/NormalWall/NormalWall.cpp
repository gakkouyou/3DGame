#include "NormalWall.h"

void NormalWall::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrain/Wall/NormalWall");
	}

	m_drawType = eDrawTypeLit;

	SetPos({ 0.0f, 0.0f, 10.0f });

	m_objectType = ObjectType::NormalWall;
}