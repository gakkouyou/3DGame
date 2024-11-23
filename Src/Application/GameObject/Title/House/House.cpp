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
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel[0], m_mWorld);
		KdShaderManager::Instance().UndoRasterizerState();

		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel[1], m_houseMat[1]);

		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel[2], m_houseMat[2]);
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
		// アルファディザ有効化
		KdShaderManager::Instance().m_StandardShader.SetDitherEnable(true, 3.0f);

		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel[0], m_mWorld);
		KdShaderManager::Instance().UndoRasterizerState();

		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel[1], m_houseMat[1]);

		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel[2], m_houseMat[2]);
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
	DataLoad();

	// 家のモデル
	m_spModel[0] = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Title/House/house.gltf");
	m_spModel[1] = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Title/GreenHouse/greenHouse.gltf");
	m_spModel[2] = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Title/BlueHouse/blueHouse.gltf");
	// 行列作成
	for (int i = 0; i < 3; i++)
	{
		m_houseMat[i] = Math::Matrix::CreateTranslation(m_housePos[i]);
	}
	m_mWorld = m_houseMat[0];

	// ドアのモデル
	m_spDoorModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Title/Door/door.gltf");
	m_doorMat.Translation(m_spModel[0]->FindNode("DoorPoint")->m_worldTransform.Translation());

	// 家具とかのモデル
	m_spHouseObjectModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Title/HouseObject/houseObject.gltf");

	// ベッドの座標
	m_bedPos = (m_spHouseObjectModel->FindNode("BedPoint")->m_worldTransform * m_mWorld).Translation();

	// 道路のモデル
	m_spRoadModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Title/Road/road.gltf");

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_roadPos);
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_roadScale);
	m_roadMat = scaleMat * transMat;

	// 木のモデル
	m_spTreeModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Title/Tree/tree.gltf");
	m_treeMat[0].Translation(m_treePos[0]);
	m_treeMat[1].Translation(m_treePos[1]);

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

void House::DataLoad()
{
	// JSONファイルを読み込む
	std::ifstream file(m_path.data());
	if (!file.is_open()) return;

	nlohmann::json data;
	file >> data;

	// JSONデータを格納していく
	// 家の座標
	m_housePos[0].x = data["House"]["m_housePos[0].x"];
	m_housePos[0].y = data["House"]["m_housePos[0].y"];
	m_housePos[0].z = data["House"]["m_housePos[0].z"];
	m_housePos[1].x = data["House"]["m_housePos[1].x"];
	m_housePos[1].y = data["House"]["m_housePos[1].y"];
	m_housePos[1].z = data["House"]["m_housePos[1].z"];
	m_housePos[2].x = data["House"]["m_housePos[2].x"];
	m_housePos[2].y = data["House"]["m_housePos[2].y"];
	m_housePos[2].z = data["House"]["m_housePos[2].z"];
	// 道路の座標
	m_roadPos.x		= data["House"]["m_roadPos.x"];
	m_roadPos.y		= data["House"]["m_roadPos.y"];
	m_roadPos.z		= data["House"]["m_roadPos.z"];
	// 道路の拡縮
	m_roadScale		= data["House"]["m_roadScale"];
	// 木の座標
	m_treePos[0].x	= data["House"]["m_treePos[0].x"];
	m_treePos[0].y	= data["House"]["m_treePos[0].y"];
	m_treePos[0].z	= data["House"]["m_treePos[0].z"];
	m_treePos[1].x	= data["House"]["m_treePos[1].x"];
	m_treePos[1].y	= data["House"]["m_treePos[1].y"];
	m_treePos[1].z	= data["House"]["m_treePos[1].z"];

	// ドアの足しこむ角度
	m_addDegAng		= data["House"]["m_addDegAng"];
	// ドアの角度の上限
	m_maxDegAng		= data["House"]["m_maxDegAng"];
}
