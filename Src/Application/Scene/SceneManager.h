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
		for (int i = 0; i < Stage::Max; i++)
		{
			m_stageInfoList.push_back(0);
		}
	}

	// シーン切り替え関数
	void ChangeScene(SceneType sceneType);

	// 現在のシーンのインスタンスを保持しているポインタ
	std::shared_ptr<BaseScene> m_currentScene = nullptr;

	// 現在のシーンの種類を保持している変数
	SceneType m_currentSceneType = SceneType::Result;

	// 次のシーンの種類を保持している変数
	SceneType m_nextSceneType = SceneType::Game;

	// プレイするステージ
	UINT m_nowStage = 2;

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
