// Copyright 2018 Matthias Schwarz
// License: MIT

#include "amp.h"

const char *SIZE_SUFFIX[] = { "B", "KB", "MB", "GB", "TB", "EB" };
const char *TIME_SUFFIX[] = { "s", "m", "h", "d" };

int has_any_stream()
{
    struct stream *current = head;
    
    while (current != NULL)
    {
        if (current->state == 0)
        {
            return 1;
        }
        current = current->next;
    }
    
    return 0;
}

void format_size(unsigned long long value, double *result, int *suffix)
{
    int    e = 0.0;
    double n = 0.0;
    
    e = fmin(floor(log10(value) / 3), 5);
    n = value / pow(1000, e);
    
    *result = n;
    *suffix = e;
}

void format_time(double value, double *result, int *suffix)
{
    if (value < 60)
    {
        *result = value;
        *suffix = 0;
    }
    else if (value < 3600)
    {
        *result = value / 60;
        *suffix = 1;
    }
    else if (value < 86400)
    {
        *result = value / 60 / 60;
        *suffix = 2;
    }
    else
    {
        *result = value / 60 / 60 / 24;
        *suffix = 3;
    }
}

void print_status(unsigned long long total, double elapsed_time)
{
    double size    = 0.0;
    int    sizesuf = 0;
    double time    = 0.0;
    int    timesuf = 0;
    double speed   = 0.0;
    double speedf  = 0.0;
    int    spdsufx = 0;
    
    format_size(total, &size, &sizesuf);
    format_time(elapsed_time, &time, &timesuf);
    
    if (elapsed_time <= 0.0)
    {
        // If no time has elapsed, we can't determine the speed
        
        log_info("%9.2f %s    %6.2f %s    (? %s/s)\n", size, SIZE_SUFFIX[sizesuf], time, TIME_SUFFIX[timesuf], SIZE_SUFFIX[spdsufx]);
    }
    else
    {
        speed = total / elapsed_time;
        
        format_size(speed, &speedf, &spdsufx);
        
        log_info("%9.2f %s    %6.2f %s    (%5.2f %s/s)\n", size, SIZE_SUFFIX[sizesuf], time, TIME_SUFFIX[timesuf], speedf, SIZE_SUFFIX[spdsufx]);
    }
}
