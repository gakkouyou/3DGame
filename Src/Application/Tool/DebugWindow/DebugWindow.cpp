#include "DebugWindow.h"
#include "../../main.h"
#include "../../Scene/SceneManager.h"
#include "../ObjectController/ObjectController.h"

void DebugWindow::Draw()
{
	//return;
	// 
	//================================================================================
	// ImGui開始
	//================================================================================
	//ImGui_ImplDX11_NewFrame();
	//ImGui_ImplWin32_NewFrame();
	//ImGui::NewFrame();

	//================================================================================
	// ImGui 描画処理
	//================================================================================

	// ImGui Demo ウィンドウ表示 ※すごく参考になるウィンドウです。imgui_demo.cpp参照。
	//ImGui::ShowDemoWindow(nullptr);

	// デバッグウィンドウ
	if (ImGui::Begin("Debug Window"))
	{
		// オブジェクトコントローラーがあるときに処理
		std::shared_ptr<ObjectController> spObjectController = m_wpObjectController.lock();
		if (spObjectController)
		{
			// 地形設置
			ImGui::Text((const char*)u8"地形設置");
			ImGui::SameLine();
			// 地形を確定させる
			if (ImGui::Button((const char*)u8"確定"))
			{
				spObjectController->ConfirmObject();
			}
			ImGui::SameLine();
			// 地形を削除する
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
				spObjectController->ConfirmObject();
				spObjectController->CreateObject(ObjectController::Object::NormalGround);
			}
			ImGui::SameLine();

			// バウンド床
			if (ImGui::Button("BoundGround"))
			{
				spObjectController->ConfirmObject();
				spObjectController->CreateObject(ObjectController::Object::BoundGround);
			}

			// 動く床
			if (ImGui::Button("MoveGround"))
			{
				spObjectController->ConfirmObject();
				spObjectController->CreateObject(ObjectController::Object::MoveGround);
				m_moveObjectInfo.goalPos = m_pos;
				m_moveObjectInfo.speed = 0.1f;
				m_moveObjectInfo.stayTime = 60;
			}

			// 座標
			ImGui::InputFloat("Pos.x", &m_pos.x, 1.0f);
			ImGui::InputFloat("Pos.y", &m_pos.y, 1.0f);
			ImGui::InputFloat("Pos.z", &m_pos.z, 1.0f);

			// 動く床ならゴール座標も変えれる用にする
			if (spObjectController->GetObjectType() == KdGameObject::ObjectType::MoveGround)
			{
				ImGui::InputFloat("GoalPos.x", &m_moveObjectInfo.goalPos.x, 1.0f);
				ImGui::InputFloat("GoalPos.y", &m_moveObjectInfo.goalPos.y, 1.0f);
				ImGui::InputFloat("GoalPos.z", &m_moveObjectInfo.goalPos.z, 1.0f);
				ImGui::InputFloat("Speed", &m_moveObjectInfo.speed, 0.1f);
				ImGui::InputInt("StayTime", &m_moveObjectInfo.stayTime, 1);

				m_moveObjectInfo.startPos = m_pos;
			}
		}
	}


	ImGui::End();

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
