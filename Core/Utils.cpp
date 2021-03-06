#include "Utils.h"

void Utils::TraceGLError(string tag)
{
    GLenum errorCode = glGetError();
    if (errorCode != GL_NO_ERROR)
    {
        fprintf(stderr, "%s: %d\n", tag.data(), errorCode);
    }
}

GLchar* Utils::LoadFile(const char* filename)
{
	FILE* file;
	long filesize;
	GLchar* buffer = new GLchar[MAX_FILE_SIZE];
	GLchar* result;

	errno_t err = fopen_s(&file, filename, "r");
	if (err == 0)
	{
		fseek(file, 0, SEEK_END);
		filesize = ftell(file);
		fseek(file, 0, SEEK_SET);
		filesize = fread_s(buffer, MAX_FILE_SIZE, sizeof(GLchar), MAX_FILE_SIZE, file);
		fclose(file);

		result = new GLchar[filesize + 1];
		strncpy_s(result, filesize + 1, buffer, filesize);
		result[filesize] = 0;
		delete buffer;
	}
	else
	{
		result = new GLchar[]{ '\0' };
		printf("Error when loading file '%s'\n", filename);
	}
	return result;
}