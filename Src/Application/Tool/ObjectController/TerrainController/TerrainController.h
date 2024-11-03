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

	// 地形のリストを渡す
	const std::vector<std::weak_ptr<TerrainBase>>& GetObjList() const { return m_wpTerrainList; }

	// CSVのデータ型
	struct Data
	{
		std::string type;
		std::string name;
		Math::Vector3 pos = Math::Vector3::Zero;
		Math::Vector3 goalPos = Math::Vector3::Zero;
		Math::Vector3 scale = { 1, 1, 1 };
		float speed = 0;
		int stayTime = 0;
		Math::Vector3 degAng = Math::Vector3::Zero;
		std::string targetName;
		int			yetActive = 0;
	};

	// CSVの中身を書き換えたい場合
	std::vector<Data>& WorkCSVData() { return m_dataList; }

	// カメラをセットする
	void SetCamera(const std::shared_ptr<const CameraBase>& _spCamera) { m_wpCamera = _spCamera; }

	// 今持っているオブジェクトのタイプをゲットする
	const ObjectType GetObjectType() const;
	// 今持っているオブジェクトの名前をゲットする
	const std::string GetObjectName() const;

	// オブジェクトを確定する(wp_ptrをリセットする)
	void ConfirmedObject();
	// オブジェクトを削除する
	void DeleteObject();
	// オブジェクトを作る
	void CreateObject(const KdGameObject::ObjectType _object);
	// CSVに書き込む
	void CSVWriter(bool _baseFlg = false);

	// スイッチのターゲット選択モードの切替
	void TargetSelect() { m_targetFlg = !m_targetFlg; };
	const bool GetTargetSelect() const { return m_targetFlg; }

	// 動かすオブジェクトのセット
	void SetObject(std::weak_ptr<TerrainBase> _wpTargetObject);

	// 今のターゲットのオブジェクト
	const std::weak_ptr<TerrainBase> GetTargetObject() const { return m_wpTargetObject; }

private:
	// CSVファイルの名前
	std::string m_fileName;

	// 動かすオブジェクト
	std::weak_ptr<TerrainBase>		m_wpTargetObject;

	// 地形リスト
	std::vector<std::weak_ptr<TerrainBase>> m_wpTerrainList;

	// カメラ
	std::weak_ptr<const CameraBase>	m_wpCamera;

	// 最初にCSVから読み込んだデータを基にオブジェクトを作成する
	void BeginCreateObject();

	// オブジェクトの個数
	struct Count
	{
		int NormalGround	= 0;
		int BoundGround		= 0;
		int MoveGround		= 0;
		int RotationGround	= 0;
		int FencePillar		= 0;
		int FenceBar		= 0;
		int DropGround		= 0;
		int Propeller		= 0;
		int Switch			= 0;
		int Door			= 0;
		int SlopeGround		= 0;
		int TransparentWall = 0;
	};

	Count m_objectCount;
	Count m_objectCountReset;

	// CSV配列
	std::vector<Data> m_dataList;
	// CSVを読み込む
	void CSVLoader();

	// マウスクリックの制御フラグ
	bool m_rightClickFlg = false;

	// ターゲットセレクトモードに移行する為のフラグ
	bool m_targetFlg = false;

	// マウスクリックでオブジェクトを選択できたかどうか
	bool m_clickHitFlg = false;
};