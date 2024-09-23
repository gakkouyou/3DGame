#include "DebugWindow.h"
#include "../../main.h"
#include "../../Scene/SceneManager.h"
#include "../ObjectController/TerrainController/TerrainController.h"
#include "../ObjectController/EnemyController/EnemyController.h"
#include "../ObjectController/CarryObjectController/CarryObjectController.h"
#include "../ObjectController/EventObjectController/EventObjectController.h"

void DebugWindow::Draw()
{
	//return;

	//===========================================================
	// ImGui開始
	//===========================================================
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//===========================================================
	// 以下にImGui描画処理を記述
	//===========================================================

	// デバッグウィンドウ
	//if (ImGui::Begin("Debug Window"))
	//{
	//	// FPS
	//	ImGui::Text("FPS : %d", m_fpsController.m_nowfps);
	//}
	//ImGui::End();

	//================================================================================
	// ImGui 描画処理
	//================================================================================
	//return;


		// ログウィンドウ
	Application::Instance().m_log.Draw("Log Window");
	if (SceneManager::Instance().GetDebug())
	{
		if (ImGui::Begin("DebugWindow"))
		{
			if(ImGui::CollapsingHeader("Terrain"))
			{
				// Terrain用のウィンドウ
				TerrainWindow();
			}

			if (ImGui::CollapsingHeader("Enemy"))
			{
				// Enemy用のウィンドウ
				EnemyWindow();
			}

			if (ImGui::CollapsingHeader("CarryObject"))
			{
				// CarryObject用のウィンドウ
				CarryObjectWindow();
			}

			if (ImGui::CollapsingHeader("Event"))
			{
				// EventObject用のウィンドウ
				EventObjectWindow();
			}
		}
		ImGui::End();
	}



	//================================================================================
	// Imguiのレンダリング：ここより上にimguiの描画はする事
	//================================================================================
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void DebugWindow::Init()
{
//	// Setup Dear ImGui context
//	IMGUI_CHECKVERSION();
//	ImGui::CreateContext();
//	// Setup Dear ImGui style
//	// ImGui::StyleColorsDark();
//	ImGui::StyleColorsClassic();
//	// Setup Platform/Renderer bindings
//	ImGui_ImplWin32_Init(Application::Instance().GetWindowHandle());
//	ImGui_ImplDX11_Init(
//		KdDirect3D::Instance().WorkDev(), KdDirect3D::Instance().WorkDevContext());
//
//#include "imgui/ja_glyph_ranges.h"
//	ImGuiIO& io = ImGui::GetIO();
//	ImFontConfig config;
//	config.MergeMode = true;
//	io.Fonts->AddFontDefault();
//	// 日本語対応
//	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f, &config, glyphRangesJapanese);
}

void DebugWindow::Release()
{/*
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();*/
}

void DebugWindow::TerrainWindow()
{
	// デバッグウィンドウ
	//if (ImGui::Begin("TerrainController"))
	{
		// Controllerがあるときに処理
		std::shared_ptr<TerrainController> spObjectController = m_wpTerrainController.lock();
		if (spObjectController)
		{
			// オブジェクトを設置
			ImGui::Text((const char*)u8"地形設置  RBUTTON");
			// オブジェクトを確定させる
			if (ImGui::Button((const char*)u8"確定"))
			{
				spObjectController->ConfirmedObject();
			}
			ImGui::SameLine();
			// オブジェクトを削除する
			if (ImGui::Button((const char*)u8"削除"))
			{
				spObjectController->DeleteObject();
			}
			ImGui::SameLine();
			// セーブ
			if (ImGui::Button((const char*)u8"セーブ"))
			{
				spObjectController->CSVWriter();
			}

			// 通常の地面
			if (ImGui::Button("NormalGround"))
			{
				spObjectController->ConfirmedObject();
				spObjectController->CreateObject(KdGameObject::ObjectType::NormalGround);
			}
			ImGui::SameLine();

			// バウンド床
			if (ImGui::Button("BoundGround"))
			{
				spObjectController->ConfirmedObject();
				spObjectController->CreateObject(KdGameObject::ObjectType::BoundGround);
			}

			// 動く床
			if (ImGui::Button("MoveGround"))
			{
				spObjectController->ConfirmedObject();
				spObjectController->CreateObject(KdGameObject::ObjectType::MoveGround);
				m_terrainParam.goalPos = m_terrainParam.startPos;
				m_terrainParam.speed = 0.1f;
				m_terrainParam.stayTime = 60;
			}
			ImGui::SameLine();

			// 回る床
			if (ImGui::Button("RotationGround"))
			{
				spObjectController->ConfirmedObject();
				spObjectController->CreateObject(KdGameObject::ObjectType::RotationGround);
			}

			// 柵の柱
			if (ImGui::Button("FencePillar"))
			{
				spObjectController->ConfirmedObject();
				spObjectController->CreateObject(KdGameObject::ObjectType::FencePillar);
			}

			// 柵の棒
			if (ImGui::Button("FenceBar"))
			{
				spObjectController->ConfirmedObject();
				spObjectController->CreateObject(KdGameObject::ObjectType::FenceBar);
			}
			ImGui::SameLine();

			// 落ちる床
			if (ImGui::Button("DropGround"))
			{
				spObjectController->ConfirmedObject();
				spObjectController->CreateObject(KdGameObject::ObjectType::DropGround);
				m_terrainParam.speed = 0.3f;
				m_terrainParam.stayTime = 60;
			}

			// プロペラ
			if (ImGui::Button("Propeller"))
			{
				spObjectController->ConfirmedObject();
				spObjectController->CreateObject(KdGameObject::ObjectType::Propeller);
			}
			ImGui::SameLine();

			// スイッチ
			if (ImGui::Button("Switch"))
			{
				spObjectController->ConfirmedObject();
				spObjectController->CreateObject(KdGameObject::ObjectType::Switch);
			}

			// ドア
			if (ImGui::Button("Door"))
			{
				spObjectController->ConfirmedObject();
				spObjectController->CreateObject(KdGameObject::ObjectType::Door);
			}

			ImGui::Text((const char*)spObjectController->GetObjectName().c_str());

			// スイッチならターゲットを設定できるようにする
			if (spObjectController->GetObjectType() == KdGameObject::ObjectType::Switch)
			{
				ImGui::InputText("TargetName", &m_terrainParam.targetName);
				ImGui::SameLine;
				// ターゲットセレクトモードの切り替え
				if (ImGui::Button("TargetSelect"))
				{
					spObjectController->TargetSelect();
				}
			}

			// 座標
			ImGui::InputFloat("Pos.x", &m_terrainParam.startPos.x, 5.0f);
			//ImGui::DragFloat("Pos.x", &m_terrainParam.startPos.x, 0.25f);
			ImGui::InputFloat("Pos.y", &m_terrainParam.startPos.y, 5.0f);
			ImGui::InputFloat("Pos.z", &m_terrainParam.startPos.z, 5.0f);


			// 動く床ならゴール座標も変えれる用にする
			if (spObjectController->GetObjectType() == KdGameObject::ObjectType::MoveGround)
			{
				ImGui::InputFloat("GoalPos.x", &m_terrainParam.goalPos.x, 0.25f);
				ImGui::InputFloat("GoalPos.y", &m_terrainParam.goalPos.y, 0.25f);
				ImGui::InputFloat("GoalPos.z", &m_terrainParam.goalPos.z, 0.25f);
				ImGui::InputFloat("Speed", &m_terrainParam.speed, 0.025f);
				ImGui::InputInt("StayTime", &m_terrainParam.stayTime, 1);
			}

			// 落ちる床用
			if (spObjectController->GetObjectType() == KdGameObject::ObjectType::DropGround)
			{
				ImGui::InputFloat("Speed", &m_terrainParam.speed, 0.025f);
				ImGui::InputInt("StayTime", &m_terrainParam.stayTime, 1);
			}

			// 拡縮
			ImGui::InputFloat("Scale.x", &m_terrainParam.scale.x, 0.1f);
			ImGui::InputFloat("Scale.y", &m_terrainParam.scale.y, 0.1f);
			ImGui::InputFloat("Scale.z", &m_terrainParam.scale.z, 0.1f);

			// 回転
			ImGui::InputFloat("DegAng.x", &m_terrainParam.degAng.x, 1.0f);
			ImGui::InputFloat("DegAng.y", &m_terrainParam.degAng.y, 1.0f);
			ImGui::InputFloat("DegAng.z", &m_terrainParam.degAng.z, 1.0f);
		}
	}
	//ImGui::End();
}

void DebugWindow::EnemyWindow()
{
	// デバッグウィンドウ
	//if (ImGui::Begin("EnemyController"))
	{
		// Controllerがあるときに処理
		std::shared_ptr<EnemyController> spObjectController = m_wpEnemyController.lock();
		if (spObjectController)
		{
			// オブジェクト設置
			ImGui::Text((const char*)u8"敵設置   MBUTTON");
			ImGui::Text((const char*)spObjectController->GetObjectName().c_str());
			// オブジェクトを確定させる
			if (ImGui::Button((const char*)u8"確定"))
			{
				spObjectController->ConfirmedObject();
			}
			ImGui::SameLine();
			// オブジェクトを削除する
			if (ImGui::Button((const char*)u8"削除"))
			{
				spObjectController->DeleteObject();
			}
			ImGui::SameLine();
			// セーブ
			if (ImGui::Button((const char*)u8"セーブ"))
			{
				spObjectController->CSVWriter();
			}

			// 通常の敵
			if (ImGui::Button("NormalEnemy"))
			{
				spObjectController->ConfirmedObject();
				spObjectController->CreateObject(KdGameObject::ObjectType::NormalEnemy);

				m_enemyParam.moveArea = 4.0f;
				m_enemyParam.searchArea = 8.0f;
			}
			ImGui::SameLine();

			// 飛ぶ敵
			if (ImGui::Button("FlyEnemy"))
			{
				spObjectController->ConfirmedObject();
				spObjectController->CreateObject(KdGameObject::ObjectType::FlyEnemy);

				m_enemyParam.moveArea = 1.0f;
			}

			// 座標
			ImGui::DragFloat("Pos.x", &m_enemyParam.pos.x, 0.25f);
			ImGui::DragFloat("Pos.y", &m_enemyParam.pos.y, 0.25f);
			ImGui::DragFloat("Pos.z", &m_enemyParam.pos.z, 0.25f);
			// 動く範囲
			ImGui::DragFloat("MoveArea", &m_enemyParam.moveArea, 0.1f);

			// 通常敵用
			if (spObjectController->GetObjectType() == KdGameObject::ObjectType::NormalEnemy)
			{
				// 索敵範囲
				ImGui::DragFloat("SerarchArea", &m_enemyParam.searchArea, 0.1f);
			}

			// 飛ぶ敵用
			if (spObjectController->GetObjectType() == KdGameObject::ObjectType::FlyEnemy)
			{
				// Y軸回転
				ImGui::InputFloat("RotDegAng", &m_enemyParam.rotDegAng, 90.0f);
				if (m_enemyParam.rotDegAng >= 360)
				{
					m_enemyParam.rotDegAng -= 360;
				}
				if (m_enemyParam.rotDegAng < 0)
				{
					m_enemyParam.rotDegAng += 360;
				}
			}
		}
	}
	//ImGui::End();
}

void DebugWindow::CarryObjectWindow()
{
	// デバッグウィンドウ
	//if (ImGui::Begin("CarryObjectController"))
	{
		// Controllerがあるときに処理
		std::shared_ptr<CarryObjectController> spObjectController = m_wpCarryObjectController.lock();
		if (spObjectController)
		{
			// オブジェクト設置
			ImGui::Text((const char*)u8"運べるオブジェクト設置   PKEY");
			ImGui::Text((const char*)spObjectController->GetObjectName().c_str());
			// オブジェクトを確定させる
			if (ImGui::Button((const char*)u8"確定"))
			{
				spObjectController->ConfirmedObject();
			}
			ImGui::SameLine();
			// オブジェクトを削除する
			if (ImGui::Button((const char*)u8"削除"))
			{
				spObjectController->DeleteObject();
			}
			ImGui::SameLine();
			// セーブ
			if (ImGui::Button((const char*)u8"セーブ"))
			{
				spObjectController->CSVWriter();
			}

			// 箱
			if (ImGui::Button("Box"))
			{
				spObjectController->ConfirmedObject();
				spObjectController->CreateObject(KdGameObject::ObjectType::Box);

				m_carryObjectParam.area = 2.0f;
			}

			// 座標
			ImGui::DragFloat("Pos.x", &m_carryObjectParam.pos.x, 0.25f);
			ImGui::DragFloat("Pos.y", &m_carryObjectParam.pos.y, 0.25f);
			ImGui::DragFloat("Pos.z", &m_carryObjectParam.pos.z, 0.25f);
			// 触れれるエリア
			ImGui::DragFloat("Area", &m_carryObjectParam.area, 0.1f);
		}
	}
	//ImGui::End();
}

void DebugWindow::EventObjectWindow()
{
	// デバッグウィンドウ
	//if (ImGui::Begin("EventObjectController"))
	{
		// Controllerがあるときに処理
		std::shared_ptr<EventObjectController> spObjectController = m_wpEventObjectController.lock();
		if (spObjectController)
		{
			// オブジェクト設置
			ImGui::Text((const char*)u8"イベントオブジェクト設置   EKEY");
			ImGui::Text((const char*)spObjectController->GetObjectName().c_str());
			// オブジェクトを確定させる
			if (ImGui::Button((const char*)u8"確定"))
			{
				spObjectController->ConfirmedObject();
			}
			ImGui::SameLine();
			// オブジェクトを削除する
			if (ImGui::Button((const char*)u8"削除"))
			{
				spObjectController->DeleteObject();
			}
			ImGui::SameLine();
			// セーブ
			if (ImGui::Button((const char*)u8"セーブ"))
			{
				spObjectController->CSVWriter();
			}

			// 箱
			if (ImGui::Button("Goal"))
			{
				spObjectController->ConfirmedObject();
				spObjectController->CreateObject(KdGameObject::ObjectType::Goal);
			}

			// ハート
			if (ImGui::Button("HealItem"))
			{
				spObjectController->ConfirmedObject();
				spObjectController->CreateObject(KdGameObject::ObjectType::HealItem);
			}

			// 座標
			ImGui::DragFloat("Pos.x", &m_eventObjectParam.pos.x, 0.25f);
			ImGui::DragFloat("Pos.y", &m_eventObjectParam.pos.y, 0.25f);
			ImGui::DragFloat("Pos.z", &m_eventObjectParam.pos.z, 0.25f);
		}
	}
	//ImGui::End();
}
