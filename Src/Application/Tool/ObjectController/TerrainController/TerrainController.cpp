#include "TerrainController.h"
#include "../../../main.h"
#include "../../../Scene/SceneManager.h"
#include "../../../GameObject/Camera/TPSCamera/TPSCamera.h"
#include "../../DebugWindow/DebugWindow.h"

#include "../../../GameObject/Terrain/TerrainBase.h"
#include "../../../GameObject/Terrain/Ground/NormalGround/NormalGround.h"
#include "../../../GameObject/Terrain/Ground/BoundGround/BoundGround.h"
#include "../../../GameObject/Terrain/Ground/MoveGround/MoveGround.h"
#include "../../../GameObject/Terrain/Ground/RotationGround/RotationGround.h"
#include "../../../GameObject/Terrain/Object/FenceBar/FenceBar.h"
#include "../../../GameObject/Terrain/Object/FencePillar/FencePillar.h"
#include "../../../GameObject/Terrain/Ground/DropGround/DropGround.h"
#include "../../../GameObject/Terrain/Object/Propeller/Propeller.h"
#include "../../../GameObject/Terrain/Object/Switch/Switch.h"
#include "../../../GameObject/Terrain/Object/Switch/SwitchBaseModel/SwitchBaseModel.h"
#include "../../../GameObject/Terrain/Object/Door/RightDoor/RightDoor.h"
#include "../../../GameObject/Terrain/Object/Door/LeftDoor/LeftDoor.h"
#include "../../../GameObject/Terrain/Object/Door/DoorWall/DoorWall.h"
#include "../../../GameObject/Terrain/Ground/SlopeGround/SlopeGround.h"
#include "../../../GameObject/Terrain/Object/TransparentWall/TransparentWall.h"

