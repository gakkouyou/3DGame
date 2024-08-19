﻿#include "TerrainController.h"
#include "../../../main.h"
#include "../../../Scene/SceneManager.h"
#include "../../../GameObject/Camera/TPSCamera/TPSCamera.h"
#include "../../DebugWindow/DebugWindow.h"

#include "../../../GameObject/Terrain/TerrainBase.h"
#include "../../../GameObject/Terrain/Ground/NormalGround/NormalGround.h"
#include "../../../GameObject/Terrain/Ground/BoundGround/BoundGround.h"
#include "../../../GameObject/Terrain/Ground/MoveGround/MoveGround.h"
#include "../../../GameObject/Terrain/Ground/RotationGround/RotationGround.h"
#include "../../../GameObject/Terrain/Object/Fence/Fence.h"
#include "../../../GameObject/Terrain/Object/HalfFence/HalfFence.h"

void TerrainController::Update()
{
	// 一度だけ実行する
	if (m_beginCreateFlg == false)
	{
		// 読み込んだデータからオブジェクトを作成する
		BeginCreateObject();
	}
	m_beginCreateFlg = true;

	// マウスでオブジェクトを選択する
	MouseSelect();

	// 対象のオブジェクト
	std::shared_ptr<TerrainBase> spTargetObject = m_wpTargetObject.lock();
	if (spTargetObject)
	{
		DebugWindow::TerrainParam debugParam = DebugWindow::Instance().GetTerrainParam();
		spTargetObject->SetParam(debugParam.startPos, debugParam.goalPos, debugParam.speed, debugParam.stayTime, debugParam.degAng);
	}

	// DELETEキーで削除する
	if (GetAsyncKeyState(VK_DELETE) & 0x8000)
	{
		DeleteObject();
	}
}

void TerrainController::Init()
{
	// CSVを読み込む
	CSVLoader();
}

void TerrainController::Reset()
{
	// 再配置
	m_beginCreateFlg = false;
}

const KdGameObject::ObjectType TerrainController::GetObjectType() const
{
	if (!m_wpTargetObject.expired())
	{
		return m_wpTargetObject.lock()->GetObjectType();
	}
}

const std::string TerrainController::GetObjectName() const
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

void TerrainController::ConfirmedObject()
{
	// 通常の床用または決定した後のオブジェクト
	std::shared_ptr<TerrainBase> spTargetObject = m_wpTargetObject.lock();
	if (spTargetObject)
	{
		// もし名前が決められてなかったら新たにdataListに追加する
		if (spTargetObject->GetObjectName() == "None")
		{
			Data data;
			// オブジェクトのタイプと名前を入れる
			switch (spTargetObject->GetObjectType())
			{
				// 通常の床の場合
			case ObjectType::NormalGround:
				// タイプのセット
				data.type = "NormalGround";
				// カウントを進める
				m_objectCount.NormalGround++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.NormalGround);
				break;

				// 跳ねる床の場合
			case ObjectType::BoundGround:
				// タイプのセット
				data.type = "BoundGround";
				// カウントを進める
				m_objectCount.BoundGround++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.BoundGround);
				break;

				// 動く床の場合
			case ObjectType::MoveGround:
				// タイプのセット
				data.type = "MoveGround";
				// カウントを進める
				m_objectCount.MoveGround++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.MoveGround);
				break;

				// 回る床の場合
			case ObjectType::RotationGround:
				// タイプのセット
				data.type = "RotationGround";
				// カウントを進める
				m_objectCount.RotationGround++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.RotationGround);
				break;

				// 柵の場合
			case ObjectType::Fence:
				// タイプのセット
				data.type = "Fence";
				// カウントを進める
				m_objectCount.Fence++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.Fence);
				break;

				// 片方柵の場合
			case ObjectType::HalfFence:
				// タイプのセット
				data.type = "HalfFence";
				// カウントを進める
				m_objectCount.HalfFence++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.HalfFence);
				break;
			}
			// 名前をセットする
			spTargetObject->SetObjectName(data.name);
			// 情報をセットする
			data.pos		= spTargetObject->GetParam().pos;		// 座標
			data.goalPos	= spTargetObject->GetParam().goalPos;	// ゴール座標
			data.speed		= spTargetObject->GetParam().speed;		// スピード
			data.stayTime	= spTargetObject->GetParam().stayTime;	// 待機時間
			data.degAng		= spTargetObject->GetParam().degAng;	// 回転角度
			// データが入っているリストにプッシュバックする
			m_dataList.push_back(data);
			// 地形のウィークポインタのリストにプッシュバックする
			m_wpTerrainList.push_back(spTargetObject);
		}
		// 決められていたら上書きする
		else
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
			m_dataList[num].pos		= spTargetObject->GetParam().startPos;	// 座標
			m_dataList[num].goalPos = spTargetObject->GetParam().goalPos;	// ゴール座標
			m_dataList[num].speed	= spTargetObject->GetParam().speed;		// スピード
			m_dataList[num].stayTime= spTargetObject->GetParam().stayTime;	// 待機時間
			m_dataList[num].degAng	= spTargetObject->GetParam().degAng;	// 回転角度
		}
	}

	m_wpTargetObject.reset();
}

