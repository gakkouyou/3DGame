#include "EnemyController.h"
#include "../../DebugWindow/DebugWindow.h"
#include "../../../Scene/SceneManager.h"
#include "../../../main.h"
#include "../../../GameObject/Camera/TPSCamera/TPSCamera.h"

#include "../../../GameObject/Character/Enemy/EnemyBase.h"
#include "../../../GameObject/Character/Enemy/NormalEnemy/NormalEnemy.h"
#include "../../../GameObject/Character/Enemy/FlyEnemy/FlyEnemy.h"

#include "../../../GameObject/Character/Player/Player.h"

#include "../TerrainController/TerrainController.h"

void EnemyController::Update()
{
	// マウスでオブジェクトを選択する
	MouseSelect();

	// 対象のオブジェクト
	std::shared_ptr<EnemyBase> spTargetObject = m_wpTargetObject.lock();
	if (spTargetObject)
	{
		DebugWindow::EnemyParam debugParam = DebugWindow::Instance().GetEnemyParam();
		EnemyBase::Param setParam{ debugParam.pos, debugParam.moveArea, debugParam.searchArea, debugParam.rotDegAng };
		spTargetObject->SetParam(setParam);
	}

	// DELETEキーで削除する
	if (GetAsyncKeyState(VK_DELETE) & 0x8000)
	{
		DeleteObject();
	}
}

void EnemyController::Init()
{
	// CSVを読み込む
	CSVLoader();

	// 読み込んだデータからオブジェクトを作成する
	BeginCreateObject();
}

void EnemyController::Reset()
{
	// 再配置
	//m_beginCreateFlg = false;

	m_objectCount = m_objectCountReset;
	m_wpEnemyList.clear();

	// 読み込んだデータからオブジェクトを作成する
	BeginCreateObject();
}

const KdGameObject::ObjectType EnemyController::GetObjectType() const
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

const std::string EnemyController::GetObjectName() const
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

void EnemyController::ConfirmedObject()
{
	std::shared_ptr<EnemyBase> spTargetObject = m_wpTargetObject.lock();
	if (spTargetObject)
	{
		// もし名前が決められてなかったら新たにdataListに追加する
		if (spTargetObject->GetObjectName() == "None")
		{
			Data data;
			// オブジェクトのタイプと名前を入れる
			switch (spTargetObject->GetObjectType())
			{
				// 通常の敵の場合
			case ObjectType::NormalEnemy:
				// タイプのセット
				data.type = "NormalEnemy";
				// カウントを進める
				m_objectCount.NormalEnemy++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.NormalEnemy);
				break;

				// 飛ぶ敵の場合
			case ObjectType::FlyEnemy:
				// タイプのセット
				data.type = "FlyEnemy";
				// カウントを進める
				m_objectCount.FlyEnemy++;
				// 名前を決める
				data.name = data.type + std::to_string(m_objectCount.FlyEnemy);
				break;
			}
			// 名前をセットする
			spTargetObject->SetObjectName(data.name);
			// 情報をセットする
			data.pos		= spTargetObject->GetParam().startPos;	// 座標
			data.moveArea	= spTargetObject->GetParam().moveArea;	// 動いていい範囲
			data.searchArea = spTargetObject->GetParam().searchArea;// 索敵範囲
			data.rotDegAng	= spTargetObject->GetParam().rotDegAng;	// Y軸回転
			// データが入っているリストにプッシュバックする
			m_dataList.push_back(data);
			// 地形のウィークポインタのリストにプッシュバックする
			m_wpEnemyList.push_back(spTargetObject);
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
			m_dataList[num].moveArea	= spTargetObject->GetParam().moveArea;	// 動いていい範囲
			m_dataList[num].searchArea	= spTargetObject->GetParam().searchArea;// 索敵範囲
			m_dataList[num].rotDegAng	= spTargetObject->GetParam().rotDegAng;	// Y軸回転
		}
	}

	m_wpTargetObject.reset();
}

