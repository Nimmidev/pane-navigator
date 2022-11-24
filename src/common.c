#include "common.h"

void get_cmd_output(const char *cmd, char *buffer, size_t buffer_size){
    FILE *fp = popen(cmd, "r");
    fread(buffer, 1, buffer_size, fp);
    fclose(fp);
}
