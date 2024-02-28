#ifndef ALIB_H
#define ALIB_H

#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdarg.h>

#ifdef NULL
#undef NULL
#endif

#define NULL 0
#define EOF (-1)
#define BUFSIZ 1024
#define FOPEN_MAX 20 

typedef struct _iobuf {
    int cnt;
    char *ptr;
    char *base;
    int flag;
    int fd;     
} FILE;

extern FILE _iob[FOPEN_MAX];

#define stdin (&_iob[0])
#define stdout (&_iob[1])
#define stderr (&_iob[2])

enum _flags {
    _READ    = 0x1,
    _WRITE   = 0x2,
    _UNBUF   = 0x3,
    _EOF     = 0x8,
    _ERR     = 0x10
};

int _fillbuf(FILE *);
int _flushbuf(int, FILE *);

#define feof(p) (((p)->flag & _EOF) != 0)
#define ferror(p) (((p)->flag & _ERR) != 0)
#define fileno(p) ((p)->fd)

#define getc(p) (--(p)->cnt >= 0 ? (unsigned char)*(p)->ptr++ : _fillbuf(p))

#define putc(x, p) (--(p)->cnt >= 0 ? *(p)->ptr++ = (x) : _flushbuf((x), p))

#define getchar() getc(stdin)
#define putchar(x) putc((x), stdout)

#define PERMS 0666
int fflush(FILE *);
int fputs(const char *s, FILE *stream);
FILE *file_open(const char *, const char *);
int file_close(FILE *);
int fprintf(FILE *stream, const char *format, ...);
int printf(const char *format, ...);

#endif //ALIB_H
