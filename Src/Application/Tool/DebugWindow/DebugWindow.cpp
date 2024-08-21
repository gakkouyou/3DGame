#include "DebugWindow.h"
#include "../../main.h"
#include "../../Scene/SceneManager.h"
#include "../ObjectController/TerrainController/TerrainController.h"
#include "../ObjectController/EnemyController/EnemyController.h"

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
	if (SceneManager::Instance().GetDebug())
	{
		// ログウィンドウ
		//Application::Instance().m_log.Draw("Log Window");

		// Terrain用のウィンドウ
		//TerrainWindow();

		// Enemy用のウィンドウ
		//EnemyWindow();
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
	if (ImGui::Begin("TerrainController"))
	{
		// Controllerがあるときに処理
		std::shared_ptr<TerrainController> spObjectController = m_wpTerrainController.lock();
		if (spObjectController)
		{
			// オブジェクトを設置
			ImGui::Text((const char*)u8"地形設置  RBUTTON");
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

			// 通常の地面
			if (ImGui::Button("NormalGround"))
			{
				spObjectController->ConfirmedObject();
				spObjectController->CreateObject(TerrainController::Object::NormalGround);
			}
			ImGui::SameLine();

			// バウンド床
			if (ImGui::Button("BoundGround"))
			{
				spObjectController->ConfirmedObject();
				spObjectController->CreateObject(TerrainController::Object::BoundGround);
			}

			// 動く床
			if (ImGui::Button("MoveGround"))
			{
				spObjectController->ConfirmedObject();
				spObjectController->CreateObject(TerrainController::Object::MoveGround);
				m_terrainParam.goalPos = m_terrainParam.startPos;
				m_terrainParam.speed = 0.1f;
				m_terrainParam.stayTime = 60;
			}

			// 回る床
			if (ImGui::Button("RotationGround"))
			{
				spObjectController->ConfirmedObject();
				spObjectController->CreateObject(TerrainController::Object::RotationGround);
			}

			// フェンス
			if (ImGui::Button("Fence"))
			{
				spObjectController->ConfirmedObject();
				spObjectController->CreateObject(TerrainController::Object::Fence);
			}

			// 片方柵
			if (ImGui::Button("HalfFence"))
			{
				spObjectController->ConfirmedObject();
				spObjectController->CreateObject(TerrainController::Object::HalfFence);
			}

			// 座標
			ImGui::InputFloat("Pos.x", &m_terrainParam.startPos.x, 1.0f);
			ImGui::InputFloat("Pos.y", &m_terrainParam.startPos.y, 1.0f);
			ImGui::InputFloat("Pos.z", &m_terrainParam.startPos.z, 1.0f);

			// 動く床ならゴール座標も変えれる用にする
			if (spObjectController->GetObjectType() == KdGameObject::ObjectType::MoveGround)
			{
				ImGui::InputFloat("GoalPos.x", &m_terrainParam.goalPos.x, 1.0f);
				ImGui::InputFloat("GoalPos.y", &m_terrainParam.goalPos.y, 1.0f);
				ImGui::InputFloat("GoalPos.z", &m_terrainParam.goalPos.z, 1.0f);
				ImGui::InputFloat("Speed", &m_terrainParam.speed, 0.1f);
				ImGui::InputInt("StayTime", &m_terrainParam.stayTime, 1);
			}

			ImGui::InputFloat("DegAng.x", &m_terrainParam.degAng.x, 1.0f);
			ImGui::InputFloat("DegAng.y", &m_terrainParam.degAng.y, 1.0f);
			ImGui::InputFloat("DegAng.z", &m_terrainParam.degAng.z, 1.0f);
		}
	}
	ImGui::End();
}

void DebugWindow::EnemyWindow()
{
	// デバッグウィンドウ
	if (ImGui::Begin("EnemyController"))
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
				spObjectController->CreateObject(EnemyController::Object::NormalEnemy);

				m_enemyParam.moveArea = 20.0f;
				m_enemyParam.searchArea = 40.0f;
			}

			// 座標
			ImGui::InputFloat("Pos.x", &m_enemyParam.pos.x, 1.0f);
			ImGui::InputFloat("Pos.y", &m_enemyParam.pos.y, 1.0f);
			ImGui::InputFloat("Pos.z", &m_enemyParam.pos.z, 1.0f);
			// 動く範囲
			ImGui::InputFloat("MoveArea", &m_enemyParam.moveArea, 0.5f);
			// 索敵範囲
			ImGui::InputFloat("SerarchArea", &m_enemyParam.searchArea, 0.5f);
		}
	}
	ImGui::End();
}