#ifndef AMP_H
#define AMP_H



#include <argp.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <time.h>



#define STATUS_TYPE_NONE 0
#define STATUS_TYPE_SIZE 1
#define STATUS_TYPE_TIME 2

#define DEF_USE_STDERR 0
#define DEF_USE_SYSLOG 0
#define DEF_IGN_STDOUT 0
#define DEF_BUFFER_SIZE 4096
#define DEF_STATUS_RATE 0
#define DEF_STATUS_TYPE STATUS_TYPE_NONE
#define DEF_LAST_FD 10



#define ERR_IMBALANCE   -1
#define ERR_EOF         -2
#define ERR_WRITE       -3
#define ERR_FLUSH       -4



struct Arguments
{
    int use_stderr;
    int use_syslog;
    int ign_stdout;
    unsigned long long buffer_size;
    unsigned long long status_rate;
    int status_type;
    int last_fd;
    char *raw_buffer_size;
    char *raw_status_rate;
    char *raw_last_fd;
};

struct Stream {
    FILE          *file;
    struct Stream *next;
    int            index;
    int            status;
};



extern struct Arguments args;
extern struct argp argp;

extern struct Stream *head;
extern struct Stream *tail;



void log_info(const char *format, ...);
void log_warning(const char *format, ...);
void log_error(const char *format, ...);
void log_critical(const char *format, ...);

error_t parse_opt(int key, char *arg, struct argp_state *state);
int get_buffer_size(char *arg, unsigned long long *buffer_size);
int get_status_rate(char *arg, unsigned long long *status_rate, int *status_type);
int get_last_fd(char *arg, int *last_fd);

int list_add(FILE *file, int index);
void list_clear();
int open_streams();



#endif /* AMP_H */