void EnemyController::DeleteObject()
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

void EnemyController::CreateObject(KdGameObject::ObjectType _object)
{
	switch (_object)
	{
		// 通常の敵
	case KdGameObject::ObjectType::NormalEnemy:
	{
		std::shared_ptr<NormalEnemy> object = std::make_shared<NormalEnemy>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		// ターゲットをセット
		if (!m_wpPlayer.expired())
		{
			object->SetTarget(m_wpPlayer.lock());
		}
		// TerrainControllerをセット
		object->SetTerrainController(m_wpTerrainController);
		break;
	}

		// 飛ぶ敵
	case KdGameObject::ObjectType::FlyEnemy:
	{
		std::shared_ptr<FlyEnemy> object = std::make_shared<FlyEnemy>();
		object->Init();
		SceneManager::Instance().AddObject(object);
		m_wpTargetObject = object;
		// TerrainControllerをセット
		object->SetTerrainController(m_wpTerrainController);
		break;
	}
	}
}

void EnemyController::AllDeath()
{
	for (auto& enemy : m_wpEnemyList)
	{
		if (!enemy.expired())
		{
			enemy.lock()->OnHit();
		}
	}
}

void EnemyController::SetObject(std::weak_ptr<EnemyBase> _wpTargetObject)
{
	// 確定
	ConfirmedObject();

	m_wpTargetObject = _wpTargetObject;

	EnemyBase::Param param = m_wpTargetObject.lock()->GetParam();
	DebugWindow::EnemyParam setParam{ param.startPos, param.moveArea, param.searchArea, param.rotDegAng };
	DebugWindow::Instance().SetEnemyParam(setParam);
}

void EnemyController::MouseSelect()
{

}

void EnemyController::BeginCreateObject()
{
	for (auto& data : m_dataList)
	{
		// 通常の敵
		if (data.type == "NormalEnemy")
		{
			std::shared_ptr<NormalEnemy> object = std::make_shared<NormalEnemy>();
			// パラメータをセットする
			EnemyBase::Param setParam{ data.pos, data.moveArea, data.searchArea };
			object->SetParam(setParam);
			// ターゲットをセットする
			if (!m_wpPlayer.expired())
			{
				object->SetTarget(m_wpPlayer.lock());
			}
			object->Init();
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.NormalEnemy++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.NormalEnemy);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// TerrainControllerをセット
			object->SetTerrainController(m_wpTerrainController);
			// リストに追加
			m_wpEnemyList.push_back(object);
		}
		// 飛ぶ敵
		else if (data.type == "FlyEnemy")
		{
			std::shared_ptr<FlyEnemy> object = std::make_shared<FlyEnemy>();
			// パラメータをセットする
			EnemyBase::Param setParam{ data.pos, data.moveArea, 0, data.rotDegAng };
			object->SetParam(setParam);
			object->Init();
			SceneManager::Instance().AddObject(object);
			// カウントを進める
			m_objectCount.FlyEnemy++;
			// 名前の数値をリセットする
			std::string name = data.type + std::to_string(m_objectCount.FlyEnemy);
			// 名前をセットする
			object->SetObjectName(name);
			// 配列の名前を変更する
			data.name = name;
			// TerrainControllerをセット
			object->SetTerrainController(m_wpTerrainController);
			// リストに追加
			m_wpEnemyList.push_back(object);
		}
	}
}

void EnemyController::CSVLoader()
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
				data.moveArea = stof(commaString);
				break;

			case 6:
				data.searchArea = stof(commaString);
				break;

			case 7:
				data.rotDegAng	= stof(commaString);
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

void EnemyController::CSVWriter()
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

		// 動いていい範囲
		ofs << data.moveArea << ",";

		// 索敵範囲
		ofs << data.searchArea << ",";

		// Y軸回転
		ofs << data.rotDegAng << std::endl;
	}
}