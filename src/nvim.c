#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <msgpack.h>

#include "nvim.h"
#include "tmux.h"

#define NVIM_MAX_OPEN_SPLITS 32
#define ASSERT(s) assert(s);
// #define DEBUG(s) s
#define DEBUG(s)

typedef struct {
    int16_t arr[NVIM_MAX_OPEN_SPLITS];
    size_t length;
} WindowIdArray;

typedef struct {
    int32_t x;
    int32_t y;
} WindowPos;

typedef struct {
    int fd;
    int64_t msg_id;
    int16_t win_id;
    WindowPos win_pos;
    WindowIdArray win_ids;
} NvimState;

typedef struct __attribute__((__packed__)) {
    char type;
    int16_t value;
} msgpack_int16;

static inline void msgpack_int16_init(msgpack_int16 *var, int16_t value){
    var->type = 0xcd;
    var->value = ntohs(value);
}

static inline int16_t msgpack_int16_unpack(const char *ptr){
    int16_t tmp = (*(msgpack_int16 *)ptr).value;
    return htons(tmp);
}

static void nvim_unpack_list_wins(msgpack_object *result, void *out);
static void nvim_pack_window_id(msgpack_packer *packer, void *args);
static void nvim_unpack_win_get_position(msgpack_object *result, void *out);
static void nvim_unpack_get_current_win(msgpack_object *result, void *out);

typedef struct {
    const char *name;
    void (*pack)(msgpack_packer *packer, void *args);
    void (*unpack)(msgpack_object *result, void *out);
} NvimCommand ;

static NvimCommand nvim_list_wins = {
    .name = "nvim_list_wins",
    .pack = NULL,
    .unpack = nvim_unpack_list_wins
};

static NvimCommand nvim_win_get_position = {
    .name = "nvim_win_get_position",
    .pack = nvim_pack_window_id,
    .unpack = nvim_unpack_win_get_position
};

static NvimCommand nvim_set_current_win = {
    .name = "nvim_set_current_win",
    .pack = nvim_pack_window_id,
    .unpack = NULL
};

static NvimCommand nvim_get_current_win = {
    .name = "nvim_get_current_win",
    .pack = NULL,
    .unpack = nvim_unpack_get_current_win
};


