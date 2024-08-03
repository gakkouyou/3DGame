#include "ObjectController.h"
#include "../../main.h"
#include "../../Scene/SceneManager.h"
#include "../../GameObject/Camera/TPSCamera/TPSCamera.h"
#include "../DebugWindow/DebugWindow.h"

#include "../../GameObject/Terrain/BaseTerrain.h"
#include "../../GameObject/Terrain/Ground/NormalGround/NormalGround.h"
#include "../../GameObject/Terrain/Ground/BoundGround/BoundGround.h"
#include "../../GameObject/Terrain/Ground/MoveGround/MoveGround.h"

void ObjectController::Update()
{
	// 一度だけ実行する
	if (beginCreateFlg == false)
	{
		// 読み込んだデータからオブジェクトを作成する
		BeginCreateObject();
	}
	beginCreateFlg = true;

	// マウスでオブジェクトを選択する
	MouseSelect();

	// 対象のオブジェクトが動く床の場合
	std::shared_ptr<MoveGround> spMoveGround = m_wpMoveGround.lock();
	if (spMoveGround)
	{
		// 情報セット
		spMoveGround->SetInfo(DebugWindow::Instance().GetMoveObjectInfo().startPos, DebugWindow::Instance().GetMoveObjectInfo().goalPos, DebugWindow::Instance().GetMoveObjectInfo().speed, DebugWindow::Instance().GetMoveObjectInfo().stayTime);

		// 止める
		spMoveGround->SetStopFlg(true);

		
	}
	// 通常の場合
	else
	{
		// 対象のオブジェクト
		std::shared_ptr<KdGameObject> spTargetObject = m_wpTargetObject.lock();
		if (spTargetObject)
		{
			// 座標セット
			spTargetObject->SetPos(DebugWindow::Instance().GetPos());
		}
	}
	// DELETEキーで削除する
	if (GetAsyncKeyState(VK_DELETE) & 0x8000)
	{
		DeleteObject();
	}

	// 動く床は止める
	if (!m_wpMoveGround.expired())
	{
		m_wpMoveGround.lock()->SetStopFlg(true);
	}
}

void ObjectController::Init()
{
	// CSVを読み込む
	CSVLoader();
}

const KdGameObject::ObjectType ObjectController::GetObjectType() const
{
	if (!m_wpTargetObject.expired())
	{
		return m_wpTargetObject.lock()->GetObjectType();
	}
}

