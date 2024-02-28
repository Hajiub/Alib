#include "alib.h"

int _fillbuf(FILE *fp)
{
    int bufsize;

    if ((fp->flag & (_READ | _EOF | _ERR)) != _READ)
        return EOF;

    bufsize = (fp->flag & _UNBUF) ? 1 : BUFSIZ;

    if (fp->base == NULL)
        if ((fp->base = (char *)malloc(bufsize)) == NULL)
            return EOF;

    fp->ptr = fp->base;
    fp->cnt = read(fp->fd, fp->ptr, bufsize);

    if (--fp->cnt < 0) {
        if (fp->cnt == -1)
            fp->flag |= _EOF;
        else
            fp->flag |= _ERR;
        fp->cnt = 0;
        return EOF;
    }

    return (unsigned char)*fp->ptr++;
}

int _flushbuf(int c, FILE *f)
{
    int num_written, bufsize;
    unsigned char uc = c;

    if ((f->flag & (_WRITE | _EOF | _ERR)) != _WRITE) {
        return EOF;
    }

    if (f->base == NULL && ((f->flag & _UNBUF) == 0)) {
        if ((f->base = malloc(BUFSIZ)) == NULL)
            f->flag |= _UNBUF;
        else {
            f->ptr = f->base;
            f->cnt = BUFSIZ - 1;
        }
    }

    if (f->flag & _UNBUF) {
        f->ptr = f->base = NULL;
        f->cnt = 0;
        if (c == EOF) {
            return EOF;
        }
        num_written = write(f->fd, &uc, 1);
        bufsize = 1;
    } else {
        bufsize = (int)(f->ptr - f->base);
        num_written = write(f->fd, f->base, bufsize);
        f->ptr = f->base;
        f->cnt = BUFSIZ - 1;
    }

    if (num_written == bufsize)
        return c;
    else {
        f->flag |= _ERR;
        return EOF;
    }
}

int fflush(FILE *stream)
{
    int retval;
    FILE *temp;

    retval = 0;

    if (stream == NULL) {
        for (temp = _iob; temp < _iob + FOPEN_MAX; temp++)
            if ((temp->flag & _WRITE) == 0 && fflush(stream) == -1)
                retval = -1;
    } else {
        if ((stream->flag & _WRITE) == 0)
            return -1;
        _flushbuf(EOF, stream);
        if (stream->flag & _ERR)
            retval = -1;
    }
    return retval;
}

int fputs(const char *s, FILE *stream)
{
    int res = 0;
    while (*s)
    {
        res += putc(*s++, stream);
    }
    return res;
}
FILE *file_open(const char *name, const char *mode) 
{
    int fd;
    FILE *fp;

    if (*mode != 'r' && *mode != 'w' && *mode != 'a')
        return NULL;

    for (fp = _iob; fp < _iob + FOPEN_MAX; fp++)
        if ((fp->flag & (_READ | _WRITE)) == 0)
            break;

    if (fp >= _iob + FOPEN_MAX)
        return NULL;

    if (*mode == 'w')
        fd = creat(name, PERMS);
    else if (*mode == 'a') {
        if ((fd = open(name, O_WRONLY, 0)) == -1)
            fd = creat(name, PERMS);
        lseek(fd, 0L, 2);
    } else
        fd = open(name, O_RDONLY, 0);

    if (fd == -1)
        return NULL;

    fp->fd = fd;
    fp->cnt = 0;
    fp->base = NULL;
    fp->flag = (*mode == 'r') ? _READ : _WRITE;
    return fp;
}



int file_close(FILE *stream)
{
    int fd;
    if (stream == NULL)
        return -1;

    fd = stream->fd;
    fflush(stream);
    stream->cnt = 0;
    stream->ptr = NULL;
    if (stream->base != NULL)
        free(stream->base);
    stream->base = NULL;
    stream->flag = 0;
    stream->fd = -1;
    return close(fd);
}

int fprintf(FILE *stream, const char *format, ...) {
    if (stream == NULL) {
        return EOF;
    }

    va_list ap;
    va_start(ap, format);
    for (const char *p = format; *p; p++) {
        if (*p != '%') {
            putc(*p, stream);
        } else {
            char conversion_type = *++p;

            switch (conversion_type) {
                case 'd':
                case 'i': {
                    int val = va_arg(ap, int);
                    putc(&val, stream);
                    break;
                }
               case 'f': {
                    double val = va_arg(ap, double);
                    fprintf(stream, "%f", val);
                    break;
                }
                case 's': {
                    const char *str = va_arg(ap, const char *);
                    if (str) {
                        fputs(str, stream);
                    } else {
                        
                        fputs("(null)", stream);
                    }
                    break;
                }
                default:
                    putc(*p, stream);
                    break;
            }
        }
    }
    va_end(ap);
    return 1;
}



FILE _iob[FOPEN_MAX] = {/* stdin, stdout, stderr */
                       {0, (char *)0, (char *)0, _READ, 0},
                       {0, (char *)0, (char *)0, _WRITE, 1},
                       {0, (char *)0, (char *)0, _WRITE | _UNBUF, 2}};


