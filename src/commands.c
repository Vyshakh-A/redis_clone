#include "commands.h"

#include "db.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>

static int is_cmd(const arg_t* arg, const char* name) {
  return arg->len == strlen(name) &&
         strncasecmp(arg->ptr, name, arg->len) == 0;
}

int execute_command(int fd, const arg_t* args, int argc) {
  printf("cmd:");
  for (int i = 0; i < argc; i++)
    printf(" [%.*s]", (int)args[i].len, args[i].ptr);
  printf("\n");

  if (is_cmd(&args[0], "PING")) {
    return WRITE_STR(fd, "+PONG\r\n");
  } else if (is_cmd(&args[0], "ECHO")) {
    if (argc == 2) {
      return write_data(fd, &args[1]);
    } else {
      return WRITE_STR(fd,
                       "-ERR wrong number of arguments for 'echo' command\r\n");
    }
  } else if (is_cmd(&args[0], "SET")) {
    if (argc == 3) {
      if (db_set(args[1].ptr, args[1].len, args[2].ptr, args[2].len) == 0) {
        return WRITE_STR(fd, "+OK\r\n");
      } else {
        return WRITE_STR(fd, "-ERR out of memory\r\n");
      }
    } else {
      return WRITE_STR(fd,
                       "-ERR wrong number of arguments for 'set' command\r\n");
    }
  } else if (is_cmd(&args[0], "GET")) {
    if (argc == 2) {
      size_t value_len;
      const char* value = db_get(args[1].ptr, args[1].len, &value_len);
      if (value == NULL) {
        return WRITE_STR(fd, "$-1\r\n");
      } else {
        arg_t out = {value, value_len};
        return write_data(fd, &out);
      }
    } else {
      return WRITE_STR(fd,
                       "-ERR wrong number of arguments for 'get' command\r\n");
    }
  } else {
    return WRITE_STR(fd, "-ERR unknown command\r\n");
  }
}
