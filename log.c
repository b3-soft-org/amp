// Copyright 2017 Matthias Schwarz
// License: MIT

#include "amp.h"

void log_core(const char *preamble, const char *format, va_list vl)
{
    time_t now;
    struct tm *local;
    char timef[TIMESTAMP_LENGTH + 1];
    
    now = time(NULL);
    local = localtime(&now);
    strftime(timef, sizeof(timef), "%H:%M:%S", local);
    
    fprintf(stderr, "%s  %s  ", timef, preamble);
    vfprintf(stderr, format, vl);
}

void log_info(const char *format, ...)
{
    va_list vl;
    va_start(vl, format);
    log_core("[i]", format, vl);
    va_end(vl);
}

void log_warning(const char *format, ...)
{
    va_list vl;
    va_start(vl, format);
    log_core("[w]", format, vl);
    va_end(vl);
}

void log_error(const char *format, ...)
{
    va_list vl;
    va_start(vl, format);
    log_core("[E]", format, vl);
    va_end(vl);
}

void log_critical(const char *format, ...)
{
    va_list vl;
    va_start(vl, format);
    log_core("[!]", format, vl);
    va_end(vl);
}
