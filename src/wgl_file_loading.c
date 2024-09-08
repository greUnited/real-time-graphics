#include "wgl_file_loading.h"

// TODO: Load just a single shader? Also make better
char*
wgl_load_shader_source(const char *file_name)
{
	FILE *shader_file = fopen(file_name, "rb");
	if(shader_file == NULL) {
		printf("ERROR: Could not load shader file %s\n", file_name);
		return NULL;
	}

	fseek(shader_file, 0, SEEK_END);
	long shader_file_length = ftell(shader_file);
	rewind(shader_file);

	char *shader_source_code = (char *)malloc((shader_file_length + 1) * sizeof(char));
	if(!shader_source_code) {
		printf("ERROR: Could not get source code from file: %s\n", file_name);
		return NULL;
	}

	fread(shader_source_code, sizeof(char), shader_file_length, shader_file);
	shader_source_code[shader_file_length] = '\0';

	fclose(shader_file);
	return shader_source_code;
}
