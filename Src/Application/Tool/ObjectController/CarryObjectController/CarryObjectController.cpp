#include "CarryObjectController.h"
#include "../../DebugWindow/DebugWindow.h"
#include "../../../Scene/SceneManager.h"
#include "../../../main.h"
#include "../../../GameObject/Camera/TPSCamera/TPSCamera.h"

#include "../TerrainController/TerrainController.h"

#include "../../../GameObject/Terrain/CarryObject/Box/Box.h"
#include "../../../GameObject/Terrain/CarryObject/BoxEnemy/BoxEnemy.h"

#include "../../../GameObject/Character/Player/Player.h"

void CarryObjectController::Update()
{
	// マウスでオブジェクトを選択する
	MouseSelect();

	// 対象のオブジェクト
	std::shared_ptr<CarryObjectBase> spTargetObject = m_wpTargetObject.lock();
	if (spTargetObject)
	{
		DebugWindow::CarryObjectParam debugParam = DebugWindow::Instance().GetCarryObjectParam();
		CarryObjectBase::Param setParam{ debugParam.pos, debugParam.area };
		spTargetObject->SetParam(setParam);
	}

	// DELETEキーで削除する
	if (GetAsyncKeyState(VK_DELETE) & 0x8000)
	{
		DeleteObject();
	}
}

void CarryObjectController::Init()
{
	// CSVを読み込む
	CSVLoader();

	// 読み込んだデータからオブジェクトを作成する
	BeginCreateObject();
}

void CarryObjectController::Reset()
{
	m_objectCount = m_objectCountReset;
	m_wpObjectList.clear();

	// 読み込んだデータからオブジェクトを作成する
	BeginCreateObject();
}

const KdGameObject::ObjectType CarryObjectController::GetObjectType() const
{
	if (!m_wpTargetObject.expired())
	{
		return m_wpTargetObject.lock()->GetObjectType();
	}
	else
	{
		return KdGameObject::ObjectType::None;
	}
}

const std::string CarryObjectController::GetObjectName() const
{
	if (!m_wpTargetObject.expired())
	{
		return m_wpTargetObject.lock()->GetObjectName();
	}
	else
	{
		return "NoTarget";
	}
}

void CarryObjectController::ConfirmedObject()
{
	std::shared_ptr<CarryObjectBase> spTargetObject = m_wpTargetObject.lock();
	if (spTargetObject)
	{
		// もし名前が決められてなかったら新たにdataListに追加する
		if (spTargetObject->GetObjectName() == "None")
		{
			Data data;
			// オブジェクトのタイプと名前を入れる
			switch (spTargetObject->GetObjectType())
			{
				// 箱の場合
			case ObjectType::Box:
				// タイプのセット
				data.type = "Box";
				// カウントを進める
				m_objectCount.Box++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.Box);
				break;

				// 箱の敵の場合
			case ObjectType::BoxEnemy:
				// タイプのセット
				data.type = "BoxEnemy";
				// カウントを進める
				m_objectCount.BoxEnemy++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.BoxEnemy);
				break;
			}
			// 名前をセットする
			spTargetObject->SetObjectName(data.name);
			// 情報をセットする
			data.pos = spTargetObject->GetParam().startPos;	// 座標
			data.area = spTargetObject->GetParam().area;	// 動いていい範囲
			// データが入っているリストにプッシュバックする
			m_dataList.push_back(data);
			// リストにプッシュバックする
			m_wpObjectList.push_back(spTargetObject);
		}
		// 決められていたら上書きする
		else
		{
			// 何個目に上書きするかを格納する変数
			int num = 0;
			for (int i = 0; i < (int)m_dataList.size(); i++)
			{
				if (m_dataList[i].name == spTargetObject->GetObjectName())
				{
					num = i;
					break;
				}
			}
			m_dataList[num].pos = spTargetObject->GetParam().startPos;	// 座標
			m_dataList[num].area = spTargetObject->GetParam().area;		// 触れれる範囲
		}
	}

	m_wpTargetObject.reset();
}

void CarryObjectController::DeleteObject()
{
	// オブジェクトを削除する
	if (!m_wpTargetObject.expired())
	{
		// データ配列からも削除する
		for (int i = 0; i < (int)m_dataList.size(); i++)
		{
			if (m_dataList[i].name == m_wpTargetObject.lock()->GetObjectName())
			{
				m_dataList.erase(m_dataList.begin() + i);
			}
		}
		m_wpTargetObject.lock()->SetExpired(true);
	}
}

void CarryObjectController::CreateObject(KdGameObject::ObjectType _object)
{
	switch (_object)
	{
		// 箱
	case KdGameObject::ObjectType::Box:
	{
		std::shared_ptr<Box> object = std::make_shared<Box>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		// ターゲットをセット
		if (!m_wpPlayer.expired())
		{
			object->SetPlayer(m_wpPlayer.lock());
		}
		if (!m_wpTerrainController.expired())
		{
			object->SetTerrainController(m_wpTerrainController.lock());
		}
		break;
	}
		// 箱の敵
	case KdGameObject::ObjectType::BoxEnemy:
	{
		std::shared_ptr<BoxEnemy> object = std::make_shared<BoxEnemy>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		// ターゲットをセット
		if (!m_wpPlayer.expired())
		{
			object->SetPlayer(m_wpPlayer.lock());
		}
		if (!m_wpTerrainController.expired())
		{
			object->SetTerrainController(m_wpTerrainController.lock());
		}
		break;
	}
	}
}