static int nvim_ipc_connect(const char *socket_path){
    int fd;
    struct sockaddr_un addr;

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

static void nvim_pack(msgpack_sbuffer *sbuf, int64_t msg_id, NvimCommand *command, void *args){
    msgpack_packer packer;
    msgpack_packer_init(&packer, sbuf, msgpack_sbuffer_write);

    msgpack_pack_array(&packer, 4);
    msgpack_pack_int(&packer, 0);
    msgpack_pack_int(&packer, msg_id);

    size_t method_length = strlen(command->name);
    msgpack_pack_str(&packer, method_length);
    msgpack_pack_str_body(&packer, command->name, method_length);

    if(command != NULL){
        if(command->pack != NULL){
            command->pack(&packer, args);
        } else {
            msgpack_pack_array(&packer, 0);
        }
    }
}

static void nvim_unpack(msgpack_sbuffer *sbuf, NvimCommand *command, void *out){
    msgpack_unpacked result;
    msgpack_unpacked_init(&result);
    msgpack_unpack_next(&result, sbuf->data, sbuf->size, NULL);
    DEBUG(msgpack_object_print(stdout, result.data);)
    DEBUG(printf("\n");)

    if(command != NULL && command->unpack != NULL) command->unpack(&result.data, out);
}

static void nvim_unpack_list_wins(msgpack_object *result, void *out){
    ASSERT(result->type != 0)
    msgpack_object_array ids = result->via.array.ptr[3].via.array;
    ASSERT(ids.size <= NVIM_MAX_OPEN_SPLITS)

    WindowIdArray *arr = out;
    arr->length = ids.size & 0xff;
    for(int i = 0; i < ids.size; i++){
        *(arr->arr + i) = msgpack_int16_unpack(ids.ptr[i].via.ext.ptr);
    }
}

static void nvim_pack_window_id(msgpack_packer *packer, void *args){
    msgpack_pack_array(packer, 1);
    msgpack_int16 window_id;
    msgpack_int16_init(&window_id, *(int16_t *)args);
    msgpack_pack_ext(packer, sizeof(window_id), 1);
    msgpack_pack_ext_body(packer, &window_id, sizeof(window_id));
}

static void nvim_unpack_win_get_position(msgpack_object *result, void *out){
    ASSERT(result->type != 0)
    msgpack_object object = result->via.array.ptr[3];
    WindowPos *pos = out;
    pos->y = object.via.array.ptr[0].via.i64;
    pos->x = object.via.array.ptr[1].via.i64;
}

static void nvim_unpack_get_current_win(msgpack_object *result, void *out){
    msgpack_object object = result->via.array.ptr[3];
    ASSERT(object.via.ext.size == 3)
    *(int16_t *)out = msgpack_int16_unpack(object.via.ext.ptr);
}

static bool nvim_rpc(NvimState *nvim, NvimCommand *command, void *args, void *out){
    msgpack_sbuffer sbuf;
    msgpack_sbuffer_init(&sbuf);

    DEBUG(printf("SND: ");)
    nvim_pack(&sbuf, nvim->msg_id++, command, args);
    DEBUG(nvim_unpack(&sbuf, NULL, NULL);)

    if(send(nvim->fd, sbuf.data, sbuf.size, 0) == -1){
        fprintf(stderr, "Error sending data\n");
        return false;
    }

    msgpack_sbuffer_destroy(&sbuf);

    char buffer[1024] = {0};
    sbuf.size = 0;
    sbuf.alloc = 1024;
    sbuf.data = buffer;

    if((sbuf.size = recv(nvim->fd, sbuf.data, sbuf.alloc, 0)) == -1){
        fprintf(stderr, "Error receiving data\n");
        return false;
    }

    DEBUG(printf("RCV: ");)
    nvim_unpack(&sbuf, command, out);
    DEBUG(printf("\n");)

    return true;
}

static bool nvim_get_win_in_direction(NvimState *nvim, Direction direction, int16_t *window_id){
    if(direction == DIRECTION_UP){
        if(nvim->win_pos.y == 0) return false;
    } else if(direction == DIRECTION_LEFT){
        if(nvim->win_pos.x == 0) return false;
    }

    int best_value = INT_MAX;
    WindowPos pos;

    if(direction == DIRECTION_UP || direction == DIRECTION_LEFT) best_value = INT_MIN;

    for(int i = 0; i < nvim->win_ids.length; i++){
        if(nvim->win_ids.arr[i] == nvim->win_id) continue;
        if(!nvim_rpc(nvim, &nvim_win_get_position, &nvim->win_ids.arr[i], &pos)) exit(1);

        pos.x = pos.x - nvim->win_pos.x;
        pos.y = pos.y - nvim->win_pos.y;

        if(direction == DIRECTION_UP){
            if(pos.y < 0 && pos.y > best_value){
                best_value = pos.y;
                *window_id = nvim->win_ids.arr[i];
            }
        } else if(direction == DIRECTION_DOWN){
            if(pos.y > 0 && pos.y < best_value){
                best_value = pos.y;
                *window_id = nvim->win_ids.arr[i];
            }
        } else if(direction == DIRECTION_LEFT){
            if(pos.x < 0 && pos.x > best_value){
                best_value = pos.x;
                *window_id = nvim->win_ids.arr[i];
            }
        } else if(direction == DIRECTION_RIGHT){
            if(pos.x > 0 && pos.x < best_value){
                best_value = pos.x;
                *window_id = nvim->win_ids.arr[i];
            }
        }
    }

    return *window_id != 0;
}

bool nvim_move_focus(const char *socket_path, Direction direction){
    NvimState nvim = {0};
    nvim.fd = nvim_ipc_connect(socket_path);

    if(!nvim_rpc(&nvim, &nvim_list_wins, NULL, &nvim.win_ids)) exit(1);

    if(nvim.win_ids.length != 1){
        if(!nvim_rpc(&nvim, &nvim_get_current_win, &nvim.win_id, &nvim.win_id)) exit(1);
        if(!nvim_rpc(&nvim, &nvim_win_get_position, &nvim.win_id, &nvim.win_pos)) exit(1);

        int16_t window_id;
        if(nvim_get_win_in_direction(&nvim, direction, &window_id)){
            return nvim_rpc(&nvim, &nvim_set_current_win, &window_id, NULL);
        }
    }

    return false;
}

void nvim_get_socket_path(int pid, char *buffer, size_t buffer_size){
    char pid_str[10];
    snprintf(pid_str, 10, "%d", pid);

    memset(buffer, 0, buffer_size);
    strcpy(buffer, getenv("XDG_RUNTIME_DIR"));
    strcat(buffer, "/nvim.");
    strcat(buffer, pid_str);
    strcat(buffer, ".0");
}

bool nvim_get_pid(const char *tmux_session_id, int *nvim_pid){
    char buffer[32];
    int pid = tmux_get_pane_pid(tmux_session_id);
    if(pid == 0) return false;

    *nvim_pid = get_process_child_pid(pid);
    if(*nvim_pid == 0) return false;
    get_process_cmdline(*nvim_pid, buffer, 32);

    return strcmp(buffer, "nvim") == 0;
}

