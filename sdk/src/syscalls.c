#include <sys/stat.h>
#include <sys/types.h>
#include <stddef.h>
#include <errno.h>

ssize_t _write(int file, const void *ptr, size_t len) {

}

ssize_t _read(int file, void *ptr, size_t len) {

}

void *_sbrk(ptrdiff_t increment) {

}

int _fstat(int file, struct stat *status) {

}

int _isatty(int file) {

}

off_t _lseek(int file, off_t offset, int whence) {

}

int _close(int file) {

}

void _exit(int status) {

}

pid_t _getpid(void) {

}

int _kill(pid_t pid, int signal_number) {

}
