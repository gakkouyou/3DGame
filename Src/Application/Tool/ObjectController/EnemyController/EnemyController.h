#pragma once

class EnemyBase;
class TPSCamera;
class Player;
class TerrainController;

class EnemyController : public KdGameObject
{
public:
	EnemyController() {}
	~EnemyController()	override {}

	void Update()		override;
	void Init()			override;

	// リセット処理
	void Reset()		override;

	// CSVファイルを指定する
	void SetCSV(std::string& _fileName) { m_fileName = _fileName; }

	// カメラをセットする
	void SetCamera(const std::shared_ptr<const TPSCamera>& _spCamera) { m_wpCamera = _spCamera; }

	// 今持っているオブジェクトのタイプをゲットする
	const KdGameObject::ObjectType GetObjectType() const;
	// 今持っているオブジェクトの名前をゲットする
	const std::string GetObjectName() const;

	// オブジェクトのリストを渡す
	const std::vector<std::weak_ptr<EnemyBase>>& GetObjList() const { return m_wpEnemyList; }

	// オブジェクトを確定する(wp_ptrをリセットする)
	void ConfirmedObject();
	// オブジェクトを削除する
	void DeleteObject();
	// オブジェクトを作る
	void CreateObject(const KdGameObject::ObjectType _object);
	// CSVに書き込む
	void CSVWriter();

	// プレイヤーをセットする
	void SetPlayer(const std::shared_ptr<Player>& _spPlayer) { m_wpPlayer = _spPlayer; }

	// TerrainController
	void SetTerrainController(const std::weak_ptr<TerrainController>& _wpTerrainController) { m_wpTerrainController = _wpTerrainController; }

	// 敵を全員殺す
	void AllDeath();

private:
	// CSVファイルの名前
	std::string m_fileName;

	// マウスでオブジェクトを選択する
	void MouseSelect();

	// 動かすオブジェクト
	std::weak_ptr<EnemyBase>		m_wpTargetObject;

	// 敵リスト
	std::vector<std::weak_ptr<EnemyBase>> m_wpEnemyList;

	// カメラ
	std::weak_ptr<const TPSCamera>	m_wpCamera;

	// 最初にCSVから読み込んだデータを基にオブジェクトを作成する
	void BeginCreateObject();
	// ↑をInitで呼べない為、Updateで1度だけ実行する為のフラグ
	bool m_beginCreateFlg = false;

	// オブジェクトの個数
	struct Count
	{
		int NormalEnemy = 0;
		int FlyEnemy	= 0;
		int BoxEnemy	= 0;
	};

	Count m_objectCount;
	Count m_objectCountReset;

	// CSVのデータ型
	struct Data
	{
		std::string type;
		std::string name;
		Math::Vector3 pos		= Math::Vector3::Zero;
		float moveArea			= 0;	// 動く範囲
		float searchArea		= 0;	// 索敵範囲
		float rotDegAng			= 0;	// 回転角度
	};
	// CSV配列
	std::vector<Data> m_dataList;
	// CSVを読み込む
	void CSVLoader();

	// 敵のターゲットとなるプレイヤー
	std::weak_ptr<Player> m_wpPlayer;

	// TerrainController
	std::weak_ptr<TerrainController> m_wpTerrainController;
};
