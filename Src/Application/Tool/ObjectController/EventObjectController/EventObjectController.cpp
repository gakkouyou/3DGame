#include "EventObjectController.h"
#include "../../DebugWindow/DebugWindow.h"
#include "../../../Scene/SceneManager.h"
#include "../../../main.h"
#include "../../../GameObject/Camera/TPSCamera/TPSCamera.h"
#include "../../../GameObject/Character/Player/Player.h"

#include "../../../GameObject/EventObject/Goal/Goal.h"
#include "../../../GameObject/EventObject/HealItem/HealItem.h"
#include "../../../GameObject/EventObject/Candy/Candy.h"

void EventObjectController::Update()
{
	// マウスでオブジェクトを選択する
	MouseSelect();

	// 対象のオブジェクト
	std::shared_ptr<KdGameObject> spTargetObject = m_wpTargetObject.lock();
	if (spTargetObject)
	{
		DebugWindow::EventObjectParam debugParam = DebugWindow::Instance().GetEventObjectParam();
		Math::Vector3 pos = debugParam.pos;
		spTargetObject->SetPos(pos);
	}

	// DELETEキーで削除する
	if (GetAsyncKeyState(VK_DELETE) & 0x8000)
	{
		DeleteObject();
	}
}

void EventObjectController::Init()
{
	// CSVを読み込む
	CSVLoader();

	// 読み込んだデータからオブジェクトを作成する
	BeginCreateObject();
}

void EventObjectController::Reset()
{
	m_objectCount = m_objectCountReset;
	m_wpObjectList.clear();

	// 読み込んだデータからオブジェクトを作成する
	BeginCreateObject();
}

const KdGameObject::ObjectType EventObjectController::GetObjectType() const
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

const std::string EventObjectController::GetObjectName() const
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

void EventObjectController::ConfirmedObject()
{
	std::shared_ptr<KdGameObject> spTargetObject = m_wpTargetObject.lock();
	if (spTargetObject)
	{
		// もし名前が決められてなかったら新たにdataListに追加する
		if (spTargetObject->GetObjectName() == "None")
		{
			Data data;
			// オブジェクトのタイプと名前を入れる
			switch (spTargetObject->GetObjectType())
			{
				// ゴールの場合
			case ObjectType::Goal:
				// タイプのセット
				data.type = "Goal";
				// カウントを進める
				m_objectCount.Goal++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.Goal);
				break;

				// 回復アイテムの場合
			case ObjectType::HealItem:
				// タイプのセット
				data.type = "HealItem";
				// カウントを進める
				m_objectCount.HealItem++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.HealItem);
				break;

				// キャンディーの場合
			case ObjectType::Candy:
				// タイプのセット
				data.type = "Candy";
				// カウントを進める
				m_objectCount.Candy++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.Candy);
				break;
			}
			// 名前をセットする
			spTargetObject->SetObjectName(data.name);
			// 情報をセットする
			data.pos = spTargetObject->GetPos();	// 座標
			// プレイヤーとカメラにゴールの座標を渡す
			if (!m_wpPlayer.expired())
			{
				m_wpPlayer.lock()->SetGoalPos(data.pos);
			}
			if (!m_wpCamera.expired())
			{
				m_wpCamera.lock()->SetGoalPos(data.pos);
			}
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
			m_dataList[num].pos = spTargetObject->GetPos();	// 座標
			// プレイヤーとカメラにゴールの座標を渡す
			if (!m_wpPlayer.expired())
			{
				m_wpPlayer.lock()->SetGoalPos(m_dataList[num].pos);
			}
			if (!m_wpCamera.expired())
			{
				m_wpCamera.lock()->SetGoalPos(m_dataList[num].pos);
			}
		}
	}

	m_wpTargetObject.reset();
}

void EventObjectController::DeleteObject()
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

void EventObjectController::CreateObject(KdGameObject::ObjectType _object)
{
	switch (_object)
	{
	// ゴール
	case KdGameObject::ObjectType::Goal:
	{
		std::shared_ptr<Goal> object = std::make_shared<Goal>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		break;
	}

	// 回復アイテム
	case KdGameObject::ObjectType::HealItem:
	{
		std::shared_ptr<HealItem> object = std::make_shared<HealItem>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		break;
	}

	// キャンディー
	case KdGameObject::ObjectType::Candy:
	{
		std::shared_ptr<Candy> object = std::make_shared<Candy>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		break;
	}
	}
}

void EventObjectController::MouseSelect()
{
	if (SceneManager::Instance().GetDebug() == false) return;

	// マウスでオブジェクトを選択する
	std::shared_ptr<const TPSCamera> spCamera = m_wpCamera.lock();

	// カメラが無かったら終了
	if (!spCamera) return;

	// クリックしたら選んだオブジェクトをセットする
	if (GetAsyncKeyState('E') & 0x8000)
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

		KdCollider::RayInfo rayInfo(KdCollider::TypeEvent, cameraPos, endRayPos);

		// 当たり判定の結果
		std::list<KdCollider::CollisionResult> resultList;

		// 当たったオブジェクトのリスト
		std::vector<std::weak_ptr<KdGameObject>> hitObjList;

		// 当たり判定
		for (auto& obj : m_wpObjectList)
		{
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

				Math::Vector3 pos = m_wpTargetObject.lock()->GetPos();
				DebugWindow::EventObjectParam setParam{ pos };
				DebugWindow::Instance().SetEventObjectParam(setParam);
			}
			cnt++;
		}
	}
}

void EventObjectController::BeginCreateObject()
{
	for (auto& data : m_dataList)
	{
		// ゴール
		if (data.type == "Goal")
		{
			std::shared_ptr<Goal> object = std::make_shared<Goal>();
			// パラメータをセットする
			Math::Vector3 pos = data.pos;
			object->SetPos(pos);
			object->Init();
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.Goal++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.Goal);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// プレイヤーとカメラにゴールの座標を渡す
			if (!m_wpPlayer.expired())
			{
				m_wpPlayer.lock()->SetGoalPos(pos);
			}
			if (!m_wpCamera.expired())
			{
				m_wpCamera.lock()->SetGoalPos(pos);
			}
			// リストに追加
			m_wpObjectList.push_back(object);
		}
		// 回復アイテム
		else if (data.type == "HealItem")
		{
			std::shared_ptr<HealItem> object = std::make_shared<HealItem>();
			// パラメータをセットする
			Math::Vector3 pos = data.pos;
			object->SetPos(pos);
			object->Init();
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.HealItem++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.HealItem);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// リストに追加
			m_wpObjectList.push_back(object);
		}
		// キャンディー
		else if (data.type == "Candy")
		{
			std::shared_ptr<Candy> object = std::make_shared<Candy>();
			// パラメータをセットする
			Math::Vector3 pos = data.pos;
			object->SetPos(pos);
			object->Init();
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.HealItem++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.HealItem);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// リストに追加
			m_wpObjectList.push_back(object);
		}
	}
}

void EventObjectController::CSVLoader()
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

void EventObjectController::CSVWriter()
{
	std::ofstream ofs(m_fileName);

	for (auto& data : m_dataList)
	{
		// オブジェクトのタイプ
		ofs << data.type << ",";

		// オブジェクトの名前
		ofs << data.name << ",";

		// 座標
		ofs << data.pos.x << "," << data.pos.y << "," << data.pos.z << "," << std::endl;
	}
}