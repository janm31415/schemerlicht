#ifndef SCHEMERLICHT_SYSCALLS_H
#define SCHEMERLICHT_SYSCALLS_H

int schemerlicht_write(int fd, const void* buffer, unsigned int count);

int schemerlicht_open_output_file(const char* filename);

int schemerlicht_open_input_file(const char* filename);

int schemerlicht_close_file(int fd);

#endif //SCHEMERLICHT_SYSCALLS_H