void TerrainController::Update()
{
	// 一度だけ実行する
	if (m_beginCreateFlg == false)
	{
		//BeginCreateObject();
	}
	m_beginCreateFlg = true;

	// マウスでオブジェクトを選択する
	MouseSelect();

	// 対象のオブジェクト
	std::shared_ptr<TerrainBase> spTargetObject = m_wpTargetObject.lock();
	if (spTargetObject)
	{
		DebugWindow::TerrainParam debugParam = DebugWindow::Instance().GetTerrainParam();
		TerrainBase::Param param;
		param.startPos		= debugParam.startPos;	// 最初の座標
		param.goalPos		= debugParam.goalPos;	// ゴールの座標
		param.scale			= debugParam.scale;		// 拡縮
		param.speed			= debugParam.speed;		// スピード
		param.stayTime		= debugParam.stayTime;	// 待機時間
		param.degAng		= debugParam.degAng;	// 角度
		param.targetName	= debugParam.targetName;// ターゲットのオブジェクトの名前
		
		spTargetObject->SetParam(param);
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

	// 読み込んだデータからオブジェクトを作成する
	BeginCreateObject();
}

void TerrainController::Reset()
{
	//m_objectCount = m_objectCountReset;
	//m_wpTerrainList.clear();

	// 読み込んだデータからオブジェクトを作成する
	//BeginCreateObject();
}

const KdGameObject::ObjectType TerrainController::GetObjectType() const
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

const std::string TerrainController::GetObjectName() const
{
	if (!m_wpTargetObject.expired())
	{
		if (m_targetFlg == false)
		{
			return m_wpTargetObject.lock()->GetObjectName();
		}
		else
		{
			return m_wpTargetObject.lock()->GetObjectName() + " TargetSelect";
		}
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

				// 柵の柱の場合
			case ObjectType::FencePillar:
				// タイプのセット
				data.type = "FencePillar";
				// カウントを進める
				m_objectCount.FencePillar++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.FencePillar);
				break;

				// 柵の棒の場合
			case ObjectType::FenceBar:
				// タイプのセット
				data.type = "FenceBar";
				// カウントを進める
				m_objectCount.FenceBar++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.FenceBar);
				break;

				// 落ちる床の場合
			case ObjectType::DropGround:
				// タイプのセット
				data.type = "DropGround";
				// カウントを進める
				m_objectCount.DropGround++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.DropGround);
				break;

				// プロペラの場合
			case ObjectType::Propeller:
				// タイプのセット
				data.type = "Propeller";
				// カウントを進める
				m_objectCount.Propeller++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.Propeller);
				break;

				// スイッチの場合
			case ObjectType::Switch:
				// タイプのセット
				data.type = "Switch";
				// カウントを進める
				m_objectCount.Switch++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.Switch);
				break;

				// ドアの場合
			case ObjectType::Door:
				// タイプのセット
				data.type = "Door";
				// カウントを進める
				m_objectCount.Door++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.Door);
				break;

				// 坂の床の場合
			case ObjectType::SlopeGround:
				// タイプのセット
				data.type = "SlopeGround";
				// カウントを進める
				m_objectCount.SlopeGround++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.SlopeGround);
				break;

				// 透明な壁の場合
			case ObjectType::TransparentWall:
				// タイプのセット
				data.type = "TransparentWall";
				// カウントを進める
				m_objectCount.TransparentWall++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.TransparentWall);
				break;
			}
			// 名前をセットする
			spTargetObject->SetObjectName(data.name);
			// 情報をセットする
			data.pos		= spTargetObject->GetParam().pos;			// 座標
			data.goalPos	= spTargetObject->GetParam().goalPos;		// ゴール座標
			data.scale		= spTargetObject->GetParam().scale;			// 拡縮
			data.speed		= spTargetObject->GetParam().speed;			// スピード
			data.stayTime	= spTargetObject->GetParam().stayTime;		// 待機時間
			data.degAng		= spTargetObject->GetParam().degAng;		// 回転角度
			data.targetName = spTargetObject->GetParam().targetName;	// ターゲットのオブジェクトの名前
			data.yetActive	= spTargetObject->GetParam().yetActive;		// すでにアクティブかどうか
			// データが入っているリストにプッシュバックする
			m_dataList.push_back(data);
			// 地形のウィークポインタのリストにプッシュバックする
			m_wpTerrainList.push_back(spTargetObject);
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
			m_dataList[num].pos			= spTargetObject->GetParam().startPos;	// 座標
			m_dataList[num].goalPos		= spTargetObject->GetParam().goalPos;	// ゴール座標
			m_dataList[num].scale		= spTargetObject->GetParam().scale;		// 拡縮
			m_dataList[num].speed		= spTargetObject->GetParam().speed;		// スピード
			m_dataList[num].stayTime	= spTargetObject->GetParam().stayTime;	// 待機時間
			m_dataList[num].degAng		= spTargetObject->GetParam().degAng;	// 回転角度
			m_dataList[num].targetName	= spTargetObject->GetParam().targetName;// ターゲットの名前
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
		for (int i = 0; i < (int)m_dataList.size(); i++)
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

