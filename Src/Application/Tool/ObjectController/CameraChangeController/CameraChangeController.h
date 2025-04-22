#pragma once
class CameraBase;
class CameraChange;

class CameraChangeController : public KdGameObject
{
public:
	CameraChangeController() {}
	~CameraChangeController()	override {}

	void Update()		override;
	void Init()			override;

	// CSVファイルを指定する
	void SetCSV(std::string _fileName) { m_fileName = _fileName; }

	// カメラをセットする
	void SetCamera(const std::shared_ptr<CameraBase>& _spCamera) { m_wpCamera = _spCamera; }

	// オブジェクトリストを渡す
	const std::vector<std::weak_ptr<CameraChange>>& GetObjList() const { return m_wpObjectList; }

	// CSVのデータ型
	struct Data
	{
		std::string type;
		std::string name;
		Math::Vector3 pos;
		float area;
		Math::Vector3 targetPos;
	};

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

	// 動かすオブジェクトのセット
	void SetObject(std::weak_ptr<CameraChange> _wpTargetObject);

private:
	// CSVファイルの名前
	std::string m_fileName;

	// 動かすオブジェクト
	std::weak_ptr<CameraChange>				m_wpTargetObject;

	// リスト
	std::vector<std::weak_ptr<CameraChange>>	m_wpObjectList;

	// カメラ
	std::weak_ptr<CameraBase>	m_wpCamera;

	// 最初にCSVから読み込んだデータを基にオブジェクトを作成する
	void BeginCreateObject();

	// オブジェクトの個数
	struct Count
	{
		int CameraChange = 0;
	};

	Count m_objectCount;
	Count m_objectCountReset;

	// CSV配列
	std::vector<Data> m_dataList;
	// CSVを読み込む
	void CSVLoader();
};