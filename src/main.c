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

	MSG main_message_buffer = {};

	while(GetMessage(&main_message_buffer, NULL, 0, 0) > 0) {
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

		case WM_PAINT:
			PAINTSTRUCT paint_struct;
			HDC device_context = BeginPaint(window, &paint_struct);
			FillRect(device_context, &paint_struct.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
			EndPaint(window, &paint_struct);
		return 0;
	}


	return DefWindowProc(window, message, w_param, l_param);
}
