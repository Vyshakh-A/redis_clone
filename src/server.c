#include "commands.h"
#include "resp.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void) {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd < 0) {
    perror("socket");
    exit(1);
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(6379);

  if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    perror("bind");
    exit(1);
  }

  if (listen(server_fd, 128) < 0) {
    perror("listen");
    exit(1);
  }

  while (1) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd =
        accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

    if (client_fd < 0) {
      perror("accept");
      continue;
    }

    char inbuf[4096];
    size_t inlen = 0;
    int client_error = 0;
    while (1) {
      if (inlen == sizeof(inbuf)) break;

      ssize_t n = read(client_fd, inbuf + inlen, sizeof(inbuf) - inlen);
      if (n <= 0) break;

      inlen += (size_t)n;

      while (inlen > 0) {
        arg_t args[MAX_ARGS];
        int argc;
        int used = parse_command(inbuf, inlen, args, &argc);

        if (used == 0) break;
        if (used < 0) {
          if (WRITE_STR(client_fd, "-ERR protocol error\r\n") < 0) {
            perror("write");
            client_error = 1;
          }
          inlen = 0;
          break;
        }

        if (execute_command(client_fd, args, argc) < 0) {
          perror("write");
          client_error = 1;
          break;
        }

        memmove(inbuf, inbuf + used, inlen - (size_t)used);
        inlen -= (size_t)used;
      }
      if (client_error) break;
    }

    close(client_fd);
  }

  return 0;
}
