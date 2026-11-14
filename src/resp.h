#ifndef RESP_H
#define RESP_H

#include <stddef.h>

#define MAX_ARGS 16

typedef struct {
  const char* ptr;
  size_t len;
} arg_t;

int parse_command(const char* buf, size_t len, arg_t* args, int* argc);
int write_all(int fd, const void* data, size_t len);
int write_data(int fd, const arg_t* arg);

#define WRITE_STR(fd, s) write_all((fd), (s), sizeof(s) - 1)

#endif
