#include <windows.h>
#include "glew/glew.h"
#include "glew/wglew.h"
#include <GL/gl.h>
#include <stdio.h>
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

	FILE *debug_output_redirect_file;
	freopen_s(&debug_output_redirect_file, "CONOUT$", "w", stdout);

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

	// Enable the above settings for an OpenGL context
	HDC main_device_context = GetDC(window);
	int pixel_format = ChoosePixelFormat(main_device_context, &pixel_format_descriptor);
	SetPixelFormat(main_device_context, pixel_format, &pixel_format_descriptor);

	// A temporary context for glew
	HGLRC opengl_rendering_context = wglCreateContext(main_device_context);
	wglMakeCurrent(main_device_context, opengl_rendering_context);

	glewExperimental = GL_TRUE;
	GLenum glew_error = glewInit();
	if(glew_error != GLEW_OK) {
		printf("Failed to initialise glew %d\n", glew_error);
	} else {
		printf("Glew succeeded in initalising %d\n", glew_error);
	}

	// Ugly, but... TODO(?)
	int wgl_attribs[7];

	if(wglewIsSupported("WGL_ARB_create_context")) {
		printf("Glew is supported\n");
		wgl_attribs[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
		wgl_attribs[1] = 3;
		wgl_attribs[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
		wgl_attribs[3] = 3;
		wgl_attribs[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
		wgl_attribs[5] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
		wgl_attribs[6] = 0;
	}

	// Remake the 330 core version of OpenGL as our current context
	opengl_rendering_context = wglCreateContextAttribsARB(main_device_context, NULL, wgl_attribs);
	wglMakeCurrent(main_device_context, opengl_rendering_context);

	printf("Current OpenGL version: %s\n", glGetString(GL_VERSION));


	//
	// MAIN PROGRAM LOOP
	//

	MSG main_message_buffer = {};

	while(main_message_buffer.message != WM_QUIT) {
		if(PeekMessage(&main_message_buffer, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&main_message_buffer);
			DispatchMessage(&main_message_buffer);
		}
		glClearColor(0.8f, 0.0f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		SwapBuffers(main_device_context);
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
		case WM_PAINT:
			PAINTSTRUCT paint_struct;
			BeginPaint(window, &paint_struct);
			EndPaint(window, &paint_struct);
			return 0;
	}


	return DefWindowProc(window, message, w_param, l_param);
}
