#pragma once

class CameraBase;
class TerrainController;
class EventObjectController;
class EnemyController;
class CarryObjectController;

class MouseClickHit : public KdGameObject
{
public:
	MouseClickHit() {}
	~MouseClickHit()	override {}

	void Update()	override;

	// カメラをセットする
	void SetCamera(const std::shared_ptr<const CameraBase>& _spCamera) { m_wpCamera = _spCamera; }

	// TerrainControllerをセットする
	void SetTerrainController		(const std::shared_ptr<TerrainController>& _spTerrainController)			{ m_wpTerrainController		= _spTerrainController; }
	void SetEventController			(const std::shared_ptr<EventObjectController>& _spEventController)			{ m_wpEventController		= _spEventController; }
	void SetEnemyController			(const std::shared_ptr<EnemyController>& _spEnemyController)				{ m_wpEnemyController		= _spEnemyController; }
	void SetCarryObjectController	(const std::shared_ptr<CarryObjectController>& _spCarryObjectController)	{ m_wpCarryObjectController	= _spCarryObjectController; }

private:
	// カメラ
	std::weak_ptr<const CameraBase>	m_wpCamera;

	std::weak_ptr<TerrainController> m_wpTerrainController;
	std::weak_ptr<EventObjectController> m_wpEventController;
	std::weak_ptr<EnemyController> m_wpEnemyController;
	std::weak_ptr<CarryObjectController> m_wpCarryObjectController;

	// マウスカーソルのレイ判定を実行
	void ClickHit();

	// 選ばれたオブジェクト以外確定する
	void ConfirmedObject(BaseObjectType _objectType);

	// クリック制御
	bool m_rightClickFlg = false;
};