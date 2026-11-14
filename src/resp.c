#include "resp.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int write_all(int fd, const void* data, size_t len) {
  const char* ptr = data;
  size_t written = 0;

  while (written < len) {
    ssize_t n = write(fd, ptr + written, len - written);
    if (n < 0) {
      if (errno == EINTR) continue;
      return -1;
    }
    if (n == 0) {
      errno = EIO;
      return -1;
    }
    written += (size_t)n;
  }

  return 0;
}

static int read_int_line(const char* buf, size_t len, size_t pos, char prefix,
                         long* out, size_t* next) {
  if (pos >= len) return 0;
  if (buf[pos] != prefix) return -1;

  size_t i = pos + 1;
  while (i + 1 < len && !(buf[i] == '\r' && buf[i + 1] == '\n')) i++;
  if (i + 1 >= len) return 0;

  size_t ndigits = i - (pos + 1);
  if (ndigits == 0 || ndigits > 10) return -1;

  char tmp[16];
  memcpy(tmp, buf + pos + 1, ndigits);
  tmp[ndigits] = '\0';

  char* end;
  long val = strtol(tmp, &end, 10);
  if (*end != '\0' || val < 0) return -1;

  *out = val;
  *next = i + 2;
  return 1;
}

int parse_command(const char* buf, size_t len, arg_t* args, int* argc) {
  long count;
  size_t pos;

  int r = read_int_line(buf, len, 0, '*', &count, &pos);
  if (r <= 0) return r;
  if (count < 1 || count > MAX_ARGS) return -1;

  for (long k = 0; k < count; k++) {
    long blen;
    size_t after_header;
    r = read_int_line(buf, len, pos, '$', &blen, &after_header);
    if (r <= 0) return r;
    pos = after_header;

    if (pos + (size_t)blen + 2 > len) return 0;
    if (buf[pos + blen] != '\r' || buf[pos + blen + 1] != '\n') return -1;

    args[k].ptr = buf + pos;
    args[k].len = (size_t)blen;
    pos += (size_t)blen + 2;
  }

  *argc = (int)count;
  return (int)pos;
}

int write_data(int fd, const arg_t* arg) {
  char hdr[32];
  int hlen = snprintf(hdr, sizeof(hdr), "$%zu\r\n", arg->len);
  if (hlen < 0 || (size_t)hlen >= sizeof(hdr)) return -1;
  if (write_all(fd, hdr, (size_t)hlen) < 0) return -1;
  if (write_all(fd, arg->ptr, arg->len) < 0) return -1;
  return write_all(fd, "\r\n", 2);
}
