#pragma once

#include "../CharacterBase.h"

class StageSelectPlayer : public CharacterBase
{
public:
	StageSelectPlayer()				{}
	~StageSelectPlayer()	override{}

	// 更新
	void Update()						override;
	void PostUpdate()					override;

	// 初期化
	void Init()							override;

	// リセット処理
	void Reset()						override;
};