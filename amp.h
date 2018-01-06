#ifndef AMP_H
#define AMP_H



#include <argp.h>
#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <sys/stat.h>



#define MIN_FD 3
#define MAX_FD UINT_MAX

#define STATUS_TYPE_NONE 0
#define STATUS_TYPE_SIZE 1
#define STATUS_TYPE_TIME 2

#define DEF_IGN_STDOUT     0
#define DEF_BUFFER_SIZE 4096
#define DEF_COUNT          0
#define DEF_LAST_FD       10
#define DEF_LINES          0
#define DEF_STATUS_RATE    0
#define DEF_STATUS_TYPE STATUS_TYPE_NONE

#define TIMESTAMP_LENGTH 8



#define ERR_IMBALANCE 1
#define ERR_EOF       2
#define ERR_WRITE     3
#define ERR_FLUSH     4



struct args
{
    int                 ign_stdout;
    unsigned long long  buffer_size;
    unsigned long long  count;
    unsigned int        last_fd;
    unsigned long long  lines;
    unsigned long long  status_rate;
    int                 status_type;
    char               *raw_buffer_size;
    char               *raw_count;
    char               *raw_last_fd;
    char               *raw_lines;
    char               *raw_status_rate;
};

struct stream
{
    FILE          *file;
    struct stream *next;
    int            index;
    int            state;
};



extern struct args args;
extern struct argp argp;

extern struct stream *head;
extern struct stream *tail;

extern const char *SIZE_SUFFIX[];
extern const char *TIME_SUFFIX[];



int amplify(int streams);
error_t parse_opt(int key, char *arg, struct argp_state *state);

void log_core(const char *preamble, const char *format, va_list vl);
void log_info(const char *format, ...);
void log_warning(const char *format, ...);
void log_error(const char *format, ...);
void log_critical(const char *format, ...);

int get_buffer_size(char *arg, unsigned long long *buffer_size);
int get_count(char *arg, unsigned long long *count);
int get_last_fd(char *arg, unsigned int *last_fd);
int get_lines(char *arg, unsigned long long *lines);
int get_status_rate(char *arg, unsigned long long *status_rate, int *status_type);

int list_add(FILE *file, int index);
void list_clear();
int open_streams();

int has_any_stream();
void format_size(unsigned long long value, double *result, int *suffix);
void format_time(double value, double *result, int *suffix);
void print_status(unsigned long long total, double elapsed_time);



#endif /* AMP_H */
