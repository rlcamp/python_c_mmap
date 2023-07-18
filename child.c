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

int main(void) {
    /* assume parent has dup2'd STDOUT_FILENO to something we can ftruncate and mmap */
    const size_t size = sizeof(float[2]);

    /* resize the fd to the number of bytes we want to pass to the parent */
    if (-1 == ftruncate(STDOUT_FILENO, size)) NOPE("child: ftruncate(): %s\n", strerror(errno));

    float * out = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, STDOUT_FILENO, 0);
    if (MAP_FAILED == out) NOPE("child: mmap(): %s\n", strerror(errno));

    /* store some values in the shared memory */
    out[0] = 24601.0f;
    out[1] = 31.7f;

    /* clean up */
    if (-1 == munmap(out, size)) NOPE("child: munmap(): %s\n", strerror(errno));
}
