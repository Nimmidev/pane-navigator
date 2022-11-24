#ifndef _I3_H
#define _I3_H

#include <stdlib.h>
#include <inttypes.h>

#include "common.h"

#define I3_MAX_PAYLOAD_SIZE 1024

extern const char *i3_dir_name[];
struct i3_ipc_message;

int i3_ipc_connect();
size_t i3_create_ipc_message(int32_t type, const char *payload, int32_t payload_size, struct i3_ipc_message **message);
void i3_ipc_send(int fd, struct i3_ipc_message *message, size_t message_bytes);

void i3_move_focus(Direction direction);


#endif
