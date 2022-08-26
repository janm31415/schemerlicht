#pragma once

#ifdef _WIN32
#if defined(vm_EXPORTS)
#define VM_API __declspec(dllexport)
#else
#define VM_API __declspec(dllimport)
#endif
#else
#define VM_API
#endif