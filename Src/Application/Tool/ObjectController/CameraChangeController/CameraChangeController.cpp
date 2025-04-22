#include "CameraChangeController.h"
#include "../../DebugWindow/DebugWindow.h"
#include "../../../Scene/SceneManager.h"
#include "../../../main.h"
#include "../../../GameObject/Camera/TPSCamera/TPSCamera.h"

#include "../../../GameObject/CameraChange/CameraChange.h"

void CameraChangeController::Update()
{
	// 対象のオブジェクト
	std::shared_ptr<CameraChange> spTargetObject = m_wpTargetObject.lock();
	if (spTargetObject)
	{
		DebugWindow::CameraChangeParam debugParam = DebugWindow::Instance().GetCameraChangeParam();
		CameraChange::Param setParam{ debugParam.pos, debugParam.area, debugParam.targetPos };
		spTargetObject->SetParam(setParam);
	}

	//if (DebugWindow::Instance().GetCameraChangeMode() == true)
	//{
	//	if (spTargetObject)
	//	{
	//		spTargetObject->WriteDebugWire();
	//	}
	//	else
	//	{
	//		for (auto& obj : m_wpObjectList)
	//		{
	//			if (obj.expired() == false)
	//			{
	//				obj.lock()->WriteDebugWire();
	//			}
	//		}
	//	}
	//}

	// DELETEキーで削除する
	if (GetAsyncKeyState(VK_DELETE) & 0x8000)
	{
		DeleteObject();
	}
}

void CameraChangeController::Init()
{
	// CSVを読み込む
	CSVLoader();

	// 読み込んだデータからオブジェクトを作成する
	BeginCreateObject();
}

const KdGameObject::ObjectType CameraChangeController::GetObjectType() const
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

const std::string CameraChangeController::GetObjectName() const
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

void CameraChangeController::ConfirmedObject()
{
	std::shared_ptr<CameraChange> spTargetObject = m_wpTargetObject.lock();
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
			case ObjectType::CameraChange:
				// タイプのセット
				data.type = "CameraChange";
				// カウントを進める
				m_objectCount.CameraChange++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.CameraChange);
			}
			// 名前をセットする
			spTargetObject->SetObjectName(data.name);
			// 情報をセットする
			data.pos = spTargetObject->GetParam().pos;				// 座標
			data.area = spTargetObject->GetParam().area;			// 範囲
			data.targetPos = spTargetObject->GetParam().targetPos;// ターゲット座標
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
			m_dataList[num].pos = spTargetObject->GetParam().pos;	// 座標
			m_dataList[num].area = spTargetObject->GetParam().area;	// 触れれる範囲
			m_dataList[num].targetPos = spTargetObject->GetParam().targetPos;	// ターゲット座標
		}
	}

	m_wpTargetObject.reset();
}

void CameraChangeController::DeleteObject()
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
				break;
			}
		}
		// オブジェクトリストからも削除する
		for (int i = 0; i < (int)m_wpObjectList.size(); i++)
		{
			if (m_wpObjectList[i].lock()->GetObjectName() == m_wpTargetObject.lock()->GetObjectName())
			{
				m_wpObjectList.erase(m_wpObjectList.begin() + i);
			}
		}
		m_wpTargetObject.lock()->SetExpired(true);
	}
}

void CameraChangeController::CreateObject(KdGameObject::ObjectType _object)
{
	switch (_object)
	{
	case KdGameObject::ObjectType::CameraChange:
	{
		std::shared_ptr<CameraChange> object = std::make_shared<CameraChange>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		break;
	}
	}
}

void CameraChangeController::SetObject(std::weak_ptr<CameraChange> _wpTargetObject)
{
	// 確定
	ConfirmedObject();

	m_wpTargetObject = _wpTargetObject;

	CameraChange::Param param = m_wpTargetObject.lock()->GetParam();
	DebugWindow::CameraChangeParam setParam{ param.pos, param.area, param.targetPos };
	DebugWindow::Instance().SetCameraChangeParam(setParam);
}

void CameraChangeController::BeginCreateObject()
{
	for (auto& data : m_dataList)
	{
		// 箱
		if (data.type == "CameraChange")
		{
			std::shared_ptr<CameraChange> object = std::make_shared<CameraChange>();
			// パラメータをセットする
			CameraChange::Param setParam{ data.pos, data.area, data.targetPos };
			object->Init();
			object->SetParam(setParam);
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.CameraChange++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.CameraChange);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// リストに追加
			m_wpObjectList.push_back(object);
		}
	}
}

void CameraChangeController::CSVLoader()
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

			case 6:
				data.targetPos.x = stof(commaString);
				break;

			case 7:
				data.targetPos.y = stof(commaString);
				break;

			case 8:
				data.targetPos.z = stof(commaString);
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

void CameraChangeController::CSVWriter()
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

		// 範囲
		ofs << data.area << ",";

		// ターゲット座標
		ofs << data.targetPos.x << "," << data.targetPos.y << "," << data.targetPos.z << std::endl;
	}
}