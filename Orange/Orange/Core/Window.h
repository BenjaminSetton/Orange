#ifndef _WINDOW_H
#define _WINDOW_H
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

enum WindowAlignment
{
	TopLeft = 0,
	Centered,
	BottomRight,
	Custom
};

struct WindowParameters
{
	LPCWSTR name;
	int32_t width, height, x, y;
	bool fullScreen;
	WindowAlignment alignment;
};

// TODO: Switch over all window logic from Application.h to this class
// TODO: Think about having derived classes for different implementations (eg Windows, Mac, etc)
class Window
{
public:

	Window() = default;
	Window(const Window& window) = default;
	~Window() = default;

	void Create(const WindowParameters& parameters);
	void Destroy();

	int32_t GetWidth() const;
	int32_t GetHeight() const;
	int32_t GetX() const;
	int32_t GetY() const;
	float GetAspectRatio() const;
	LPCWSTR GetName() const;
	HWND GetHWND() const;
	bool IsFullScreen() const;

private:

	HWND m_hwnd;
	LPCWSTR m_windowName;
	HINSTANCE m_hinstance;
	bool m_isFullscreen;
};

#endif