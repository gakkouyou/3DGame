#pragma once

class TPSCamera;
class TerrainBase;
class CameraBase;

class TerrainController : public KdGameObject
{
public:
	TerrainController()			{}
	~TerrainController()	override{}

	void Update()		override;
	void Init()			override;

	// リセット処理
	void Reset()		override;

	// CSVファイルを指定する
	void SetCSV(std::string _fileName) { m_fileName = _fileName; }

	// カメラをセットする
	void SetCamera(const std::shared_ptr<const CameraBase>& _spCamera) { m_wpCamera = _spCamera; }

	// 今持っているオブジェクトのタイプをゲットする
	const ObjectType GetObjectType() const;
	// 今持っているオブジェクトの名前をゲットする
	const std::string GetObjectName() const;

	// 作りたいオブジェクト
	enum class Object
	{
		NormalGround,	// 通常の床
		BoundGround,	// 跳ねる床
		MoveGround,		// 動く床
		NormalWall,		// 通常の壁
		RotationGround,	// 回る床
		Fence,			// 柵
		HalfFence,		// 片方の柵
		DropGround,		// 落ちる床
	};

	// オブジェクトを確定する(wp_ptrをリセットする)
	void ConfirmedObject();
	// オブジェクトを削除する
	void DeleteObject();
	// オブジェクトを作る
	void CreateObject(Object _object);
	// CSVに書き込む
	void CSVWriter();

private:
	// CSVファイルの名前
	std::string m_fileName;

	// マウスでオブジェクトを選択する
	void MouseSelect();

	// 動かすオブジェクト
	std::weak_ptr<TerrainBase>		m_wpTargetObject;

	// 地形リスト
	std::vector<std::weak_ptr<TerrainBase>> m_wpTerrainList;

	// カメラ
	std::weak_ptr<const CameraBase>	m_wpCamera;

	// 最初にCSVから読み込んだデータを基にオブジェクトを作成する
	void BeginCreateObject();
	// ↑をInitで呼べない為、Updateで1度だけ実行する為のフラグ
	bool m_beginCreateFlg = false;

	// オブジェクトの個数
	struct Count
	{
		int NormalGround	= 0;
		int BoundGround		= 0;
		int MoveGround		= 0;
		int RotationGround	= 0;
		int Fence			= 0;
		int HalfFence		= 0;
		int DropGround		= 0;
	};

	Count m_objectCount;
	Count m_objectCountReset;

	// CSVのデータ型
	struct Data
	{
		std::string type;
		std::string name;
		Math::Vector3 pos		= Math::Vector3::Zero;
		Math::Vector3 goalPos	= Math::Vector3::Zero;
		float speed				= 0;
		int stayTime			= 0;
		Math::Vector3 degAng	= Math::Vector3::Zero;
	};
	// CSV配列
	std::vector<Data> m_dataList;
	// CSVを読み込む
	void CSVLoader();
};