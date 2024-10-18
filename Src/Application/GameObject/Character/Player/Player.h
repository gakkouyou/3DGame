#pragma once
#include "../CharacterBase.h"

class CameraBase;
class TerrainController;
class CarryObjectController;
class CarryObjectBase;
class EnemyController;

class Player : public CharacterBase
{
public:
	Player()									{}
	~Player()							override{}

	// 更新
	void Update()						override;
	void PostUpdate()					override;
	// 描画
	void GenerateDepthMapFromLight()	override;
	void DrawLit()						override;
	// 初期化
	void Init()							override;

	// リセット処理
	void Reset()						override;

	// 更新前の座標に戻す
	void BackPos();

	//--------------------
	// セッター
	//--------------------
	// ポーズフラグセット
	void SetPauseFlg(const bool _pauseFlg)	override { if (m_aliveFlg == true) m_pauseFlg = _pauseFlg; }

	// カメラセット
	void SetCamera(const std::shared_ptr<CameraBase>& _spCamera) { m_wpCamera = _spCamera; }

	// 動いていいかのフラグのセット(操作を受け付けなくなるが、更新自体はする)
	void SetStopFlg(const bool _stopFlg) { m_stopFlg = _stopFlg; }

	// ゴールの座標をセット
	void SetGoalPos(const Math::Vector3& _goalPos) { m_goalPos = _goalPos; }

	//--------------------
	// ゲッター
	//--------------------
	// 回転角度ゲット
	const float GetAngle()const { return m_angle; }

	// クリアしたかどうか
	const bool GetGoalFlg() const { return m_goalFlg || m_finalGoalFlg; }

	// モデルをゲット
	const std::shared_ptr<KdModelWork>& GetModel() const { return m_spModel; }

	// ライフをゲット
	const int GetLife() const { return m_life; }
	// ライフの上限をゲット
	const int GetLifeMax()	const { return m_maxLife; }

	// SituationTypeのゲット
	const int GetSituationType() const { return m_situationType; }

	// 運び状態のキャンセル
	void CancelCarryMode() { m_situationType &= (~SituationType::Carry); }

	// ステージセレクトシーンからゲームシーンへ移る時のフラグ
	const bool GetBeginGameScene() const { return m_beginGameSceneFlg; }

	// 今の状況
	enum SituationType
	{
		Idle = 1 << 0,	// 止まっている
		Walk = 1 << 1,	// 歩いている
		Jump = 1 << 2,	// ジャンプしている
		Air = 1 << 3,	// 空中にいる
		WallHit = 1 << 4,	// 壁に触れている
		Run = 1 << 5,	// 走っている
		Carry = 1 << 6,	// 持っている
	};

	// カメラがY軸を追尾すべきかどうか(乗っているオブジェクトによって判断)
	const bool IsCameraTracking() const;

	// EnemyControllerをセットする
	void SetEnemyController(const std::weak_ptr<EnemyController>& _wpEnemyController) { m_wpEnemyController = _wpEnemyController; }

private:
	// 当たり判定
	void HitJudge();
	// 地面との当たり判定
	void HitJudgeGround();
	// 当たったらイベントが発生する系の当たり判定
	void HitJudgeEvent();
	// 敵との当たり判定
	void HitJudgeEnemy();
	// 運べるオブジェクトとの当たり判定
	void HitJudgeCarryObject();

	// ダメージを受けた時の処理
	void DamageProcess();

	// カメラのウィークポインタ
	std::weak_ptr<CameraBase> m_wpCamera;

	// 今の状況
	UINT m_situationType	= Idle;

	// ジャンプ力
	float m_jumpPow			= 0;

	// 角度
	float m_angle			= 0.0f;

	// 回転行列
	Math::Matrix m_rotMat	= Math::Matrix::Identity;

	// スケール
	const float m_scale = 8.f;

	// 当たったら一緒に動くような地形に当たった際の処理のための構造体
	struct HitMoveTerrain
	{
		Math::Matrix transMat	= Math::Matrix::Identity;	// 動く前の行列
		bool hitFlg				= false;					// 当たったかどうか
	};
	
	// 動く床
	HitMoveTerrain m_moveGround;
	// 回る床
	HitMoveTerrain m_rotationGround;
	// 運べるオブジェクトが当たっている地面
	HitMoveTerrain m_carryObjectHitTerrain;
	// 運べるオブジェクト
	HitMoveTerrain m_carryObject;

	// エフェクシア用フラグ
	bool m_effectFlg = false;
	// エフェクシア用ウィークポインタ
	std::weak_ptr<KdEffekseerObject> m_wpEffekseer;

	// ゴールのフラグ
	bool m_goalFlg	= false;

	// ゴールした時の処理
	void GoalProcess();

	// 止めるフラグ
	bool m_stopFlg = false;

	// ゴールの処理
	int m_goalStayCount	= 0;
	const int m_goalStayTime	= 60;
	Math::Vector3 m_goalPos = Math::Vector3::Zero;

	// 足元の煙用
	int m_smokeCount	= 0;
	int m_walkSmokeTime = 30;
	int m_runSmokeTime	= 16;

	// 走り状態の時のスピード
	const float m_runSpeed	= 0.125f;

	// 音
	struct Sound
	{
		std::weak_ptr<KdSoundInstance> wpSound;	// 音
		bool flg = false;						// 既に鳴っているかどうかのフラグ
	};

	enum WalkSoundType
	{
		Grass,
		Tile,
		Max,
	};

	// 歩く音の配列
	std::weak_ptr<KdSoundInstance> m_wpWalkSound[WalkSoundType::Max];
	// 音制御のフラグ
	bool m_nowWalkSoundFlg	= false;
	// 歩く音のタイプ
	UINT m_walkSoundType	= 0;

	// ジャンプ
	Sound m_jumpSound;
	// 敵を踏んだ時の音
	Sound m_stampSound;
	// きのこで跳ねた時の音
	Sound m_boundSound;

	// ライフ
	int m_life = 0;
	const int m_maxLife = 3;

	// ダメージを受けた時の処理用
	struct Damage
	{
		bool		nowDamageFlg	= false;	// ダメージを受けている最中
		const int	damageTime		= 90;		// 無敵時間
		int			damageCount		= 0;		// ダメージを受けた時からのカウント
		const int	blinkingTime	= 5;		// 点滅の間隔の時間
		bool		drawFlg			= true;		// 描画するフラグ
	};
	Damage m_damage;

	// 移動前の座標
	Math::Vector3 m_oldPos;

	// 何かアクションを起こすキー制御
	bool m_actionKeyFlg = false;


	// 運んでいるオブジェクト
	std::weak_ptr<CarryObjectBase> m_wpCarryObject;

	// アニメーションをセットする関数
	void SetAnimation(std::string_view _animationName, bool _loopFlg) { if (m_spAnimator && m_spModel) m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation(_animationName), _loopFlg); }

	// EnemyController
	std::weak_ptr<EnemyController> m_wpEnemyController;

	// 最後に地面にいた時の状態
	UINT m_lastGroundSituationType = 0;

	// リスポーン座標
	Math::Vector3 m_respawnPos;

	// ステージセレクトシーンからゲームシーンに移る時のフラグ
	bool m_beginGameSceneFlg = false;

	// 最終ゴールのフラグ
	bool m_finalGoalFlg = false;

	bool flg[4] = { false, false, false, false };

	// ゴール音のフラグ
	bool m_goalSEFlg = false;
	bool m_goalBGMFlg = false;
};