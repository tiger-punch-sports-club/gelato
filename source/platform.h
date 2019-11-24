#pragma once

#include <assert.h>

typedef unsigned char		byte8;

typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned int		uint32;
typedef unsigned long long	uint64;

typedef signed char			int8;
typedef short				int16;
typedef int					int32;
typedef long long			int64;

void check_gl_error(const char* file_name, int line_number);

#define GELATO_HASH_SEED 0xCAFEBABE
#define GELATO_CONTENT_PACKAGE_MAGIC 0xCAFEBABE
#define __DEBUG__

#ifdef __DEBUG__
    #define GL_CHECK(function)\
    function;\
    check_gl_error(__FILE__, __LINE__);
#else
    #define GL_CHECK(function) function
#endif

#define ARRAY_SIZE(array) \
    sizeof(array) / sizeof(array[0]) 



// void Shader::check_error(uint32 shaderID, std::string type, std::string path)
// 		{
// 			char error[1024];
// 			glGetShaderInfoLog(shaderID, 1024, nullptr, error);
// #if DEVELOPMENT == 1
// 			SHF_PRINTF("CheckError for %s, Type = %s \n", path.c_str(), type.c_str());
// 			SHF_PRINTF("\n%s\n", error);

// 			GLint success = 0;
// 			glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

// 			if (success == 0)
// 			{
// #ifdef PLATFORM_WINDOWS
// 				std::string error_message = type + "\n" + path + "\n" + std::string(error);
// 				MessageBoxA(NULL, error_message.c_str(), "Shader compile error", MB_OK | MB_ICONERROR);
// #endif
// 				ASSERT_DISAPPOINTED(error_message.c_str());
// 			}
// #endif
// 		}


//  int err = glGetError();
//             if (err != 0)
//             {
//                 SHF_PRINTF("Error %d happened BEFORE initializing Texture2D (%s | %s) \n", err, _name.c_str(), _path.c_str());
//             }