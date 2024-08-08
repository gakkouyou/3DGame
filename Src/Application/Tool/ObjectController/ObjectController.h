#pragma once

class TPSCamera;
class TerrainBase;

class ObjectController : public KdGameObject
{
public:
	ObjectController()			{}
	~ObjectController()	override{}

	void Update()		override;
	void Init()			override;

	// カメラをセットする
	void SetCamera(const std::shared_ptr<const TPSCamera>& _spCamera) { m_wpCamera = _spCamera; }

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
	};

	// オブジェクトを確定する(wp_ptrをリセットする)
	void ConfirmObject();
	// オブジェクトを削除する
	void DeleteObject();
	// オブジェクトを作る
	void CreateObject(Object _object);
	// CSVに書き込む
	void CSVWriter();

private:
	// マウスでオブジェクトを選択する
	void MouseSelect();

	// 動かすオブジェクト
	std::weak_ptr<TerrainBase>		m_wpTargetObject;

	// 地形リスト
	std::vector<std::weak_ptr<TerrainBase>> m_wpTerrainList;

	// カメラ
	std::weak_ptr<const TPSCamera>	m_wpCamera;

	// 最初にCSVから読み込んだデータを基にオブジェクトを作成する
	void BeginCreateObject();
	// ↑をInitで呼べない為、Updateで1度だけ実行する為のフラグ
	bool beginCreateFlg = false;

	// オブジェクトの個数
	struct Count
	{
		int NormalGround	= 0;
		int BoundGround		= 0;
		int MoveGround		= 0;
		int RotationGround	= 0;
	};

	Count m_objectCount;

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