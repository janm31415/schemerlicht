#pragma once

#include "namespace.h"
#include "schemerlicht_api.h"
#include <string>

COMPILER_BEGIN

COMPILER_SCHEMERLICHT_API std::wstring convert_string_to_wstring(const std::string& str);
COMPILER_SCHEMERLICHT_API std::string convert_wstring_to_string(const std::wstring& str);

COMPILER_SCHEMERLICHT_API std::string get_executable_path();
COMPILER_SCHEMERLICHT_API std::string get_cwd();
/*
Everything is assumed to be in utf8 encoding
*/
COMPILER_SCHEMERLICHT_API std::string get_folder(const std::string& path);
COMPILER_SCHEMERLICHT_API std::string get_filename(const std::string& path);
COMPILER_SCHEMERLICHT_API std::string getenv(const std::string& name);
COMPILER_SCHEMERLICHT_API void putenv(const std::string& name, const std::string& value);

COMPILER_END
