#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define NOPE(...) do { fprintf(stderr, "error: " __VA_ARGS__); exit(EXIT_FAILURE); } while(0)

int main(const int argc, char ** const argv) {
    /* assume parent has passed the file descriptor as a single command line argument */
    if (argc < 2) NOPE("%s: don't call this directly\n", __FILE__);
    const int fd = strtol(argv[1], NULL, 10);

    const size_t size = sizeof(float[2]);

    /* resize the fd to the number of bytes we want to pass to the parent */
    if (-1 == ftruncate(fd, size)) NOPE("child: ftruncate(): %s\n", strerror(errno));

    float * out = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (MAP_FAILED == out) NOPE("child: mmap(): %s\n", strerror(errno));
    if (-1 == close(fd)) NOPE("child: close(): %s\n", strerror(errno));

    /* store some values in the shared memory */
    out[0] = 24601.0f;
    out[1] = 31.7f;

    /* clean up */
    if (-1 == munmap(out, size)) NOPE("child: munmap(): %s\n", strerror(errno));
}
