#ifndef _GRAPHICS_H
#define _GRAPHICS_H

// TODO: Include CAMERA_CLASS, SHADERS, and other necessary classes used for rendering
#include "D3D.h"

// idk why it makes me include this here..again
#include <windows.h>

/////////////
// GLOBALS
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_FAR = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Graphics
{
public:

	Graphics();
	Graphics(const Graphics&);
	~Graphics();

	bool Initialize(const int& screenWidth, const int& screenHeight, HWND windowHandle);
	void Shutdown();

	bool Frame(const float& deltaTime);

private:

	D3D* m_D3D;

};

#endif