void TerrainController::DeleteObject()
{
	// オブジェクトを削除する
	if (!m_wpTargetObject.expired())
	{
		// データ配列からも削除する
		for (int i = 0; i < m_dataList.size(); i++)
		{
			if (m_dataList[i].name == m_wpTargetObject.lock()->GetObjectName())
			{
				m_dataList.erase(m_dataList.begin() + i);
				break;
			}
		}
		m_wpTargetObject.lock()->SetExpired(true);
	}
}

void TerrainController::CreateObject(Object _object)
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
		m_wpTargetObject = object;
		break;
	}

	// 回る床
	case Object::RotationGround:
	{
		std::shared_ptr<RotationGround> object = std::make_shared<RotationGround>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		break;
	}

	// 柵
	case Object::Fence:
	{
		std::shared_ptr<Fence> object = std::make_shared<Fence>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		break;
	}

	// 片方柵
	case Object::HalfFence:
	{
		std::shared_ptr<HalfFence> object = std::make_shared<HalfFence>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		break;
	}
	}
}

void TerrainController::BeginCreateObject()
{
	for (auto& data : m_dataList)
	{
		// 通常の床
		if (data.type == "NormalGround")
		{
			std::shared_ptr<NormalGround> object = std::make_shared<NormalGround>();
			object->Init();
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.NormalGround++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.NormalGround);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// 座標をセットする
			object->SetParam(data.pos);
			// リストに追加
			m_wpTerrainList.push_back(object);
		}
		// 跳ねる床
		else if (data.type == "BoundGround")
		{
			std::shared_ptr<BoundGround> object = std::make_shared<BoundGround>();
			object->Init();
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.BoundGround++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.BoundGround);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// 座標をセットする
			object->SetParam(data.pos);
			m_wpTerrainList.push_back(object);
		}
		// 動く床
		else if (data.type == "MoveGround")
		{
			std::shared_ptr<MoveGround> object = std::make_shared<MoveGround>();
			object->Init();
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.MoveGround++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.MoveGround);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// 情報をセットする
			object->SetParam(data.pos, data.goalPos, data.speed, data.stayTime);
			m_wpTerrainList.push_back(object);
		}
		// 回る床
		else if (data.type == "RotationGround")
		{
			std::shared_ptr<RotationGround> object = std::make_shared<RotationGround>();
			object->Init();
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.RotationGround++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.RotationGround);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// 情報をセットする
			object->SetParam(data.pos, Math::Vector3::Zero, 0, 0, data.degAng);
			m_wpTerrainList.push_back(object);
		}
		// 柵
		else if (data.type == "Fence")
		{
			std::shared_ptr<Fence> object = std::make_shared<Fence>();
			object->Init();
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.Fence++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.Fence);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// 座標をセットする
			object->SetParam(data.pos, Math::Vector3::Zero, 0, 0, data.degAng);
			// リストに追加
			m_wpTerrainList.push_back(object);
		}
		// 片方の柵
		else if (data.type == "HalfFence")
		{
			std::shared_ptr<HalfFence> object = std::make_shared<HalfFence>();
			object->Init();
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.HalfFence++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.HalfFence);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// 座標をセットする
			object->SetParam(data.pos, Math::Vector3::Zero, 0, 0, data.degAng);
			// リストに追加
			m_wpTerrainList.push_back(object);
		}
	}
}

void TerrainController::CSVLoader()
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
				break;

			case 10:
				data.degAng.x = stof(commaString);
				break;

			case 11:
				data.degAng.y = stof(commaString);
				break;

			case 12:
				data.degAng.z = stof(commaString);
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

void TerrainController::CSVWriter()
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

		// ゴール座標
		ofs << data.goalPos.x << "," << data.goalPos.y << "," << data.goalPos.z << ",";

		// スピード
		ofs << data.speed << ",";

		// 待機時間
		ofs << data.stayTime << ",";

		// 回転角度
		ofs << data.degAng.x << "," << data.degAng.y << "," << data.degAng.z << std::endl;
	}
}

void TerrainController::MouseSelect()
{
	// マウスでオブジェクトを選択する
	std::shared_ptr<const CameraBase> spCamera = m_wpCamera.lock();

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
		std::vector<std::weak_ptr<TerrainBase>> hitObjList;

		// 当たり判定
		for (auto& obj : m_wpTerrainList)
		{
			if(!obj.expired())
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

				TerrainBase::Param param = m_wpTargetObject.lock()->GetParam();
				DebugWindow::TerrainParam setParam{ param.startPos, param.goalPos, param.speed, param.stayTime, param.degAng };
				DebugWindow::Instance().SetTerrainParam(setParam);
			}
			cnt++;
		}
	}
}
