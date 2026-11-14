#ifndef COMMANDS_H
#define COMMANDS_H

#include "resp.h"

int execute_command(int fd, const arg_t* args, int argc);

#endif
