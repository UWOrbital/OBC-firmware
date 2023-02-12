/* Support files for GNU libc.  Files in the system namespace go here.
   Files in the C namespace (ie those that do not start with an
   underscore) go in .c.  */

#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#include <reent.h>
#include <unistd.h>
#include <sys/wait.h>

caddr_t _sbrk(int incr)
{
	return 0;
}

/*
 * _gettimeofday primitive (Stub function)
 * */
int _gettimeofday (struct timeval * tp, struct timezone * tzp)
{
  return 0;
}
void initialise_monitor_handles()
{
}

int _getpid(void)
{
	return 1;
}

int _kill(int pid, int sig)
{
	return -1;
}

void _exit (int status)
{
	while (1) {}
}

int _write(int file, char *ptr, int len)
{
    return 0;
}

int _close(int file)
{
	return -1;
}

int _fstat(int file, struct stat *st)
{
	return 0;
}

int _isatty(int file)
{
	return 1;
}

int _lseek(int file, int ptr, int dir)
{
	return 0;
}

int _read(int file, char *ptr, int len)
{
    return 0;
}

int _open(char *path, int flags, ...)
{
	return -1;
}

int _wait(int *status)
{
	return -1;
}

int _unlink(char *name)
{
	return -1;
}

int _times(struct tms *buf)
{
	return -1;
}

int _stat(char *file, struct stat *st)
{
	return 0;
}

int _link(char *old, char *new)
{
	return -1;
}

int _fork(void)
{
	return -1;
}

int _execve(char *name, char **argv, char **env)
{
	return -1;
}