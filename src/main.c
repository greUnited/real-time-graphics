#include <windows.h>
#include "glew/glew.h"
#include "glew/wglew.h"
#include <GL/gl.h>
#include <stdio.h>
#include "main.h"
#include "wgl_file_loading.h"

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



	// OpenGL attribute pointing
	GLuint vertex_array;
	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	// OpenGL buffer data
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(temp_vertex_data), temp_vertex_data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	// OpenGL shader source
	char *tri_vertex_source = wgl_load_shader_source("shaders/equi_tri_vertex.glsl");
	char *tri_fragment_source = wgl_load_shader_source("shaders/equi_tri_fragment.glsl");
	
	// OpenGL shaders
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &tri_vertex_source, NULL);
	glCompileShader(vertex_shader);

	GLint shader_success;
	char info_log[512];

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_success);
	if(shader_success == 0) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		printf("Could not compile shader: %s\n", info_log);
	}

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &tri_fragment_source, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_success);
	if(shader_success == 0) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		printf("Could not compile shader: %s\n", info_log);
	}

	// OpenGL program
	GLuint shader_program;
	shader_program = glCreateProgram();

	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	glGetProgramiv(shader_program, GL_LINK_STATUS, &shader_success);
	if(shader_success == 0) {
		glGetProgramInfoLog(shader_program, 512, NULL, info_log);
	}

	glUseProgram(shader_program);

	// Clean up shaders
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	//
	// MAIN PROGRAM LOOP
	//

	MSG main_message_buffer = {};

	while(main_message_buffer.message != WM_QUIT) {
		if(PeekMessage(&main_message_buffer, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&main_message_buffer);
			DispatchMessage(&main_message_buffer);
		}

		glClearColor(0.3f, 0.4f, 0.56f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glDrawArrays(GL_TRIANGLES, 0, 3);
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
