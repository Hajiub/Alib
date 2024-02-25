#include <fcntl.h>
#include <stdlib.h>
// Include this header for read and write functions
#include <unistd.h> 

#define EOF (-1)

#define MAX 20
#define BUFSIZE 1024

typedef struct {
    int fd;
    int cnt;
    char *base;
    char *ptr;
    int flag;
} file;

file files[MAX] = {
    { 0, 0, NULL, NULL, 1 }, // Use NULL instead of (char *)0
    { 1, 0, NULL, NULL, 1 }  // Use NULL instead of (char *)0
};

#define stdin (&files[0])
#define stdout (&files[1])

#define getc(p) (--(p)->cnt >= 0 ? (unsigned char) *(p)->ptr++ : fillbuffer(p))
#define putc(x, p) (--(p)->cnt >= 0 ? *(p)->ptr++ = (x) : flushbuf((x), p))

#define getchar() getc(stdin)
#define putchar(c) putc(c, stdout)


int fillbuffer(file *fp) {
    if (fp->base == NULL) {
        if ((fp->base = (char *)malloc(BUFSIZE)) == NULL) {
            return EOF;
        }
    }
    fp->ptr = fp->base;
    fp->cnt = read(fp->fd, fp->ptr, BUFSIZE);
    if (--fp->cnt < 0) {
        if (fp->cnt == -1)
            return EOF;
        else
            return -24;
        fp->cnt = 0;
        return EOF;
    }
    return (unsigned char) *fp->ptr++;
}

int flushbuf(int c, file *fp) {
    int num_written, bufsize;

    if (fp->base == NULL) {
        if ((fp->base = (char *)malloc(BUFSIZE * sizeof(char))) == NULL) {
            return EOF;
        }
    }

    bufsize = (int)(fp->ptr - fp->base);
    num_written = write(fp->fd, fp->base, bufsize);
    fp->ptr = fp->base;
    fp->cnt = BUFSIZE - 1;

    if (num_written == bufsize) {
        return c;
    }

    return EOF;
}

#define PERMS 0666

// read and write only 
file *fopen(const char *name, const char *mode)
{
    if (*mode != 'r' && *mode != 'w')
        return NULL;
    file *temp;
    int fd = 0;
    for (temp = files; temp < files + MAX; temp++ )
    {
        if(temp->flag == 0)
            break;
    }

    if (temp >= files + MAX)
        return NULL;

    if (*mode == 'r')
       fd  = open(name, O_RDONLY);

    else if (*mode == 'w')
        fd = creat(name, PERMS);
    if (fd == -1)
        return NULL;

    temp->fd = fd;
    temp->cnt = 0;
    temp->flag = 1;
    temp->base = NULL;
    return temp;
}

int main(int ac, char **av) 
{
    (void)ac;

    file *f = fopen(av[1], "r");
    if (f == NULL)
        return -1; 
    int c;
    while ((c = getc(f)) != EOF)
        putchar(c);
    return 0;
}

