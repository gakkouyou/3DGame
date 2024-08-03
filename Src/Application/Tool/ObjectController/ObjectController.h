#pragma once

class TPSCamera;
class MoveGround;

class ObjectController : public KdGameObject
{
public:
	ObjectController()			{}
	~ObjectController()	override
	{
		ConfirmObject();
		CSVWriter();
	}

	void Update()		override;
	void Init()			override;

	// カメラをセットする
	void SetCamera(const std::shared_ptr<const TPSCamera>& _spCamera) { m_wpCamera = _spCamera; }

	// 今持っているオブジェクトのタイプをゲットする
	const KdGameObject::ObjectType GetObjectType() const;

	// 作りたいオブジェクト
	enum class Object
	{
		NormalGround,	// 通常の床
		BoundGround,	// 跳ねる床
		MoveGround,		// 動く床
		NormalWall,		// 通常の壁
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
	std::weak_ptr<KdGameObject>		m_wpTargetObject;

	// 動く床用
	std::weak_ptr<MoveGround>		m_wpMoveGround;
	
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
	};

	Count m_objectCount;

	// CSVのデータ型
	struct Data
	{
		std::string type;
		std::string name;
		Math::Vector3 pos;
	};
	// CSV配列
	std::vector<Data> m_dataList;
	// CSVを読み込む
	void CSVLoader();

	// 動く床用のデータ型
	struct MoveData
	{
		std::string type;
		std::string name;
		Math::Vector3 startPos;
		Math::Vector3 goalPos;
		float speed;
		int stayTime;
	};
	// 配列
	std::vector<MoveData> m_moveDataList;
};