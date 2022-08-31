#pragma once

#include "namespace.h"
#include "schemescript_api.h"
#include "vm/vm.h"
#include "compiler.h"
#include <map>
#include <string>

COMPILER_BEGIN

int _skiwi_close(int const fd);
int _skiwi_read(int const fd, void* const buffer, unsigned const buffer_size);
int _skiwi_write(int fd, const void* buffer, unsigned int count);
const char* _skiwi_getenv(const char* name);
int _skiwi_putenv(const char* name, const char* value);
int _skiwi_file_exists(const char* filename);
uint64_t _skiwi_current_seconds();
uint64_t _skiwi_current_milliseconds();
uint64_t _skiwi_sprintf(char *, const char * , const char* );
uint64_t _skiwi_sprintf_floating(char * a, const char * b, double c);

COMPILER_SCHEMESCRIPT_API void add_system_calls(std::map<std::string, external_function>& externals);

VM::external_function convert_external_to_vm(const external_function& ext);
std::vector<VM::external_function> convert_externals_to_vm(std::map<std::string, external_function>& externals);


COMPILER_END