void CarryObjectController::MouseSelect()
{
	if (SceneManager::Instance().GetDebug() == false) return;

	// マウスでオブジェクトを選択する
	std::shared_ptr<const TPSCamera> spCamera = m_wpCamera.lock();

	// カメラが無かったら終了
	if (!spCamera) return;

	// クリックしたら選んだオブジェクトをセットする
	if (GetAsyncKeyState('P') & 0x8000)
	{
		// マウス位置の取得
		POINT mousePos;
		GetCursorPos(&mousePos);
		ScreenToClient(Application::Instance().GetWindowHandle(), &mousePos);

		Math::Vector3	cameraPos = spCamera->GetPos();
		Math::Vector3	rayDir = Math::Vector3::Zero;
		float			rayRange = 100.0f;

		// レイの方向取得
		spCamera->GetCamera()->GenerateRayInfoFromClientPos(mousePos, cameraPos, rayDir, rayRange);

		Math::Vector3 endRayPos = cameraPos + (rayDir * rayRange);

		KdCollider::RayInfo rayInfo(KdCollider::TypeGround, cameraPos, endRayPos);

		// 当たり判定の結果
		std::list<KdCollider::CollisionResult> resultList;

		// 当たったオブジェクトのリスト
		std::vector<std::weak_ptr<CarryObjectBase>> hitObjList;

		int listSize = m_wpObjectList.size();

		// 当たり判定
		for (int i = 0; i < listSize; i++)
		{
			std::weak_ptr<CarryObjectBase> obj = m_wpObjectList[i];
			if (!obj.expired())
			{
				if (obj.lock()->Intersects(rayInfo, &resultList))
				{
					hitObjList.push_back(obj);
					// １回でも当たったらリセット
					ConfirmedObject();
				}
			}
		}

		// 一番近いオブジェクトを探す
		float maxOverLap = 0;
		int cnt = 0;

		for (auto& ret : resultList)
		{
			if (ret.m_overlapDistance > maxOverLap)
			{
				maxOverLap = ret.m_overlapDistance;
				m_wpTargetObject = hitObjList[cnt];

				CarryObjectBase::Param param = m_wpTargetObject.lock()->GetParam();
				DebugWindow::CarryObjectParam setParam{ param.startPos, param.area };
				DebugWindow::Instance().SetCarryObjectParam(setParam);
			}
			cnt++;
		}
	}
}

void CarryObjectController::BeginCreateObject()
{
	for (auto& data : m_dataList)
	{
		// 箱
		if (data.type == "Box")
		{
			std::shared_ptr<Box> object = std::make_shared<Box>();
			// パラメータをセットする
			CarryObjectBase::Param setParam{ data.pos, data.area };
			object->SetParam(setParam);
			// ターゲットをセットする
			if (!m_wpPlayer.expired())
			{
				object->SetPlayer(m_wpPlayer.lock());
			}
			object->Init();
			if (!m_wpTerrainController.expired())
			{
				object->SetTerrainController(m_wpTerrainController.lock());
			}
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.Box++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.Box);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// リストに追加
			m_wpObjectList.push_back(object);
		}
		// 箱
		else if (data.type == "BoxEnemy")
		{
			std::shared_ptr<BoxEnemy> object = std::make_shared<BoxEnemy>();
			// パラメータをセットする
			CarryObjectBase::Param setParam{ data.pos, data.area };
			object->SetParam(setParam);
			// ターゲットをセットする
			if (!m_wpPlayer.expired())
			{
				object->SetPlayer(m_wpPlayer.lock());
			}
			object->Init();
			if (!m_wpTerrainController.expired())
			{
				object->SetTerrainController(m_wpTerrainController.lock());
			}
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.BoxEnemy++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.BoxEnemy);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// リストに追加
			m_wpObjectList.push_back(object);
		}
	}
}

void CarryObjectController::CSVLoader()
{
	std::ifstream ifs(m_fileName);
	if (!ifs.is_open())
	{
		return;
	}

	m_dataList.clear();

	std::string lineString;

	while (std::getline(ifs, lineString))
	{
		std::istringstream iss(lineString);
		std::string commaString;

		int cnt = 0;
		// 空かどうかを確認するフラグ
		bool emptyFlg = true;
		Data data;
		while (std::getline(iss, commaString, ','))
		{
			emptyFlg = false;
			switch (cnt)
			{
			case 0:
				data.type = commaString;
				break;

			case 1:
				data.name = commaString;
				break;

			case 2:
				data.pos.x = stof(commaString);
				break;

			case 3:
				data.pos.y = stof(commaString);
				break;

			case 4:
				data.pos.z = stof(commaString);
				break;

			case 5:
				data.area = stof(commaString);
				break;
			}
			cnt++;
		}
		// 空じゃなかった時だけpush_backする
		if (emptyFlg == false)
		{
			m_dataList.push_back(data);
		}
	}
	ifs.close();
}

void CarryObjectController::CSVWriter()
{
	std::ofstream ofs(m_fileName);

	for (auto& data : m_dataList)
	{
		// オブジェクトのタイプ
		ofs << data.type << ",";

		// オブジェクトの名前
		ofs << data.name << ",";

		// 座標
		ofs << data.pos.x << "," << data.pos.y << "," << data.pos.z << ",";

		// 触れれる範囲
		ofs << data.area << "," << std::endl;
	}
}