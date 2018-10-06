#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    
#define LOG_PATH 	  "./bankmanager"
#define LOG_PATH_BACK "./bankmanager_back"


#define SAVE_LINE 1000
#define COMMAND_LEN 512
#define BUF_LEN 1024


void write_running_log(const char *cmd,const char* name);

#ifdef __cplusplus
}
#endif

