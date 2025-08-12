#ifndef UTILS_H
#define UTILS_H


#include "main_inc.h"
extern volatile sig_atomic_t stop_threads;


int check_valid_ip(const char* ip);

void get_lines_from_file(const char * file);

void help(void);

struct app_config 
{
    char *ip;
    int port;
    int QPS;
    char *file;
    int number_of_threads;
    int working_time;
};
int parse_arguments(int argc, char *argv[], struct app_config *config);

#endif