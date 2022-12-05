#ifndef SCHEMERLICHT_SYSCALLS_H
#define SCHEMERLICHT_SYSCALLS_H

#include "schemerlicht.h"
#include "limits.h"

int schemerlicht_write(int fd, const void* buffer, unsigned int count);

int schemerlicht_read(int fd, void* buffer, unsigned int buffer_size);

int schemerlicht_open_output_file(const char* filename);

int schemerlicht_open_input_file(const char* filename);

int schemerlicht_close_file(int fd);

long schemerlicht_tell(int fd);

const char* schemerlicht_getenv(const char* name);

int schemerlicht_putenv(const char* name, const char* value);

int schemerlicht_file_exists(const char* filename);

void schemerlicht_fixnum_to_char(char* buffer, schemerlicht_fixnum fx);
void schemerlicht_fixnum_to_hex_char(char* buffer, schemerlicht_fixnum fx);
void schemerlicht_fixnum_to_binary_char(char* buffer, schemerlicht_fixnum fx);
void schemerlicht_fixnum_to_oct_char(char* buffer, schemerlicht_fixnum fx);
void schemerlicht_flonum_to_char(char* buffer, schemerlicht_flonum fl);
void schemerlicht_flonum_to_char_scientific(char* buffer, schemerlicht_flonum fl);
void schemerlicht_memsize_to_char(char* buffer, schemerlicht_memsize s);
void schemerlicht_int_to_char(char* buffer, int i);

#endif //SCHEMERLICHT_SYSCALLS_H