#pragma once

class BaseScene;

class SceneManager
{
public :

	// シーン情報
	enum class SceneType
	{
		Title,
		StageSelect,
		Game,
		Result,
		First,
	};

	void PreUpdate();
	void Update();
	void PostUpdate();

	void PreDraw();
	void Draw();
	void DrawSprite();
	void DrawDebug();

	// 次のシーンをセット (次のフレームから切り替わる)
	void SetNextScene(SceneType nextScene)
	{
		m_nextSceneType = nextScene;
	}

	// 現在のシーンのオブジェクトリストを取得
	const std::list<std::shared_ptr<KdGameObject>>& GetObjList();

	// 現在のシーンにオブジェクトを追加
	void AddObject(const std::shared_ptr<KdGameObject>& obj);

	// 今のシーンをゲット
	const SceneType GetNowScene() const { return m_currentSceneType; }

	// ステージをゲット
	const UINT GetNowStage() const { return m_nowStage; }
	// ステージをセット
	void SetNowStage(int _nowStage) { m_nowStage = _nowStage; }

	// ステージのクリア状況のリストを変更する場合
	std::vector<UINT>& WorkStageInfo() { return m_stageInfoList; }
	// ステージのクリア状況のリストのゲット
	const std::vector<UINT>& GetStageInfo() const { return m_stageInfoList; }

	// ステージのクリア状況のCSVをロードする
	void StageInfoCSVLoader();
	// ステージのクリア状況をCSVに書き込む
	void StageInfoCSVWriter();

	// デバッグモードかどうか
	bool GetDebug() const;

	// ステージ
	enum Stage
	{
		Stage1 = 1,
		Stage2,
		Stage3,
		AllGimmick,
		Max
	};

	// ステージのクリア状況
	enum StageInfo
	{
		NotClear,
		FirstClear,
		Clear,
	};

	// ステージセレクトのTerrainのCSVをリセットする
	void CSVReset();

private :

	// マネージャーの初期化
	// インスタンス生成(アプリ起動)時にコンストラクタで自動実行
	void Init()
	{
		// CSVを読み込む
		StageInfoCSVLoader();

		// 開始シーンに切り替え
		ChangeScene(m_currentSceneType);
	}

	// シーン切り替え関数
	void ChangeScene(SceneType sceneType);

	// 現在のシーンのインスタンスを保持しているポインタ
	std::shared_ptr<BaseScene> m_currentScene = nullptr;

	// 現在のシーンの種類を保持している変数
	SceneType m_currentSceneType = SceneType::First;

	// 次のシーンの種類を保持している変数
	//SceneType m_nextSceneType = SceneType::Title;
	SceneType m_nextSceneType = SceneType::Game;
	//SceneType m_nextSceneType = SceneType::StageSelect;
	//SceneType m_nextSceneType = SceneType::Result;

	// プレイするステージ
	//UINT m_nowStage	= Stage::AllGimmick;
	UINT m_nowStage = Stage::Stage2;
	//UINT m_nowStage	= 0;

	// ステージのクリア状況
	std::vector<UINT> m_stageInfoList;

private:

	SceneManager() { Init(); }
	~SceneManager() { CSVReset(); }

public:

	// シングルトンパターン
	// 常に存在する && 必ず1つしか存在しない(1つしか存在出来ない)
	// どこからでもアクセスが可能で便利だが
	// 何でもかんでもシングルトンという思考はNG
	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}
};
