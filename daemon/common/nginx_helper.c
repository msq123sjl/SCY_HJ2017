#include <stdio.h>   
#include <stdlib.h>
#include <ctype.h>   
#include <string.h>   
#include <time.h>
#include <dirent.h>   
#include <stdint.h>

#include "config_file.h"   
#include "tinz_base_def.h" 
#include "nginx_helper.h"
#include <unistd.h>

ngx_int_t ngx_isnumbers(u_char *line, size_t n)
{
    if (n == 0) {
        return NGX_ERROR;
    }

    for (; n--; line++) {
        if (*line < '0' || *line > '9') {
            return NGX_ERROR;
        }
    }

    return NGX_OK;
}


ngx_int_t ngx_atoi(u_char *line, size_t n)
{
    ngx_int_t  value, cutoff, cutlim;

    if (n == 0) {
        return NGX_ERROR;
    }

    cutoff = NGX_MAX_INT_T_VALUE / 10;
    cutlim = NGX_MAX_INT_T_VALUE % 10;

    for (value = 0; n--; line++) {
        if (*line < '0' || *line > '9') {
            return NGX_ERROR;
        }

        if (value >= cutoff && (value > cutoff || *line - '0' > cutlim)) {
            return NGX_ERROR;
        }

        value = value * 10 + (*line - '0');
    }

    return value;
}

ngx_int_t ngx_hextoi(u_char *line, size_t n)
{
    u_char     c, ch;
    ngx_int_t  value, cutoff;

    if (n == 0) {
        return NGX_ERROR;
    }

    cutoff = NGX_MAX_INT_T_VALUE / 16;

    for (value = 0; n--; line++) {
        if (value > cutoff) {
            return NGX_ERROR;
        }

        ch = *line;

        if (ch >= '0' && ch <= '9') {
            value = value * 16 + (ch - '0');
            continue;
        }

        c = (u_char) (ch | 0x20);

        if (c >= 'a' && c <= 'f') {
            value = value * 16 + (c - 'a' + 10);
            continue;
        }

        return NGX_ERROR;
    }

    return value;
}


