#include <stdio.h>
#include <i3/ipc.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "i3.h"

const char *i3_dir_name[] = {
    "up",
    "down",
    "left",
    "right"
};

struct __attribute__((__packed__)) i3_ipc_message {
    char magic_str[6];
    int32_t payload_size;
    int32_t type;
    char payload[I3_MAX_PAYLOAD_SIZE];
};

size_t i3_create_ipc_message(int32_t type, const char *payload, int32_t payload_size, struct i3_ipc_message **message){
    *message = malloc(sizeof(struct i3_ipc_message));
    memcpy((*message)->magic_str, "i3-ipc", 6);
    (*message)->payload_size = payload_size;
    (*message)->type = type;
    memcpy((*message)->payload, payload, strlen(payload));

    return sizeof(struct i3_ipc_message) + sizeof(payload);
}

int i3_ipc_connect(){
    int fd;
    struct sockaddr_un addr;

    char socket_path[1024], *iter = socket_path;
    get_cmd_output("i3 --get-socketpath", socket_path, 1024);

    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path, socket_path);

    if((fd = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0){
        fprintf(stderr, "socket error");
        exit(1);
    }

    if(connect(fd, (struct sockaddr *) &addr, sizeof(addr)) != 0){
        fprintf(stderr, "socket connect error");
        exit(1);
    }

    return fd;
}

void i3_ipc_send(int fd, struct i3_ipc_message *message, size_t message_bytes){
    int bytes_received;
    char buffer[1024];

    if(send(fd, message, message_bytes, 0) == -1){
        fprintf(stderr, "socket send error");
        exit(1);
    }

    while((bytes_received = recv(fd, buffer, 1024, 0)) > 0){
        struct i3_ipc_message *message = (struct i3_ipc_message *) &buffer;
        printf("magic_str: %s\n", message->magic_str);
        printf("payload_size: %u\n", message->payload_size);
        printf("type: %u\n", message->type);
        printf("payload: %s\n", message->payload);
    }
}

static size_t i3_create_focus_message(Direction direction, struct i3_ipc_message **message){
    char buffer[32];
    snprintf(buffer, 32, "focus %s", i3_dir_name[direction]);
    return i3_create_ipc_message(0x00, buffer, strlen(buffer), message);
}

void i3_move_focus(Direction direction){
    int fd = i3_ipc_connect();
    struct i3_ipc_message *message;
    size_t message_bytes = i3_create_focus_message(direction, &message);
    i3_ipc_send(fd, message, message_bytes);
}

