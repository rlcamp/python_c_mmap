#!/usr/bin/env python3
# creates an anonymous fd, and forks and execs a child process, which mmaps the fd and fills
# it. after the child exits, the parent can mmap the fd for reading and do whatever it wants
# with the bytes, resulting in zero-copy return of possibly large amounts of output to the
# calling code, across a process and language boundary

# two-way zero-copy ipc between the parent and child would also be possible using this method

import mmap
import os
import stat
import fcntl
import subprocess
import sys

# shim for memfd_create on non-linux posix systems (macos, &c.) and older linux/python
try: from os import memfd_create
except:
    try: from _posixshmem import shm_open, shm_unlink
    except:
        def shm_open(name, flags, access):
            return os.open('/dev/shm/' + name, flags | os.O_CLOEXEC, access)
        def shm_unlink(name):
            return os.unlink('/dev/shm/' + name)

    def memfd_create(name):
        tmp = "arr" + str(os.getpid()) + name
        fd = shm_open(tmp, os.O_RDWR | os.O_CREAT | os.O_EXCL, stat.S_IRUSR | stat.S_IWUSR)
        if -1 == fd: raise RuntimeError(os.strerror(ctypes.get_errno()))

        # immediately delete the temporary "file" (it never actually appears on disk in macos)
        if -1 == shm_unlink(tmp): raise RuntimeError(os.strerror(ctypes.get_errno()))

        return fd

def main():
    # get an fd
    fd = memfd_create('')

    # discard the cloexec flag (not necessary if passing to a child's stdout or stdin)
    fcntl.fcntl(fd, fcntl.F_SETFD, fcntl.fcntl(fd, fcntl.F_GETFD) & ~fcntl.FD_CLOEXEC)

    # run a child process, wait for it to finish, and get its return status
    child = subprocess.Popen(['./child', str(fd)], bufsize=0, stdin=subprocess.PIPE, stdout=subprocess.PIPE, close_fds=False)
    child_ret = child.wait()
    if (child_ret != 0): raise RuntimeError("child exited nonzero")

    # do an mmap of the size of the fd (which may be rounded up to page size, don't trust it)
    map = mmap.mmap(fd, os.fstat(fd).st_size, prot=mmap.PROT_READ)
    os.close(fd)

    # get a pointer to the memory as an array of floats
    try: import numpy
    except: out = memoryview(map).cast('f')
    else: out = numpy.ndarray(buffer=memoryview(map), dtype='f', shape=[2])

    # do something to show we can use the bytes
    print("child returns: %g %g, accessible as a %s" % (out[0], out[1], type(out)))

main()
