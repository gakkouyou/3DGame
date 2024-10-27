#include "House.h"

void House::Update()
{
	if (m_openDoorFlg == true && m_closeDoorFlg == false)
	{
		if (m_openSoundFlg == false)
		{
			if (m_wpOpenSound.expired() == false)
			{
				m_wpOpenSound.lock()->Play();
			}
			m_openSoundFlg = true;
		}

		m_degAng += m_addDegAng;
		if (m_degAng > m_maxDegAng)
		{
			m_degAng = m_maxDegAng;
			m_openDoorFlg = false;
		}

		Math::Vector3 pos = m_doorMat.Translation();
		m_doorMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));
		m_doorMat.Translation(pos);
	}

	if (m_closeDoorFlg == true)
	{
		m_degAng -= m_addDegAng;
		if (m_degAng < 0)
		{
			m_degAng = 0;
			m_closeDoorFlg = false;

			if (m_wpCloseSound.expired() == false)
			{
				m_wpCloseSound.lock()->Play();
			}

			// ドアの音を鳴らせるようにする
			m_openSoundFlg = false;
		}
		Math::Vector3 pos = m_doorMat.Translation();
		m_doorMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng));
		m_doorMat.Translation(pos);
	}
}

void House::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
		KdShaderManager::Instance().UndoRasterizerState();

		Math::Matrix mat = Math::Matrix::CreateTranslation({ -20, 0, 0 });
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, mat);

		mat.Translation({ 20, 0, 0 });
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, mat);
	}

	if (m_spDoorModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spDoorModel, m_doorMat);
	}

	if (m_spHouseObjectModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spHouseObjectModel, m_mWorld);
	}

	if (m_spRoadModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spRoadModel, m_roadMat);
	}

	// 木
	if (m_spTreeModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spTreeModel, m_treeMat[0]);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spTreeModel, m_treeMat[1]);
	}
}

void House::DrawLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.SetDitherEnable(true);

		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
		KdShaderManager::Instance().UndoRasterizerState();

		Math::Matrix mat = Math::Matrix::CreateTranslation({ -20, 0, 0 });
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, mat);

		mat.Translation({ 20, 0, 0 });
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, mat);
	}

	if (m_spDoorModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spDoorModel, m_doorMat);
	}

	if (m_spHouseObjectModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spHouseObjectModel, m_mWorld);
	}

	if (m_spRoadModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spRoadModel, m_roadMat);
	}

	// 木
	if (m_spTreeModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spTreeModel, m_treeMat[0]);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spTreeModel, m_treeMat[1]);
	}
}

void House::Init()
{
	// 家のモデル
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Title/House/house.gltf");
	}

	// ドアのモデル
	if (!m_spDoorModel)
	{
		m_spDoorModel = std::make_shared<KdModelData>();
		m_spDoorModel->Load("Asset/Models/Title/Door/door.gltf");
	}
	m_doorMat.Translation(m_spModel->FindNode("DoorPoint")->m_worldTransform.Translation());

	// 家具とかのモデル
	if (!m_spHouseObjectModel)
	{
		m_spHouseObjectModel = std::make_shared<KdModelData>();
		m_spHouseObjectModel->Load("Asset/Models/Title/HouseObject/houseObject.gltf");
	}

	// ベッドの座標
	m_bedPos = (m_spHouseObjectModel->FindNode("BedPoint")->m_worldTransform * m_mWorld).Translation();

	// 道路のモデル
	if (!m_spRoadModel)
	{
		m_spRoadModel = std::make_shared<KdModelData>();
		m_spRoadModel->Load("Asset/Models/Title/Road/road.gltf");

		Math::Matrix transMat = Math::Matrix::CreateTranslation({ 0, 0, -10 });
		Math::Matrix scaleMat = Math::Matrix::CreateScale(2);
		m_roadMat = scaleMat * transMat;
	}

	// 木のモデル
	if (!m_spTreeModel)
	{
		m_spTreeModel = std::make_shared<KdModelData>();
		m_spTreeModel->Load("Asset/Models/Title/Tree/tree.gltf");
		m_treeMat[0].Translation({-10, 0, -7});
		m_treeMat[1].Translation({ 8.5, 0, -7 });
	}

	// ドアを開ける音
	m_wpOpenSound = KdAudioManager::Instance().Play("Asset/Sounds/SE/doorOpen.wav", false);
	if (m_wpOpenSound.expired() == false)
	{
		m_wpOpenSound.lock()->SetVolume(0.15f);
		m_wpOpenSound.lock()->Stop();
	}

	// ドアを閉める音
	m_wpCloseSound = KdAudioManager::Instance().Play("Asset/Sounds/SE/doorClose.wav", false);
	if (m_wpCloseSound.expired() == false)
	{
		m_wpCloseSound.lock()->SetVolume(0.15f);
		m_wpCloseSound.lock()->Stop();
	}
}
