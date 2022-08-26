#pragma once

#ifdef _WIN32
#if defined(schemescript_EXPORTS)
#  define COMPILER_SCHEMESCRIPT_API __declspec(dllexport)
#else
#  define COMPILER_SCHEMESCRIPT_API __declspec(dllimport)
#endif
#else
#define COMPILER_SCHEMESCRIPT_API
#endif