void ObjectController::ConfirmObject()
{
	// 動く床用
	std::shared_ptr<MoveGround> spMoveGround = m_wpMoveGround.lock();
	if (spMoveGround)
	{
		// もし名前が決められてなかったら新たにdataListに追加する
		if (spMoveGround->GetObjectName() == "None")
		{
			MoveData data;
			// オブジェクトのタイプと名前を入れる
			switch (spMoveGround->GetObjectType())
			{
			case ObjectType::MoveGround:
				// タイプのセット
				data.type = "MoveGround";
				// カウントを進める
				m_objectCount.MoveGround++;
				// 名前のセット
				data.name = data.type + std::to_string(m_objectCount.MoveGround);
				break;
			}

			// 名前をセットする
			spMoveGround->SetObjectName(data.name);

			// スタート座標を入れる
			data.startPos = spMoveGround->GetInfo().startPos;

			// ゴール座標をセットする
			data.goalPos = spMoveGround->GetInfo().goalPos;

			// スピードをセットする
			data.speed = spMoveGround->GetInfo().speed;

			// 待機時間をセットする
			data.stayTime = spMoveGround->GetInfo().stayTime;

			m_moveDataList.push_back(data);
		}
		// 決められていたら上書きする
		else
		{
			// 何個目に上書きするかを格納する変数
			int num;
			for (int i = 0; i < m_moveDataList.size(); i++)
			{
				if (m_moveDataList[i].name == spMoveGround->GetObjectName())
				{
					num = i;
					break;
				}
			}
			// スタート座標を入れる
			m_moveDataList[num].startPos = spMoveGround->GetInfo().startPos;

			// ゴール座標をセットする
			m_moveDataList[num].goalPos = spMoveGround->GetInfo().goalPos;

			// スピードをセットする
			m_moveDataList[num].speed = spMoveGround->GetInfo().speed;

			// 待機時間をセットする
			m_moveDataList[num].stayTime = spMoveGround->GetInfo().stayTime;
		}
		// 止まるフラグをオフにする
		spMoveGround->SetStopFlg(false);
	}
	// 通常の床用または決定した後のオブジェクト
	else
	{
		std::shared_ptr<KdGameObject> spTargetObject = m_wpTargetObject.lock();
		if (spTargetObject)
		{
			// もし名前が決められてなかったら新たにdataListに追加する
			if (spTargetObject->GetObjectName() == "None")
			{
				Data data;
				MoveData moveData;
				// オブジェクトのタイプと名前を入れる
				switch (spTargetObject->GetObjectType())
				{
				case ObjectType::NormalGround:
					// タイプのセット
					data.type = "NormalGround";
					// カウントを進める
					m_objectCount.NormalGround++;
					// 名前のセット
					data.name = data.type + std::to_string(m_objectCount.NormalGround);
					// 名前をセットする
					spTargetObject->SetObjectName(data.name);
					// 座標を入れる
					data.pos = spTargetObject->GetPos();
					m_dataList.push_back(data);
					break;

				case ObjectType::BoundGround:
					// タイプのセット
					data.type = "BoundGround";
					// カウントを進める
					m_objectCount.BoundGround++;
					// 名前のセット
					data.name = data.type + std::to_string(m_objectCount.BoundGround);
					// 名前をセットする
					spTargetObject->SetObjectName(data.name);
					// 座標を入れる
					data.pos = spTargetObject->GetPos();
					m_dataList.push_back(data);
					break;
				}
			}
			// 決められていたら上書きする
			else
			{
				if (spTargetObject->GetObjectType() != ObjectType::MoveGround)
				{
					// 何個目に上書きするかを格納する変数
					int num;
					for (int i = 0; i < m_dataList.size(); i++)
					{
						if (m_dataList[i].name == spTargetObject->GetObjectName())
						{
							num = i;
							break;
						}
					}
					m_dataList[num].pos = spTargetObject->GetPos();
				}
			}
		}
	}

	m_wpMoveGround.reset();
	m_wpTargetObject.reset();
}

void ObjectController::DeleteObject()
{
	if (!m_wpTargetObject.expired())
	{
		// 配列からも削除する
		// 動く系だった場合
		if (m_wpTargetObject.lock()->GetObjectType() == ObjectType::MoveGround)
		{
			for (int i = 0; i < m_moveDataList.size(); i++)
			{
				if (m_moveDataList[i].name == m_wpTargetObject.lock()->GetObjectName())
				{
					m_moveDataList.erase(m_moveDataList.begin() + i);
				}
			}
		}
		// 通常だった場合
		else
		{
			for (int i = 0; i < m_dataList.size(); i++)
			{
				if (m_dataList[i].name == m_wpTargetObject.lock()->GetObjectName())
				{
					m_dataList.erase(m_dataList.begin() + i);
				}
			}
		}

		m_wpTargetObject.lock()->SetExpired(true);
	}
}

void ObjectController::CreateObject(Object _object)
{
	switch (_object)
	{
		// 通常の床
		case Object::NormalGround:
		{
			std::shared_ptr<NormalGround> object = std::make_shared<NormalGround>();
			object->Init();
			SceneManager::Instance().AddObject(object);
			m_wpTargetObject = object;
			break;
		}

		// 跳ねる床
		case Object::BoundGround:
		{
			std::shared_ptr<BoundGround> object = std::make_shared<BoundGround>();
			object->Init();
			SceneManager::Instance().AddObject(object);
			m_wpTargetObject = object;
			break;
		}

		// 動く床
		case Object::MoveGround:
		{
			std::shared_ptr<MoveGround> object = std::make_shared<MoveGround>();
			object->Init();
			SceneManager::Instance().AddObject(object);
			m_wpMoveGround = object;
			m_wpTargetObject = object;
			break;
		}
	}
}

