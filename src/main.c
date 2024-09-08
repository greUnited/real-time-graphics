#include <windows.h>
#include <GL/gl.h>
#include "main.h"

int WINAPI
wWinMain(HINSTANCE main_instance, HINSTANCE prev_instance, PWSTR command, int is_shown)
{
	const char MAIN_CLASS_NAME[] = "Main Application Class";

	WNDCLASS window_class = {};

	window_class.style			= CS_VREDRAW | CS_HREDRAW;
	window_class.lpfnWndProc 	= main_window_proc;
	window_class.hInstance		= main_instance;
	window_class.lpszClassName	= MAIN_CLASS_NAME;

	RegisterClass(&window_class);

	HWND window = CreateWindowEx(
						0,
						MAIN_CLASS_NAME,
						"Real Time Graphics",
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
						NULL, NULL, main_instance, NULL
					);

	if(window == NULL) {
		return 0;
	}

	// Console for debugging
	AllocConsole();
	AttachConsole(GetCurrentProcessId());

	ShowWindow(window, is_shown);


	//
	// INITIALISE OPENGL CONTEXT
	//
	
	PIXELFORMATDESCRIPTOR pixel_format_descriptor = {
		sizeof(PIXELFORMATDESCRIPTOR),		// Size
		1,									// Version
		PFD_DRAW_TO_WINDOW |				// Enable drawing to window
		PFD_SUPPORT_OPENGL |				// Enable OpenGL support
		PFD_DOUBLEBUFFER   |				// Enable doublebuffering
		PFD_TYPE_RGBA,						// Enable Red Green Blue Alpha
		32,									// Enable 32 bit Colour depth
		0, 0,								// Ignore red bits and red shift
		0, 0,								// Ignore green bits and green shift
		0, 0,								// Ignore blue bits and blue shift
		0, 0,								// Ignore alpha bits and alpha shift
		0, 									// Ignore bitplanes in accumulation buffer
		0, 0, 0, 0,							// Ignore RGBA bits in accumulation buffer
		24,									// Enable 24 bit depth buffer
		8,									// Enable 8 bit stencil buffer
		0, 									// Ignore auxliary buffer
		0,									// Ignore layer type
		0,									// Ignore overlay and underlay planes
		0, 0, 0								// Ignore layer, visible and damage mask
	};


	//
	// MAIN PROGRAM LOOP
	//

	MSG main_message_buffer = {};

	while(GetMessage(&main_message_buffer, NULL, 0, 0) > 0){
		TranslateMessage(&main_message_buffer);
		DispatchMessage(&main_message_buffer);
	}

	return 0;
}

LRESULT CALLBACK
main_window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{

	switch (message) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}


	return DefWindowProc(window, message, w_param, l_param);
}
