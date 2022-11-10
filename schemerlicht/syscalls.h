#ifndef SCHEMERLICHT_SYSCALLS_H
#define SCHEMERLICHT_SYSCALLS_H

int schemerlicht_write(int fd, const void* buffer, unsigned int count);

int schemerlicht_read(int fd, const void* buffer, unsigned int buffer_size);

int schemerlicht_open_output_file(const char* filename);

int schemerlicht_open_input_file(const char* filename);

int schemerlicht_close_file(int fd);

long schemerlicht_tell(int fd);

#endif //SCHEMERLICHT_SYSCALLS_H