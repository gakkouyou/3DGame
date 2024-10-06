#pragma once

class CameraBase;
class Player;
class EventObjectBase;

class EventObjectController : public KdGameObject
{
public:
	EventObjectController() {}
	~EventObjectController()	override {}

	void Update()		override;
	void Init()			override;

	// リセット処理
	void Reset()		override;

	// CSVファイルを指定する
	void SetCSV(std::string _fileName) { m_fileName = _fileName; }

	// カメラをセットする
	void SetCamera(const std::shared_ptr<CameraBase>& _spCamera) { m_wpCamera = _spCamera; }

	// プレイヤーをセットする
	void SetPlayer(const std::shared_ptr<Player>& _spPlayer) { m_wpPlayer = _spPlayer; }

	// オブジェクトリストを渡す
	const std::vector<std::weak_ptr<EventObjectBase>>& GetObjList() const { return m_wpObjectList; }

	// 今持っているオブジェクトのタイプをゲットする
	const KdGameObject::ObjectType GetObjectType() const;
	// 今持っているオブジェクトの名前をゲットする
	const std::string GetObjectName() const;

	// オブジェクトを確定する(wp_ptrをリセットする)
	void ConfirmedObject();
	// オブジェクトを削除する
	void DeleteObject();
	// オブジェクトを作る
	void CreateObject(const KdGameObject::ObjectType _object);
	// CSVに書き込む
	void CSVWriter();

private:
	// CSVファイルの名前
	std::string m_fileName;

	// マウスでオブジェクトを選択する
	void MouseSelect();

	// 動かすオブジェクト
	std::weak_ptr<EventObjectBase>				m_wpTargetObject;

	// リスト
	std::vector<std::weak_ptr<EventObjectBase>>	m_wpObjectList;

	// カメラ
	std::weak_ptr<CameraBase>	m_wpCamera;
	// プレイヤー
	std::weak_ptr<Player>		m_wpPlayer;

	// 最初にCSVから読み込んだデータを基にオブジェクトを作成する
	void BeginCreateObject();

	// オブジェクトの個数
	struct Count
	{
		int Goal		= 0;
		int HealItem	= 0;
		int Candy		= 0;
		int SavePoint	= 0;
		int WarpPoint	= 0;
		int StageSelectObject = 0;
	};

	Count m_objectCount;
	Count m_objectCountReset;

	// CSVのデータ型
	struct Data
	{
		std::string type;
		std::string name;
		Math::Vector3 pos;
		Math::Vector3 secondPos;
		int modelNum;
	};
	// CSV配列
	std::vector<Data> m_dataList;
	// CSVを読み込む
	void CSVLoader();
};