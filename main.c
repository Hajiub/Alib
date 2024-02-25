#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h> // Include this header for read and write functions

#define EOF (-1)

#define MAX 20
#define BUFSIZE 1024

typedef struct {
    int fd;
    int cnt;
    char *base;
    char *ptr;
} file;

file files[MAX] = {
    { 0, 0, NULL, NULL }, // Use NULL instead of (char *)0
    { 1, 0, NULL, NULL }  // Use NULL instead of (char *)0
};

#define stdin (&files[0])
#define stdout (&files[1])

int fillbuffer(file *fp) {
    if (fp->base == NULL) {
        if ((fp->base = (char *)malloc(BUFSIZE)) == NULL) {
            return EOF;
        }
    }
    fp->ptr = fp->base;
    fp->cnt = read(fp->fd, fp->ptr, BUFSIZE);
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

file *fopen(const char *name, const char *mode)
{
    return NULL;
}

int main(void) {
    int c;
    while ((c = getchar()) != EOF) // Check for EOF explicitly
        putchar(c);
    return 0;
}