void TerrainController::CreateObject(KdGameObject::ObjectType _object)
{
	switch (_object)
	{
		// 通常の床
	case ObjectType::NormalGround:
	{
		std::shared_ptr<NormalGround> object = std::make_shared<NormalGround>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		break;
	}

	// 跳ねる床
	case ObjectType::BoundGround:
	{
		std::shared_ptr<BoundGround> object = std::make_shared<BoundGround>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		break;
	}

	// 動く床
	case ObjectType::MoveGround:
	{
		std::shared_ptr<MoveGround> object = std::make_shared<MoveGround>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		break;
	}

	// 回る床
	case ObjectType::RotationGround:
	{
		std::shared_ptr<RotationGround> object = std::make_shared<RotationGround>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		break;
	}

	// 柵の柱
	case ObjectType::FencePillar:
	{
		std::shared_ptr<FencePillar> object = std::make_shared<FencePillar>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		break;
	}

	// 柵の棒
	case ObjectType::FenceBar:
	{
		std::shared_ptr<FenceBar> object = std::make_shared<FenceBar>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		break;
	}

	// 落ちる床
	case ObjectType::DropGround:
	{
		std::shared_ptr<DropGround> object = std::make_shared<DropGround>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		break;
	}

	// プロペラ
	case ObjectType::Propeller:
	{
		std::shared_ptr<Propeller> object = std::make_shared<Propeller>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		break;
	}

	// スイッチ
	case ObjectType::Switch:
	{
		std::shared_ptr<Switch> object = std::make_shared<Switch>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		// スイッチベースモデル
		std::weak_ptr<SwitchBaseModel> switchBaseModel = object->GetSwitchBaseModel();
		// スイッチベースモデルにスイッチを渡す
		if (switchBaseModel.expired() == false)
		{
			switchBaseModel.lock()->SetSwitch(object);
			// スイッチのベースのモデルもpush_backする
			m_wpTerrainList.push_back(switchBaseModel);
		}
		break;
	}

	// ドア
	case ObjectType::Door:
	{
		std::shared_ptr<RightDoor> object = std::make_shared<RightDoor>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		// 左のドア
		std::weak_ptr<LeftDoor> leftDoor = object->GetLeftDoor();
		// ドアの壁
		std::weak_ptr<DoorWall> doorWall = object->GetDoorWall();
		// それぞれにそれぞれを渡しあう
		if (leftDoor.expired() == false && doorWall.expired() == false)
		{
			leftDoor.lock()->SetRightDoor(object);
			leftDoor.lock()->SetDoorWall(doorWall);

			doorWall.lock()->SetLeftDoor(leftDoor);
			doorWall.lock()->SetRightDoor(object);
		}
		break;
	}

	// 坂
	case ObjectType::SlopeGround:
	{
		std::shared_ptr<SlopeGround> object = std::make_shared<SlopeGround>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		break;
	}

	// 透明な壁
	case ObjectType::TransparentWall:
	{
		std::shared_ptr<TransparentWall> object = std::make_shared<TransparentWall>();
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
		TerrainBase::Param param;
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
			// パラメータセット
			param.startPos = data.pos;	// 座標
			param.scale = data.scale;	// 拡縮
			// 座標をセットする
			object->SetParam(param);
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
			// パラメータセット
			param.startPos = data.pos;	// 座標
			param.scale = data.scale;	// 拡縮
			// 座標をセットする
			object->SetParam(param);
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
			// パラメータセット
			param.startPos = data.pos;		// 最初の座標
			param.goalPos = data.goalPos;	// ゴール座標
			param.scale = data.scale;		// 拡縮
			param.speed = data.speed;		// スピード
			param.stayTime = data.stayTime;	// 待機時間
			// 情報をセットする
			object->SetParam(param);
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
			// パラメータセット
			param.startPos = data.pos;	// 座標
			param.scale = data.scale;	// 拡縮
			param.degAng = data.degAng;	// 角度
			// 情報をセットする
			object->SetParam(param);
			m_wpTerrainList.push_back(object);
		}
		// 柵の柱
		else if (data.type == "FencePillar")
		{
			std::shared_ptr<FencePillar> object = std::make_shared<FencePillar>();
			object->Init();
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.FencePillar++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.FencePillar);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// パラメータセット
			param.startPos = data.pos;	// 座標
			param.scale = data.scale;	// 拡縮
			param.degAng = data.degAng;	// 角度
			// パラメータをセットする
			object->SetParam(param);
			// リストに追加
			m_wpTerrainList.push_back(object);
		}
		// 片方の柵
		else if (data.type == "FenceBar")
		{
			std::shared_ptr<FenceBar> object = std::make_shared<FenceBar>();
			object->Init();
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.FenceBar++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.FenceBar);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// パラメータセット
			param.startPos = data.pos;	// 座標
			param.scale = data.scale;	// 拡縮
			param.degAng = data.degAng;	// 角度
			// パラメータをセットする
			object->SetParam(param);
			// リストに追加
			m_wpTerrainList.push_back(object);
		}
		// 落ちる床
		else if (data.type == "DropGround")
		{
			std::shared_ptr<DropGround> object = std::make_shared<DropGround>();
			object->Init();
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.DropGround++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.DropGround);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// パラメータセット
			param.startPos = data.pos;		// 座標
			param.scale = data.scale;		// 拡縮
			param.speed = data.speed;		// スピード
			param.stayTime = data.stayTime;	// 待機時間
			// 情報をセットする
			object->SetParam(param);
			m_wpTerrainList.push_back(object);
		}
		// プロペラ
		else if (data.type == "Propeller")
		{
			std::shared_ptr<Propeller> object = std::make_shared<Propeller>();
			object->Init();
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.Propeller++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.Propeller);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// パラメータセット
			param.startPos = data.pos;		// 座標
			param.scale = data.scale;		// 拡縮
			param.degAng = data.degAng;		// 回転角度
			// 情報をセットする
			object->SetParam(param);
			m_wpTerrainList.push_back(object);
		}
		// スイッチ
		else if (data.type == "Switch")
		{
			std::shared_ptr<Switch> object = std::make_shared<Switch>();
			object->Init();
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.Switch++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.Switch);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// パラメータセット
			param.startPos = data.pos;			// 座標
			param.scale = data.scale;			// 拡縮
			param.targetName = data.targetName;	// ターゲットの名前
			// 情報をセットする
			object->SetParam(param);
			m_wpTerrainList.push_back(object);
			// スイッチベースモデル
			std::weak_ptr<SwitchBaseModel> switchBaseModel = object->GetSwitchBaseModel();
			// スイッチベースモデルにスイッチを渡す
			if (switchBaseModel.expired() == false)
			{
				switchBaseModel.lock()->SetSwitch(object);
				// 名前
				switchBaseModel.lock()->SetObjectName(name);
				// スイッチのベースのモデルもpush_backする
				m_wpTerrainList.push_back(switchBaseModel);
			}
		}
		// ドア
		else if (data.type == "Door")
		{
			std::shared_ptr<RightDoor> object = std::make_shared<RightDoor>();
			object->Init();
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.Door++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.Door);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// パラメータセット
			param.startPos = data.pos;			// 座標
			param.scale = data.scale;			// 拡縮
			// 情報をセットする
			object->SetParam(param);
			m_wpTerrainList.push_back(object);
			// 左のドア
			std::weak_ptr<LeftDoor> leftDoor = object->GetLeftDoor();
			// ドアの壁
			std::weak_ptr<DoorWall> doorWall = object->GetDoorWall();
			// それぞれにそれぞれを渡しあう
			if (leftDoor.expired() == false && doorWall.expired() == false)
			{
				leftDoor.lock()->SetRightDoor(object);
				leftDoor.lock()->SetDoorWall(doorWall);

				doorWall.lock()->SetLeftDoor(leftDoor);
				doorWall.lock()->SetRightDoor(object);

				// 名前
				leftDoor.lock()->SetObjectName("Door" + std::to_string(m_objectCount.Door));
				doorWall.lock()->SetObjectName("Door" + std::to_string(m_objectCount.Door));

				// push_back
				m_wpTerrainList.push_back(leftDoor);
				m_wpTerrainList.push_back(doorWall);
			}
		}
		// 坂
		else if (data.type == "SlopeGround")
		{
			std::shared_ptr<SlopeGround> object = std::make_shared<SlopeGround>();
			object->Init();
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.SlopeGround++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.SlopeGround);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// パラメータセット
			param.startPos = data.pos;	// 座標
			param.scale = data.scale;	// 拡縮
			param.degAng = data.degAng;	// 回転
			param.targetName = data.targetName;	// ターゲットの名前
			param.yetActive = data.yetActive;
			// 座標をセットする
			object->SetParam(param);
			// リストに追加
			m_wpTerrainList.push_back(object);
		}
		// 透明な壁
		else if (data.type == "TransparentWall")
		{
			std::shared_ptr<TransparentWall> object = std::make_shared<TransparentWall>();
			object->Init();
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.TransparentWall++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.TransparentWall);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// パラメータセット
			param.startPos = data.pos;	// 座標
			param.scale = data.scale;	// 拡縮
			param.degAng = data.degAng;	// 回転
			param.targetName = data.targetName;	// ターゲットの名前
			param.yetActive = data.yetActive;	// すでにアクティブかどうか
			// 座標をセットする
			object->SetParam(param);
			// リストに追加
			m_wpTerrainList.push_back(object);
		}
	}

	// スイッチにターゲットをセットする処理
	for (auto& obj : m_wpTerrainList)
	{
		// なかったら飛ばす
		if (obj.expired() == true) continue;
		// スイッチじゃなかったら飛ばす
		if (obj.lock()->GetObjectType() != ObjectType::Switch) continue;
		for (auto& target : m_wpTerrainList)
		{
			// なかったら飛ばす
			if (target.expired() == true) continue;
			if (obj.lock()->GetParam().targetName == target.lock()->GetObjectName())
			{
				obj.lock()->SetTarget(target);
			}
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
				data.scale.x = stof(commaString);
				break;

			case 9:
				data.scale.y = stof(commaString);
				break;

			case 10:
				data.scale.z = stof(commaString);
				break;

			case 11:
				data.speed = stof(commaString);
				break;

			case 12:
				data.stayTime = stoi(commaString);
				break;

			case 13:
				data.degAng.x = stof(commaString);
				break;

			case 14:
				data.degAng.y = stof(commaString);
				break;

			case 15:
				data.degAng.z = stof(commaString);
				break;

			case 16:
				data.targetName = commaString;
				break;

			case 17:
				data.yetActive = stoi(commaString);
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

void TerrainController::CSVWriter(bool _baseFlg)
{
	if (_baseFlg == true)
	{
		std::ofstream ofs("Base" + m_fileName);

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

			// 拡縮
			ofs << data.scale.x << "," << data.scale.y << "," << data.scale.z << ",";

			// スピード
			ofs << data.speed << ",";

			// 待機時間
			ofs << data.stayTime << ",";

			// 回転角度
			ofs << data.degAng.x << "," << data.degAng.y << "," << data.degAng.z << ",";

			// ターゲットの名前
			ofs << data.targetName << ",";

			// すでにActiveになったかどうか
			ofs << data.yetActive << std::endl;
		}
	}

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

		// 拡縮
		ofs << data.scale.x << "," << data.scale.y << "," << data.scale.z << ",";

		// スピード
		ofs << data.speed << ",";

		// 待機時間
		ofs << data.stayTime << ",";

		// 回転角度
		ofs << data.degAng.x << "," << data.degAng.y << "," << data.degAng.z << ",";

		// ターゲットの名前
		ofs << data.targetName << ",";

		// すでにActiveになったかどうか
		ofs << data.yetActive << std::endl;
	}
}

