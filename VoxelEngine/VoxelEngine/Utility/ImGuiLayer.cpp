#include "../Misc/pch.h"
#include "ImGuiLayer.h"



ImGuiLayer::ImGuiLayer(){}

ImGuiLayer::ImGuiLayer(const ImGuiLayer& layer){}

ImGuiLayer::~ImGuiLayer(){}

void ImGuiLayer::Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, context);

}

void ImGuiLayer::BeginFrame()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiLayer::Draw()
{

#pragma region DAYNIGHT_CYCLE

	ImGui::Begin("Day/Night Cycle Debug");

	ImGui::Text("Sun/Moon Direction: (%2.2f, %2.2f, %2.2f) / (%2.2f, %2.2f, %2.2f)",
		Cycle_Data::sunDir.x, Cycle_Data::sunDir.y, Cycle_Data::sunDir.z,
		Cycle_Data::moonDir.x, Cycle_Data::moonDir.y, Cycle_Data::moonDir.z);
	ImGui::Text("Sun/Moon Position: (%2.2f, %2.2f, %2.2f) / (%2.2f, %2.2f, %2.2f)",
		Cycle_Data::sunPos.x, Cycle_Data::sunPos.y, Cycle_Data::sunPos.z,
		Cycle_Data::moonPos.x, Cycle_Data::moonPos.y, Cycle_Data::moonPos.z);

	ImGui::Text("Sun Color "); ImGui::SameLine();
	ImGui::ColorButton("Sun Color", {
		Cycle_Data::sunColor.x,
		Cycle_Data::sunColor.y,
		Cycle_Data::sunColor.z, 1.0f }, ImGuiColorEditFlags_NoAlpha);

	ImGui::Text("Moon Color "); ImGui::SameLine();
	ImGui::ColorButton("Moon Color", {
		Cycle_Data::moonColor.x,
		Cycle_Data::moonColor.y,
		Cycle_Data::moonColor.z,
		1.0f }, ImGuiColorEditFlags_NoAlpha);

	ImGui::Text("Time Elapsed: %2.3f seconds (%1.2f)", Cycle_Data::elapsedTime, Cycle_Data::timePct * 100.0f);
	ImGui::Text(Cycle_Data::cycle);
	ImGui::Text(Cycle_Data::time);

	ImGui::End();

#pragma endregion

#pragma region BLOCK_SHADER

	ImGui::Begin("Debug Panel");

	ImGui::Checkbox("Enable Frustum Culling", &BlockShader_Data::enableFrustumCulling);

	ImGui::Text("Vertex Count: %i", BlockShader_Data::debugVerts);
	ImGui::Text("Draw Calls: %i", BlockShader_Data::numDrawCalls);

	ImGui::End();

#pragma endregion

#pragma region RENDERER_DATA

	ImGui::Begin("Debug Panel");
	ImGui::Text("Player Position: %2.2f, %2.2f, %2.2f (%i, %i, %i)",
		Renderer_Data::playerPos.x, Renderer_Data::playerPos.y, Renderer_Data::playerPos.z,
		(int)Renderer_Data::playerPosChunkSpace.x, (int)Renderer_Data::playerPosChunkSpace.y, (int)Renderer_Data::playerPosChunkSpace.z);
	ImGui::Text("Active Chunks: %i", Renderer_Data::numActiveChunks);
	ImGui::Text("Render Distance: %i", Renderer_Data::renderDist);
	ImGui::End();

#pragma endregion

#pragma region PLAYERPHYSICS_DATA

	ImGui::Begin("Debug Panel");
	ImGui::Text("Velocity: %2.2f, %2.2f, %2.2f", 
		PlayerPhysics_Data::vel.x, PlayerPhysics_Data::vel.y, PlayerPhysics_Data::vel.z);
	ImGui::Text("Acceleration: %2.2f, %2.2f, %2.2f",
		PlayerPhysics_Data::accel.x, PlayerPhysics_Data::accel.y, PlayerPhysics_Data::accel.z);
	ImGui::Text("IsCollidingWall: %i", PlayerPhysics_Data::isCollidingWall);
	ImGui::Text("IsCollidingFloor: %i", PlayerPhysics_Data::isCollidingFloor);
	ImGui::Text("Delta X Rot: %2.2f", PlayerPhysics_Data::deltaXRot);
	ImGui::Text("Delta Y Rot: %2.2f", PlayerPhysics_Data::deltaYRot);
	ImGui::End();

#pragma endregion

#pragma region CHUNKMANAGER_DATA

	ImGui::Begin("Timing Panel");
	ImGui::Text("ChunkManager Update: %2.2f ms", ChunkManager_Data::updateTimer);
	ImGui::Text("ChunkManager Creation Loop: %2.2f ms", ChunkManager_Data::creationLoop);
	ImGui::Text("ChunkManager Deletion Loop: %2.2f ms", ChunkManager_Data::deletionLoop);
	ImGui::Text("ChunkManager Creating Chunks: %2.2f ms", ChunkManager_Data::creatingChunks);
	ImGui::Text("ChunkManager Deleting Chunks: %2.2f ms", ChunkManager_Data::deletingChunks);
	ImGui::End();

#pragma endregion

#pragma region GRAPHICSTIMER_DATA

	ImGui::Begin("Timing Panel");
	ImGui::Text("Graphics Frame: %2.2f", GraphicsTimer_Data::frameTimer);
	ImGui::Text("Graphics Present: %2.2f", GraphicsTimer_Data::presentTimer);
	ImGui::End();

#pragma endregion

}

void ImGuiLayer::EndFrame()
{
	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::Shutdown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool ImGuiLayer::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// Call the WndProc Handler from the Win32 Imgui implementation
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) return true;

	return false;
}

