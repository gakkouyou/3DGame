﻿#pragma once
#include "../CharacterBase.h"

class CameraBase;
class TerrainController;
class CarryObjectController;
class CarryObjectBase;
class EventObjectController;
class GameUI;
class Tutorial;

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
	void DrawUnLit()					override;
	void DrawBright()					override;
	// 初期化
	void Init()							override;

	// リセット処理
	void Reset()						override;

	// 更新前の座標に戻す
	void BackPos();

	// 当たった時の処理
	void OnHit()	override;

	//--------------------
	// セッター
	//--------------------
	// ポーズフラグセット
	void SetPauseFlg(const bool _pauseFlg)	override { if (m_aliveFlg == true) m_pauseFlg = _pauseFlg; }

	// カメラセット
	void SetCamera(const std::shared_ptr<CameraBase>& _spCamera) { m_wpCamera = _spCamera; }

	// 動いていいかのフラグのセット(操作を受け付けなくなるが、更新自体はする) UIにも渡す
	void SetStopFlg(const bool _stopFlg);

	// ゴールの座標をセット
	void SetGoalPos(const Math::Vector3& _goalPos) { m_goalPos = _goalPos; }

	// UIセット
	void SetGameUI(const std::shared_ptr<GameUI>& _gameUI) { m_wpGameUI = _gameUI; }

	// 操作説明セット
	void SetTutorial(const std::shared_ptr<Tutorial>& _tutorial) { m_wpTutorial = _tutorial; }

	// EventObjectControllerをセットする
	void SetEventObjectController(const std::weak_ptr<EventObjectController>& _wpEventObjectController) { m_wpEventObjectController = _wpEventObjectController; }

	void CameraFinish() { m_goalJumpFlg = true; }

	//--------------------
	// ゲッター
	//--------------------
	// 回転角度ゲット
	const float GetAngle()const { return m_angle; }

	// クリアしたかどうか
	const bool GetGoalFlg() const { return m_goalFlg || m_finalGoalFlg; }

	// モデルをゲット
	const std::shared_ptr<KdModelWork>& GetModel() const { return m_spModel; }

	// SituationTypeのゲット
	const int GetSituationType() const { return m_situationType; }

	// 運び状態のキャンセル
	void CancelCarryMode() { m_situationType &= (~SituationType::Carry); }

	// ステージセレクトシーンからゲームシーンへ移る時のフラグ
	const bool GetBeginGameScene() const { return m_beginGameSceneFlg; }

	// 今の状況
	enum SituationType
	{
		Idle			= 1 << 0,	// 止まっている
		Walk			= 1 << 1,	// 歩いている
		Jump			= 1 << 2,	// ジャンプしている
		Air				= 1 << 3,	// 空中にいる
		WallHit			= 1 << 4,	// 壁に触れている
		Run				= 1 << 5,	// 走っている
		Carry			= 1 << 6,	// 持っている
		CarryAnimation	= 1 << 7,	// 持つときのアニメーション
	};

	// カメラがY軸を追尾すべきかどうか(乗っているオブジェクトによって判断)
	const bool IsCameraTracking() const;

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

	// アニメーションをセットする関数
	void SetAnimation(std::string_view _animationName, bool _loopFlg) { if (m_spAnimator && m_spModel) m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation(_animationName), _loopFlg); }

	// アニメーションの変更をまとめた関数
	void ChangeAnimation();

	// ゴールした時の処理
	void GoalProcess();

	// 着地する場所を示すエフェクトの座標を決める
	void LandingPosEffectPos(KdCollider::RayInfo _rayInfo);

	// 音を設定
	void SoundLoad(std::weak_ptr<KdSoundInstance>& _wpSound, std::string _path, float _vol);

	// ステージに入った時の座標
	Math::Vector3 m_stageStartPos;
	// 移動前の座標
	Math::Vector3 m_oldPos;
	// リスポーン座標
	Math::Vector3 m_respawnPos;

	// これ以上下に行くと死ぬライン
	float m_underLine = 0;

	// 今の状況
	UINT m_situationType	= Idle;
	// 更新前の状況
	UINT m_oldSituationType = Idle;
	// 最後に地面にいた時の状態
	UINT m_lastGroundSituationType = 0;

	// ジャンプ力
	float m_jumpPow	= 0;
	// 跳ねる床に乗った時のジャンプ力
	float m_boundJumpPow = 0;
	// 敵を踏んだ時のジャンプ力
	float m_enemyJumpPow = 0;

	// 角度
	float m_angle			= 0.0f;
	// 回転角度制限
	float m_maxAngle		= 0.0f;

	// 回転行列
	Math::Matrix m_rotMat	= Math::Matrix::Identity;

	// スケール
	float m_scale = 0.0f;

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

	// ゴールのフラグ
	bool m_goalFlg	= false;

	// ゴールの処理
	int m_goalStayCount = 0;
	const int m_goalStayTime = 60;
	Math::Vector3 m_goalPos = Math::Vector3::Zero;
	bool m_goalJumpFlg = false;
	bool m_oldGoalJumpFlg = false;

	// 止めるフラグ(操作を受け付けなくなる)
	bool m_stopFlg = false;

	// 足元の煙用
	int m_smokeCount	= 0;
	int m_walkSmokeTime = 0;
	int m_runSmokeTime	= 0;

	// 走り状態の時のスピード
	float m_runSpeed	= 0;
	// 歩き状態の時のスピード
	float m_walkSpeed	= 0;

	// 物を持つときのアニメーション時間
	float m_carryAnimationTime = 20;
	float m_carryAnimationCount = 0;

	// 着地エフェクト
	std::shared_ptr<KdModelData>m_spEffectModel = nullptr;
	bool m_landingEffectFlg = false;
	Math::Vector3 m_landingEffectPos;
	float m_landingEffectAlpha = 0.0f;
	float m_maxLandingEffectLength = 5.0f;

	// 音
	struct Sound
	{
		std::weak_ptr<KdSoundInstance> wpSound;	// 音
		bool flg = false;						// 既に鳴っているかどうかのフラグ
	};

	// 歩いた時の音の種類
	enum WalkSEType
	{
		Grass,
		Tile,
		Max,
	};

	// 歩く音の配列
	std::weak_ptr<KdSoundInstance> m_wpWalkSE[WalkSEType::Max];
	// 音制御のフラグ
	bool m_nowWalkSEFlg	= false;
	// 歩く音のタイプ
	UINT m_walkSEType	= 0;

	// ジャンプ
	Sound m_jumpSE;
	// 敵を踏んだ時の音
	Sound m_stampSE;
	// きのこで跳ねた時の音
	Sound m_boundSE;
	// 落ちた時の音
	Sound m_dropSE;
	// ゴールした時の効果音
	Sound m_goalSE;
	// ゴールした時のBGM
	Sound m_goalBGM;
	// ステージを選択した際のSE
	Sound m_stageSelectSE;
	// 物を持った時のSE
	Sound m_holdSE;
	// 物を離した時のSE
	Sound m_letGoSE;

	// 何かアクションを起こすキー制御
	bool m_actionKeyFlg = false;

	// ステージセレクトシーンからゲームシーンに移る時のフラグ
	bool m_beginGameSceneFlg = false;

	// 最終ゴールのフラグ
	bool m_finalGoalFlg = false;

	// シーンを始めた時の着地音防止フラグ
	bool m_firstLandFlg = false;

	// 二つのオブジェクトに触れた時、何度以上の角度なら座標を戻すかの角度
	const float m_doubleObjectHitMinDegAng = 80.0f;
	const float m_doubleObjectHitMaxDegAng = 100.0f;

	// カメラのウィークポインタ
	std::weak_ptr<CameraBase> m_wpCamera;

	// EventObjectController
	std::weak_ptr<EventObjectController> m_wpEventObjectController;

	// ゲームのUI
	std::weak_ptr<GameUI> m_wpGameUI;

	// 操作説明
	std::weak_ptr<Tutorial> m_wpTutorial;

	// 運んでいるオブジェクト
	std::weak_ptr<CarryObjectBase> m_wpCarryObject;

	// 物を持てる状態かどうか
	bool m_holdFlg = false;
	// ステージに入れる状態かどうか
	bool m_stageSelectObjectFlg = false;
	// 持てない状態でアクションキーを押すと、空ぶる
	bool m_missingShotFlg = false;
	int m_missingShotCount = 0;
	int m_missingShotTime = 30;

	// 無限ジャンプ
	bool m_mugenJumpFlg = false;
	bool m_mugenJumpKeyFlg = false;

	// JSONファイルのパス
	std::string_view m_path = "Asset/Data/Json/Character/Player/Player.json";

	// JSONのデータをロードする
	void DataLoad();
};