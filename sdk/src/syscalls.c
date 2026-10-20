#include <sys/stat.h>
#include <sys/types.h>
#include <stddef.h>
#include <errno.h>

extern char __heap_start[];
extern char __heap_limit[];

ssize_t _write(int file, const void *ptr, size_t len) {

}

ssize_t _read(int file, void *ptr, size_t len) {

}

void *_sbrk(ptrdiff_t increment) {
    static char *heap_end = __heap_start;

    uintptr_t current = (uintptr_t)heap_end;
    uintptr_t limit = (uintptr_t)__heap_limit;

    if (increment < 0) {
        errno = EINVAL;
        return (void *)-1;
    }

    if (current > limit || (uintptr_t)increment > limit - current) {
        errno = ENOMEM;
        return (void *)-1;
    }

    void *prev_end = heap_end;
    heap_end = (char *)(current + (uintptr_t)increment);

    return prev_end;
}

int _fstat(int file, struct stat *status) {
    if (file >= 0 && file <= 2) {
        status->st_mode = S_IFCHR;
        return 0;
    } else {
        errno = EBADF;
        return -1;
    }
}

int _isatty(int file) {
    if (file >= 0 && file <= 2) {
        return 1;
    } else {
        errno = EBADF;
        return 0;
    }
}

off_t _lseek(int file, off_t offset, int whence) {
    (void)file;
    (void)offset;
    (void)whence;

    if (file >= 0 && file <= 2) {
        errno = ESPIPE;
    } else {
        errno = EBADF;
    }

    return -1;
}

int _close(int file) {
    (void)file;

    errno = EBADF;
    return -1;
}

void _exit(int status) {
    (void)status;

    for (;;) {
        //infinite loop
    }
}

pid_t _getpid(void) {
    return (1);
}

int _kill(pid_t pid, int signal_number) {
    (void)pid;
    (void)(signal_number);

    errno = ESRCH;
    return -1;
}
