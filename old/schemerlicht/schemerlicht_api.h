#pragma once

#ifdef _WIN32
#if defined(schemerlicht_EXPORTS)
#  define COMPILER_SCHEMERLICHT_API __declspec(dllexport)
#else
#  define COMPILER_SCHEMERLICHT_API __declspec(dllimport)
#endif
#else
#define COMPILER_SCHEMERLICHT_API
#endif
