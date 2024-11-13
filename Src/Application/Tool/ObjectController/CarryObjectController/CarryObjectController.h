#pragma once

class TPSCamera;
class Player;
class CarryObjectBase;
class TerrainController;

class CarryObjectController : public KdGameObject
{
public:
	CarryObjectController() {}
	~CarryObjectController()	override {}

	void Update()		override;
	void Init()			override;

	// CSVファイルを指定する
	void SetCSV(const std::string& _fileName) { m_fileName = _fileName; }

	// カメラをセットする
	void SetCamera(const std::shared_ptr<const TPSCamera>& _spCamera) { m_wpCamera = _spCamera; }

	// オブジェクトリストを渡す
	const std::vector<std::weak_ptr<CarryObjectBase>> GetObjList() const { return m_wpObjectList; }

	// 今持っているオブジェクトのタイプをゲットする
	const KdGameObject::ObjectType GetObjectType() const;
	// 今持っているオブジェクトの名前をゲットする
	const std::string GetObjectName() const;

	// オブジェクトを確定する(wp_ptrをリセットする)
	void ConfirmedObject();
	// オブジェクトを削除する
	void DeleteObject();
	// オブジェクトを作る
	void CreateObject(KdGameObject::ObjectType _object);
	// CSVに書き込む
	void CSVWriter();

	// プレイヤーをセットする
	void SetPlayer(const std::shared_ptr<Player>& _spPlayer) { m_wpPlayer = _spPlayer; }

	// TerrainControllerをセットする
	void SetTerrainController(const std::shared_ptr<TerrainController>& _spTerrainController) { m_wpTerrainController = _spTerrainController; }

	// 動かすオブジェクトのセット
	void SetObject(std::weak_ptr<CarryObjectBase> _wpTargetObject);

private:
	// CSVファイルの名前
	std::string m_fileName;

	// 動かすオブジェクト
	std::weak_ptr<CarryObjectBase>		m_wpTargetObject;

	// リスト
	std::vector<std::weak_ptr<CarryObjectBase>> m_wpObjectList;

	// カメラ
	std::weak_ptr<const TPSCamera>	m_wpCamera;

	// 最初にCSVから読み込んだデータを基にオブジェクトを作成する
	void BeginCreateObject();
	// ↑をInitで呼べない為、Updateで1度だけ実行する為のフラグ
	bool m_beginCreateFlg = false;

	// オブジェクトの個数
	struct Count
	{
		int Box			= 0;
		int BoxEnemy	= 0;
	};

	Count m_objectCount;
	Count m_objectCountReset;

	// CSVのデータ型
	struct Data
	{
		std::string type;
		std::string name;
		Math::Vector3 pos = Math::Vector3::Zero;
		float area = 0;	// 触れれる範囲
	};
	// CSV配列
	std::vector<Data> m_dataList;
	// CSVを読み込む
	void CSVLoader();

	// ターゲットとなるプレイヤー
	std::weak_ptr<Player> m_wpPlayer;

	// TerrainController
	std::weak_ptr<TerrainController> m_wpTerrainController;
};