void ObjectController::BeginCreateObject()
{
	for (auto& data : m_dataList)
	{
		// 通常の床
		if (data.type == "NormalGround")
		{
			std::shared_ptr<NormalGround> object = std::make_shared<NormalGround>();
			object->Init();
			SceneManager::Instance().AddObject(object);
			// 名前をセットする
			object->SetObjectName(data.name);
			// 座標をセットする
			object->SetPos(data.pos);
			// カウントを進める
			m_objectCount.NormalGround++;
		}
		// 跳ねる床
		else if (data.type == "BoundGround")
		{
			std::shared_ptr<BoundGround> object = std::make_shared<BoundGround>();
			object->Init();
			SceneManager::Instance().AddObject(object);
			// 名前をセットする
			object->SetObjectName(data.name);
			// 座標をセットする
			object->SetPos(data.pos);
			// カウントを進める
			m_objectCount.BoundGround++;
		}
	}

	for (auto& data : m_moveDataList)
	{
		// 通常の床
		if (data.type == "MoveGround")
		{
			std::shared_ptr<MoveGround> object = std::make_shared<MoveGround>();
			object->Init();
			SceneManager::Instance().AddObject(object);
			// 名前をセットする
			object->SetObjectName(data.name);
			// 座標をセットする
			object->SetInfo(data.startPos, data.goalPos, data.speed, data.stayTime);
			// カウントを進める
			m_objectCount.MoveGround++;
		}
	}
}

void ObjectController::CSVLoader()
{
	// 通常の地形
	{
		std::ifstream ifs("Asset/Data/CSV/Terrain.csv");
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
	// 動く地形用
	{
		std::ifstream ifs("Asset/Data/CSV/MoveTerrain.csv");
		if (!ifs.is_open())
		{
			return;
		}

		m_moveDataList.clear();

		std::string lineString;

		while (std::getline(ifs, lineString))
		{
			std::istringstream iss(lineString);
			std::string commaString;

			int cnt = 0;
			// 空かどうかを確認するフラグ
			bool emptyFlg = true;
			MoveData data;
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
					data.startPos.x = stof(commaString);
					break;

				case 3:
					data.startPos.y = stof(commaString);
					break;

				case 4:
					data.startPos.z = stof(commaString);
					break;

				case 5:
					data.goalPos.x = stof(commaString);
					break;

				case 6:
					data.goalPos.y = stof(commaString);
					break;

				case 7:
					data.goalPos.z = stof(commaString);
					break;

				case 8:
					data.speed = stof(commaString);
					break;

				case 9:
					data.stayTime = stoi(commaString);
				}
				cnt++;
			}
			// 空じゃなかった時だけpush_backする
			if (emptyFlg == false)
			{
				m_moveDataList.push_back(data);
			}
		}
		ifs.close();
	}
}

void ObjectController::CSVWriter()
{
	// 通常の床
	{
		std::ofstream ofs("Asset/Data/CSV/Terrain.csv");

		for (auto& data : m_dataList)
		{
			// オブジェクトのタイプ
			ofs << data.type << ",";

			// オブジェクトの名前
			ofs << data.name << ",";

			// 座標
			Math::Vector3 pos = data.pos;

			//書き込み
			ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
		}
	}
	// 動く床
	{
		std::ofstream ofs("Asset/Data/CSV/MoveTerrain.csv");

		for (auto& data : m_moveDataList)
		{
			// オブジェクトのタイプ
			ofs << data.type << ",";

			// オブジェクトの名前
			ofs << data.name << ",";

			// 座標
			Math::Vector3 pos = data.startPos;

			//書き込み
			ofs << pos.x << "," << pos.y << "," << pos.z << ",";

			// ゴール座標
			pos = data.goalPos;

			// 書き込み
			ofs << pos.x << "," << pos.y << "," << pos.z << ",";

			// スピード
			ofs << data.speed << ",";

			// 待機時間
			ofs << data.stayTime << std::endl;
		}
	}
}

void ObjectController::MouseSelect()
{
	// マウスでオブジェクトを選択する
	std::shared_ptr<const TPSCamera> spCamera = m_wpCamera.lock();

	// カメラが無かったら終了
	if (!spCamera) return;

	// クリックしたら選んだオブジェクトをセットする
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
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
		std::vector<std::weak_ptr<KdGameObject>> hitObjList;

		// 当たり判定
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			if (obj->Intersects(rayInfo, &resultList))
			{
				hitObjList.push_back(obj);
				// １回でも当たったらリセット
				ConfirmObject();
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
				DebugWindow::Instance().SetPos(m_wpTargetObject.lock()->GetPos());
			}
			cnt++;
		}
	}
}
