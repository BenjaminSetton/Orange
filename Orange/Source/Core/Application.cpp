#include "../Misc/pch.h"

#include "Application.h"
#include "../Utility/FileSystem/FileSystem.h"
#include "../Utility/FontManager.h"
#include "Game.h"
#include "../Utility/HeapOverrides.h"
#include "../Utility/ImGuiLayer.h"
#include "./Events/KeyCodes.h"
#include "UI/UIHelper.h"

namespace Orange
{

	Application* Application::Handle = nullptr;

	Application::Application() : EventSubject(),
		m_mainWindow(nullptr), m_Input(nullptr), m_Graphics(nullptr), m_Clock(nullptr)
	{
	}

	Application::~Application() 
	{
	}

	bool Application::Initialize() 
	{
		bool result;

		if (Handle == nullptr)
		{
			Handle = this;
		}
		else
		{
			OG_ASSERT_MSG(false, "Why are we trying to create multiple applications?");
		}

		// Initialize the FileSystem module
		FileSystem::Initialize();

		// Populate the window parameters
		WindowParameters params;
		params.fullScreen = false;
		params.width = 1600;
		params.height = 900;
		params.x = 0;
		params.y = 0;
		params.name = L"Orange";
		params.alignment = Centered;

		// Create the main window
		m_mainWindow = OG_NEW Window();
		m_mainWindow->Create(params);

		// Initialize the Font Manager
		FontManager::Initialize();

		// Create the input object.  This object will be used to handle reading the keyboard input from the user.
		m_Input = OG_NEW Input();
		Subscribe(m_Input);

		// Initialize the EditorLayer module
		EditorLayer::Initialize();

		// Initialize the game object
		Game::Initialize();

		UI::Initialize();

		// Create the graphics object
		m_Graphics = OG_NEW Graphics();
		result = m_Graphics->Initialize();
		OG_ASSERT_MSG(result, "Graphics Initialize() failed");

		return true;
	}

	void Application::Run() 
	{
		MSG msg;
		bool isRunning = true;

		while (isRunning) 
		{
			// Handle the windows messages
			while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT) return;
			}

			isRunning = Update();
		}

	}

	void Application::Shutdown() 
	{
		if (m_Graphics)
		{
			m_Graphics->Shutdown();
			delete m_Graphics;
			m_Graphics = nullptr;
		}

		if (m_mainWindow)
		{
			m_mainWindow->Destroy();
			delete m_mainWindow;
			m_mainWindow = nullptr;
		}

		if (m_Input)
		{
			delete m_Input;
			m_Input = nullptr;
		}

		if (m_Clock)
		{
			delete m_Clock;
			m_Clock = nullptr;
		}

		FontManager::Deinitialize();
		EditorLayer::Shutdown();
		Game::Shutdown();
		UI::Shutdown();

		Handle = nullptr;
	}

	bool Application::Update()
	{
		// Quit running if ESC is pressed
		if (m_Input->IsKeyDown(VK_ESCAPE)) return false;
		m_Input->Update();

		// Signal the clock every frame to obtain deltaTime
		m_Clock->Signal();

		UI::BeginFrame();

		// Store the frame's delta time in dt
		float dt = m_Clock->GetDeltaTime(Clock::TimePrecision::SECONDS);
		// HACK! Prevent delta time from giving bad results after moving game window
		dt = dt > 0.5f ? 0.016666f : dt;

		UI::Update(dt);

		Game::Update(dt);

		// We should consider using a switch case to check which layer is active
		EditorLayer::Update(dt);

		// Process each frame in the graphics class
		m_Graphics->Frame(dt);

		// TODO: Move this to a separate Draw function
		EditorLayer::Draw();

		UI::EndFrame();

		m_Graphics->Present();

		return true;
	}

	LRESULT CALLBACK Application::MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (m_Graphics && m_Graphics->WndProc(hwnd, msg, wparam, lparam)) return true;

		switch (msg)
		{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_KEYDOWN:
		{
			KeyboardDownEvent keyDown = KeyboardDownEvent(static_cast<uint16_t>(wparam));
			Broadcast(keyDown);
			return 0;
		}
		case WM_KEYUP:
		{
			KeyboardUpEvent keyUp = KeyboardUpEvent(static_cast<uint16_t>(wparam));
			Broadcast(keyUp);
			return 0;
		}
		case WM_MOUSEMOVE:
		{
			MouseMovedEvent mouseMoved = MouseMovedEvent(LOWORD(lparam), HIWORD(lparam));
			Broadcast(mouseMoved);
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			MouseButtonDownEvent lmbDown = MouseButtonDownEvent(MK_LBUTTON);
			Broadcast(lmbDown);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			MouseButtonUpEvent lmbUp = MouseButtonUpEvent(MK_LBUTTON);
			Broadcast(lmbUp);
			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			MouseButtonDownEvent rmbDown = MouseButtonDownEvent(MK_RBUTTON);
			Broadcast(rmbDown);
			return 0;
		}
		case WM_RBUTTONUP:
		{
			MouseButtonUpEvent rmbUp = MouseButtonUpEvent(MK_RBUTTON);
			Broadcast(rmbUp);
			return 0;
		}
		case WM_MOUSEHWHEEL:
		{
			MouseScrolledEvent scroll = MouseScrolledEvent(HIWORD(wparam));
			Broadcast(scroll);
			return 0;
		}
		}

		return DefWindowProc(hwnd, msg, wparam, lparam);

	}

	const Window* Application::GetMainWindow()
	{
		return m_mainWindow;
	}

}

