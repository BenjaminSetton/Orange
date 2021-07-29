#include "../Misc/pch.h"

#include "Application.h"
#include "ApplicationHandle.h"

// Windows procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{

	switch (msg)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, msg, wparam, lparam);
	}
	}
}

Application::Application()
{
	m_Input = nullptr;
	m_Graphics = nullptr;
	m_Clock = nullptr;

}
Application::Application(const Application&){}

Application::~Application()
{
	// Clean up in reverse order of object initialization

	if (m_Clock)
	{
		delete m_Clock;
		m_Clock = nullptr;
	}

	if (m_Graphics)
	{
		// Shutdown the graphics class before deleting
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = nullptr;
	}

	if(m_Input)
	{
		delete m_Input;
		m_Input = nullptr;
	}
}

bool Application::Initialize() 
{
	UINT screenWidth, screenHeight;
	bool result;

	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	m_Input = new Input;
	if (!m_Input) { return false; }

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	m_Graphics = new Graphics;
	if (!m_Graphics) { return false; }

	// Initialize the graphics object.
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result) return false;

	// Create the clock object
	m_Clock = new Clock;
	if (!m_Clock) return false;


	return true;
}

void Application::Run() 
{
	MSG msg;
	bool running = true;

	while (running) 
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Check if the quit message is given and stop running if it has
		if (msg.message == WM_QUIT) running = false;
		else // we can go to the next frame
		{
			running = Frame();
		}


	}

}

void Application::Shutdown() 
{
	// Shutdown the application and all of it's dependencies 
	if (m_Input) 
	{
		delete m_Input;
		m_Input = nullptr;
	}
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = nullptr;
	}
	if (m_Clock)
	{
		delete m_Clock;
		m_Clock = nullptr;
	}

	ShutdownWindows();
}

bool Application::Frame() 
{
	// Quit running if ESC is pressed
	if (m_Input->IsKeyDown(VK_ESCAPE)) return false;

	// Signal the clock every frame to obtain deltaTime
	m_Clock->Signal();

	// Store the frame's delta time in dt
	float dt = m_Clock->GetDeltaTime(Clock::TimePrecision::SECONDS);
	
	// Process each frame in the graphics class
	m_Graphics->Frame(dt);

	return true;
}

void Application::InitializeWindows(UINT& screenWidth, UINT& screenHeight) 
{
	int posX, posY;

	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Voxel Engine";

	// Setup the windows class with default settings.
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 1040x585 resolution.
		screenWidth = 1040;
		screenHeight = 585;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_OVERLAPPEDWINDOW | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	return;
}


LRESULT CALLBACK Application::MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (m_Graphics && m_Graphics->WndProc(hwnd, msg, wparam, lparam)) return true;

	switch (msg)
	{
		// Check if a key has been pressed on the keyboard.
	case WM_KEYDOWN:
	{
		// If a key is pressed send it to the input object so it can record that state.
		//m_Input->KeyDown(static_cast<unsigned int>(wparam));
		KeyboardEvent event;
		event.key = static_cast<unsigned int>(wparam);
		event.isPressed = true;

		Broadcast(event);

		return 0;
	}

	// Check if a key has been released on the keyboard.
	case WM_KEYUP:
	{
		// If a key is released then send it to the input object so it can unset the state for that key.
		//m_Input->KeyUp(static_cast<unsigned int>(wparam));

		KeyboardEvent event;
		event.key = static_cast<unsigned int>(wparam);
		event.isPressed = false;

		Broadcast(event);

		return 0;
	}

	// Any other messages send to the default message handler as our application won't make use of them.
	default:
	{
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	}

}

void Application::ShutdownWindows()
{

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(nullptr, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = nullptr;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = nullptr;

	// Release the pointer to this class.
	ApplicationHandle = nullptr;

	return;
}
