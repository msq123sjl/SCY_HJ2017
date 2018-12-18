#ifndef __NGINX_HELPER__
#define __NGINX_HELPER__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define ngx_string(str)     { sizeof(str) - 1, (u_char *) str }
#define ngx_null_string     { 0, NULL }
#define ngx_str_set(str, text)                                               \
    (str)->len = sizeof(text) - 1; (str)->data = (u_char *) text
#define ngx_str_null(str)   (str)->len = 0; (str)->data = NULL

#define ngx_tolower(c)      (u_char) ((c >= 'A' && c <= 'Z') ? (c | 0x20) : c)
#define ngx_toupper(c)      (u_char) ((c >= 'a' && c <= 'z') ? (c & ~0x20) : c)

typedef intptr_t        ngx_int_t;
typedef uintptr_t       ngx_uint_t;

#define  NGX_OK          0
#define  NGX_ERROR      -1

#define NGX_INT32_LEN   (sizeof("-2147483648") - 1)

#define NGX_INT_T_LEN   NGX_INT32_LEN
#define NGX_MAX_INT_T_VALUE  2147483647


#define ngx_str2cmp(m, c0, c1)                                       \
	m[0] == c0 && m[1] == c1

#define ngx_str3cmp(m, c0, c1, c2)                                       \
    ((*(uint32_t *) m) & 0xffffff) == ((c2 << 16) | (c1 << 8) | c0)

#define ngx_str4cmp(m, c0, c1, c2, c3)                                        \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)

#define ngx_str5cmp(m, c0, c1, c2, c3, c4)                                    \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)             \
        && m[4] == c4

#define ngx_str6cmp(m, c0, c1, c2, c3, c4, c5)                                \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)             \
        && (((uint32_t *) m)[1] & 0xffff) == ((c5 << 8) | c4)

#define ngx_str7cmp(m, c0, c1, c2, c3, c4, c5, c6)                       \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)             \
        && (((uint32_t *) m)[1] & 0xffffff) == ((c6 << 16) | (c5 << 8) | c4)

#define ngx_str8cmp(m, c0, c1, c2, c3, c4, c5, c6, c7)                        \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)             \
        && ((uint32_t *) m)[1] == ((c7 << 24) | (c6 << 16) | (c5 << 8) | c4)

#define ngx_str9cmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8)                    \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)             \
        && ((uint32_t *) m)[1] == ((c7 << 24) | (c6 << 16) | (c5 << 8) | c4)  \
        && m[8] == c8

#define ngx_str10cmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9)               \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)             \
        && ((uint32_t *) m)[1] == ((c7 << 24) | (c6 << 16) | (c5 << 8) | c4)  \
        && (((uint32_t *) m)[2] & 0xffff) == ((c9 << 8) | c8)

#define ngx_str11cmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10)               \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)             \
        && ((uint32_t *) m)[1] == ((c7 << 24) | (c6 << 16) | (c5 << 8) | c4)  \
        && (((uint32_t *) m)[2] & 0xffffff) == ((c10 << 16) | (c9 << 8) | c8)

#define ngx_str12cmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11)     \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)             \
        && ((uint32_t *) m)[1] == ((c7 << 24) | (c6 << 16) | (c5 << 8) | c4)  \
        && ((uint32_t *) m)[2] == ((c11 << 24) | (c10 << 16) | (c9 << 8) | c8)

typedef struct {
    size_t      len;
    u_char     *data;
} ngx_str_t;

static inline u_char *
ngx_strlchr(u_char *p, u_char *last, u_char c)
{
    while (p < last) {

        if (*p == c) {
            return p;
        }

        p++;
    }

    return NULL;
}

ngx_int_t ngx_isnumbers(u_char *line, size_t n);
ngx_int_t ngx_atoi(u_char *line, size_t n);
ngx_int_t ngx_hextoi(u_char *line, size_t n);

#endif

