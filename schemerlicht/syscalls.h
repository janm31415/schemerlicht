#pragma once

#include "namespace.h"
#include "schemerlicht_api.h"
#include "vm/vm.h"
#include "compiler.h"
#include <map>
#include <string>

COMPILER_BEGIN

int _scheme_close(int const fd);
int _scheme_read(int const fd, void* const buffer, unsigned const buffer_size);
int _scheme_write(int fd, const void* buffer, unsigned int count);
const char* _scheme_getenv(const char* name);
int _scheme_putenv(const char* name, const char* value);
int _scheme_file_exists(const char* filename);
uint64_t _scheme_current_seconds();
uint64_t _scheme_current_milliseconds();
uint64_t _scheme_sprintf(char *, const char * , const char* );
uint64_t _scheme_sprintf_floating(char * a, const char * b, double c);

COMPILER_SCHEMERLICHT_API void add_system_calls(std::map<std::string, external_function>& externals);

VM::external_function convert_external_to_vm(const external_function& ext);
std::vector<VM::external_function> convert_externals_to_vm(std::map<std::string, external_function>& externals);


COMPILER_END
