#pragma once

class TerrainController;
class EnemyController;
class CarryObjectController;
class EventObjectController;

class TerrainBase;

class DebugWindow
{
public:

	void Draw();
	void Init();

	void Release();

	void SetTerrainController		(const std::shared_ptr<TerrainController>& _spObjectController)		{ m_wpTerrainController = _spObjectController; }
	void SetEnemyController			(const std::shared_ptr<EnemyController>& _spObjectController)		{ m_wpEnemyController = _spObjectController; }
	void SetCarryObjectController	(const std::shared_ptr<CarryObjectController>& _spObjectController) { m_wpCarryObjectController = _spObjectController; }
	void SetEventObjectController	(const std::shared_ptr<EventObjectController>& _spObjectController) { m_wpEventObjectController = _spObjectController; }

	// Terrain用のパラメータ
	struct TerrainParam
	{
		Math::Vector3 startPos	= Math::Vector3::Zero;
		Math::Vector3 goalPos	= Math::Vector3::Zero;
		Math::Vector3 scale		= { 1, 1, 1 };
		float speed				= 0;
		int stayTime			= 0;
		Math::Vector3 degAng	= Math::Vector3::Zero;
		std::string targetName;
	};
	// Terrainのパラメータゲット関数
	const TerrainParam GetTerrainParam() const { return m_terrainParam; }
	// Terrainのパラメータセット関数
	void SetTerrainParam(TerrainParam _param) { m_terrainParam = _param; }

	// Enemy用のパラメータ
	struct EnemyParam
	{
		Math::Vector3 pos	= Math::Vector3::Zero;
		float moveArea		= 0;
		float searchArea	= 0;
		float rotDegAng		= 0;
	};
	// Enemyのパラメータゲット関数
	const EnemyParam GetEnemyParam() const { return m_enemyParam; }
	// Enemyのパラメータセット関数
	void SetEnemyParam(EnemyParam _param) { m_enemyParam = _param; }

	// 運べるオブジェクト用のパラメータ
	struct CarryObjectParam
	{
		Math::Vector3 pos = Math::Vector3::Zero;
		float area = 0;
	};
	// 運べるオブジェクト用のパラメータゲット関数
	const CarryObjectParam GetCarryObjectParam() const { return m_carryObjectParam; }
	// 運べるオブジェクト用のパラメータセット関数
	void SetCarryObjectParam(CarryObjectParam _param) { m_carryObjectParam = _param; }

	// EventObject用のパラメータ
	struct EventObjectParam
	{
		Math::Vector3 pos = Math::Vector3::Zero;
		int stageNum = 0;
	};
	// 運べるオブジェクト用のパラメータゲット関数
	const EventObjectParam GetEventObjectParam() const { return m_eventObjectParam; }
	// 運べるオブジェクト用のパラメータセット関数
	void SetEventObjectParam(EventObjectParam _param) { m_eventObjectParam = _param; }

private:
	// Terrain用
	std::weak_ptr<TerrainController> m_wpTerrainController;
	TerrainParam m_terrainParam;
	// Terrain用のウィンドウ
	void TerrainWindow();

	// Enemy用
	std::weak_ptr<EnemyController> m_wpEnemyController;
	EnemyParam m_enemyParam;
	// Enemy用のウィンドウ
	void EnemyWindow();

	// 運べるオブジェクト用
	std::weak_ptr<CarryObjectController> m_wpCarryObjectController;
	CarryObjectParam m_carryObjectParam;
	// 運べるオブジェクト用のウィンドウ
	void CarryObjectWindow();

	// EventObject用
	std::weak_ptr<EventObjectController> m_wpEventObjectController;
	EventObjectParam m_eventObjectParam;
	// EventObject用のウィンドウ
	void EventObjectWindow();

private:

	DebugWindow() { Init(); }
	~DebugWindow() {}

public:
	// シングルトンパターン
	// 常に存在する && 必ず1つしか存在しない(1つしか存在出来ない)
	// どこからでもアクセスが可能で便利だが
	// 何でもかんでもシングルトンという思考はNG
	static DebugWindow& Instance()
	{
		static DebugWindow instance;
		return instance;
	}
};