void TerrainController::MouseSelect()
{
	if (SceneManager::Instance().GetDebug() == false) return;
	// マウスでオブジェクトを選択する
	std::shared_ptr<const CameraBase> spCamera = m_wpCamera.lock();

	// カメラが無かったら終了
	if (!spCamera) return;

	// クリックしたら選んだオブジェクトをセットする
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		if (m_rightClickFlg == true) return;
		m_rightClickFlg = true;
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

		int listSize = m_wpTerrainList.size();

		// 当たり判定
		for (int i = 0; i < listSize; i++)
		{
			std::weak_ptr<TerrainBase> obj = m_wpTerrainList[i];
			if (obj.expired() == false)
			{
				if (obj.lock()->Intersects(rayInfo, &resultList))
				{
					hitObjList.push_back(obj);
					// ターゲットモードじゃなかったら
					if (!m_targetFlg)
					{
						// １回でも当たったらリセット
						ConfirmedObject();
					}
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

				// ターゲットセレクトモードじゃなかったら
				if (!m_targetFlg)
				{
					m_wpTargetObject = hitObjList[cnt];

					TerrainBase::Param param = m_wpTargetObject.lock()->GetParam();
					DebugWindow::TerrainParam setParam{ param.startPos, param.goalPos, param.scale, param.speed, param.stayTime, param.degAng, param.targetName };
					DebugWindow::Instance().SetTerrainParam(setParam);
				}
				// ターゲットセレクトモードの場合
				else
				{
					if (m_wpTargetObject.expired() == false)
					{
						m_wpTargetObject.lock()->SetTarget(hitObjList[cnt]);
						TerrainBase::Param param = m_wpTargetObject.lock()->GetParam();
						DebugWindow::TerrainParam setParam{ param.startPos, param.goalPos, param.scale, param.speed, param.stayTime, param.degAng, param.targetName };
						DebugWindow::Instance().SetTerrainParam(setParam);
						// ターゲットセレクトモードを終了する
						m_targetFlg = false;
					}
				}
			}
			cnt++;
		}
	}
	else
	{
		m_rightClickFlg = false;
	}
}
