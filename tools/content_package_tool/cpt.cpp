#include <stdio.h>
#include <string>
#include <vector>
#include "glob.h"

typedef char byte8;

std::vector<std::string> glob_files(const std::string& pattern)
{
	// glob struct resides on the stack
	glob_t glob_result;
	memset(&glob_result, 0, sizeof(glob_result));

	// do the glob operation
	int return_value = glob(pattern.c_str(), 0, NULL, &glob_result);

	if (return_value != 0)
	{
		globfree(&glob_result);
	}

	// collect all the filenames into a std::list<std::string>
	std::vector<std::string> filenames;
	for (size_t i = 0; i < glob_result.gl_pathc; ++i)
	{
		filenames.push_back(std::string(glob_result.gl_pathv[i]));
	}

	// cleanup
	globfree(&glob_result);

	// done
	return filenames;
}

int execute_command(const char* cmd) 
{
    return system(cmd);
}

uint64_t file_size(std::string& file_path)
{
	FILE* file = fopen(file_path.c_str(), "rb");

	fseek(file, 0, SEEK_END);
	uint64_t file_size_bytes = static_cast<uint64_t>(ftell(file));
	fseek(file, 0, SEEK_SET);

	fclose(file);
	return file_size_bytes;
}

void* file_data(std::string& file_path)
{
	FILE* file = fopen(file_path.c_str(), "rb");

	fseek(file, 0, SEEK_END);
	uint64_t file_size_bytes = static_cast<uint64_t>(ftell(file));
	fseek(file, 0, SEEK_SET);

	void* buffer = calloc(1, file_size_bytes);
	fread(buffer, file_size_bytes, 1, file);

	fclose(file);
	return buffer;
}

void file_read(byte8* dest, std::string& file_path)
{
	FILE* file = fopen(file_path.c_str(), "rb");

	fseek(file, 0, SEEK_END);
	uint64_t file_size_bytes = static_cast<uint64_t>(ftell(file));
	fseek(file, 0, SEEK_SET);

	fread(dest, file_size_bytes, 1, file);

	fclose(file);
}

int main(void)
{
	printf("Cool!\n");
	return 0;
}