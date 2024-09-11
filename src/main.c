#include <windows.h>
#include "glew/glew.h"
#include "glew/wglew.h"
#include <GL/gl.h>
#include <stdio.h>
#include <stdint.h>
#include "main.h"
#include "wgl_file_loading.h"
#include "wgl_transformation_maths.h"

// These are temporary variables for testing transforms and inputs
char input_queue[4] = {0};
float cube_auto_rotation = 0.0f;

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
						CW_USEDEFAULT, CW_USEDEFAULT, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT,
						NULL, NULL, main_instance, NULL
					);

	if(window == NULL) {
		return 0;
	}

	// Console for debugging
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	
	// Cache the performance frequency
	QueryPerformanceFrequency(&w32perf_frequency);

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


	glViewport(0, 0, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT);


	// OpenGL attribute pointing
	GLuint vertex_array;
	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	// OpenGL buffer data
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_cube), vertices_cube, GL_STATIC_DRAW);

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

	// Uniforms
	GLint uniform_loc_model = glGetUniformLocation(shader_program, "u_model");
	GLint uniform_loc_view = glGetUniformLocation(shader_program, "u_view");
	GLint uniform_loc_projection = glGetUniformLocation(shader_program, "u_projection");

	mat4f uniform_transform_model = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};

	mat4f uniform_transform_view = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};

	mat4f uniform_transform_projection = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};

	//
	// MAIN PROGRAM LOOP
	//

	MSG main_message_buffer = {};
	float aspect_ratio = (float)DEFAULT_WIN_WIDTH / (float)DEFAULT_WIN_HEIGHT;
	/* m_rotate_z(uniform_transform_model, 0.1f); */
	glEnable(GL_DEPTH_TEST);

	// Main loop
	while(main_message_buffer.message != WM_QUIT) {
		if(PeekMessage(&main_message_buffer, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&main_message_buffer);
			DispatchMessage(&main_message_buffer);
		}

		// Do game updating here

		// Get the delta time
		QueryPerformanceCounter(&w32perf_start_time);
		// This is a placeholder sleep
		Sleep(1);

		QueryPerformanceCounter(&w32perf_end_time);
		delta_time = (double)(w32perf_end_time.QuadPart - w32perf_start_time.QuadPart) / w32perf_frequency.QuadPart;

		m_translate(uniform_transform_view, (vec3f){0.0f, -0.45f, -3.0f});
		m_rotate_y(uniform_transform_model, cube_auto_rotation);
		/* m_view_ortho(uniform_transform_projection, -aspect_ratio, aspect_ratio, -1.0f, 1.0f, 0.1f, 100.0f); */
		m_view_perspective(uniform_transform_projection, 45.0f, aspect_ratio, 0.1f, 100.0f);
		cube_auto_rotation += 2.0f * delta_time;

		// Generate the OpenGL output
		glClearColor(0.3f, 0.4f, 0.56f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniformMatrix4fv(uniform_loc_model, 1, GL_FALSE, &uniform_transform_model[0][0]);
		glUniformMatrix4fv(uniform_loc_view, 1, GL_FALSE, &uniform_transform_view[0][0]);
		glUniformMatrix4fv(uniform_loc_projection, 1, GL_FALSE, &uniform_transform_projection[0][0]);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, 36);

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
		case WM_KEYDOWN:
			// TODO
			if(w_param == VK_RIGHT) {
				input_queue[0] = 1;
			}
			if(w_param == VK_UP) {
				input_queue[1] = 1;
			}
			if(w_param == VK_LEFT) {
				input_queue[2] = 1;
			}
			if(w_param == VK_DOWN) {
				input_queue[3] = 1;
			}
		break;
		case WM_KEYUP:
			// TODO
			if(w_param == VK_RIGHT) {
				input_queue[0] = 0;
			}
			if(w_param == VK_UP) {
				input_queue[1] = 0;
			}
			if(w_param == VK_LEFT) {
				input_queue[2] = 0;
			}
			if(w_param == VK_DOWN) {
				input_queue[3] = 0;
			}
		break;
	}

	return DefWindowProc(window, message, w_param, l_param);
}
