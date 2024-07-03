#include <machine/syscall.h>
#include <sys/types.h>
#include "uart8250.h"
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

//#include <reent.h>

extern unsigned int _heap;
extern unsigned int _eheap;
static caddr_t heap = NULL;

// No reentrant versions of these exist
void
_exit(int exit_status)
{
  while (1);
}

int
_kill(int pid, int sig)
{
	return -1;
}


/* Write to a file.  */
ssize_t
_write(int file, const void *ptr, size_t len)
{
  /** We can only write to STDOUT. Files are not supported. */
  if (file == STDOUT_FILENO)
  {

    /**
     * Write len amount of bytes to the UART output buffer. This
     * is a non-blocking write, which means that when writing
     * consecutuvely, the CPU has to busy-wait for the transmission
     * to finish before writing the next byte.
     */
    for(size_t i=0; i < len; i++)
    {
      uart8250_putc(((char*)ptr)[i]);
    }

    return (len);
  }
  else return -EBADF;
}

int
_close(int fildes)
{
	return -1;
}

int
_execve(const char *name, char * const *argv,
		char * const *env)
{
	return -1;
}

int
_fork()
{
	errno = ENOSYS;
	return -1;
}

int
_fstat(int fildes, struct stat *st)
{
  if (fildes < 3)
  {
    st->st_mode = S_IFCHR;
    st->st_blksize = 0;
    return 0;
  }
	else return -1;
}

int
_getpid()
{
	return -1;
}

int
_isatty(int file)
{
	if (file < 3) return 1;
  else return -1;
}

int
_link(const char *existing, const char *new)
{
	return -1;
}

_off_t
_lseek(int file, _off_t ptr, int dir)
{
	errno = ENOSYS;
	return -1;
}

int
_open(const char *file, int flags, int mode)
{
	return -1;
}

_ssize_t
_read(int file, void *ptr, size_t len)
{
  for(int i=0; i < len; i++)
  {
    ((char*)ptr)[i] = uart8250_getc();
  }
	return 0;
}

int
_readlink(const char *path, char *buf, size_t bufsize)
{
	return -1;
}

caddr_t *
_sbrk(ptrdiff_t incr)
{
  caddr_t prevHeap;
  caddr_t nextHeap;

  if (heap == NULL) { // first allocation
    heap = (caddr_t) & _heap;
  }

  prevHeap = heap;

  // Always return data aligned on a 8 byte boundary
  nextHeap = (caddr_t) (((unsigned int) (heap + incr) + 7) & ~7);
  if (nextHeap >= (caddr_t) & _eheap) {
    errno = ENOMEM;
    return ((void*)-1); // error - no more memory
  } else {
    heap = nextHeap;
    return (caddr_t) prevHeap;
  }
}

int
_stat(const char *path, struct stat *buf)
{
	return -1;
}

int
_unlink(const char * path)
{
	return (-1);
}

/* /\* Write to a file.  *\/ */
/* ssize_t */
/* _write_r(struct _reent * reent, int file, const void *ptr, size_t len) */
/* { */
/*   /\** We can only write to STDOUT. Files are not supported. *\/ */
/*   // if (file == STDOUT_FILENO) */
/*   // { */

/*     /\** */
/*      * Write len amount of bytes to the UART output buffer. This */
/*      * is a non-blocking write, which means that when writing */
/*      * consecutuvely, the CPU has to busy-wait for the transmission */
/*      * to finish before writing the next byte. */
/*      *\/ */
/*     for(size_t i=0; i < len; i++) */
/*     { */
/*       _uart8250_putc(((char*)ptr)[i]); */
/*     } */

/*     return (len); */
/*   //} */
/*   //else return syscall_errno(SYS_write, file, ptr, len, 0, 0, 0); */
/* } */

/* int */
/* _close_r(struct _reent *reent, int fildes) */
/* { */
/* 	reent->_errno = ENOSYS; */
/* 	return -1; */
/* } */

/* void */
/* _exit_r(struct _reent *reent, int exit_status) */
/* { */
/* 	reent->_errno = ENOSYS; */
/*   while (1); */
/* } */

/* int */
/* _execve_r(struct _reent *reent, const char *name, char * const *argv, */
/* 		char * const *env) */
/* { */
/* 	reent->_errno = ENOSYS; */
/* 	return -1; */
/* } */

/* int */
/* _fork_r(struct _reent *reent) */
/* { */
/* 	errno = ENOSYS; */
/* 	return -1; */
/* } */

/* int */
/* _fstat_r(struct _reent *reent, int fildes, struct stat *st) */
/* { */
/* 	reent->_errno = ENOSYS; */
/* 	return -1; */
/* } */

/* int */
/* _getpid_r(struct _reent *reent) */
/* { */
/* 	reent->_errno = ENOSYS; */
/* 	return -1; */
/* } */

/* int */
/* _isatty_r(struct _reent *reent, int file) */
/* { */
/* 	reent->_errno = ENOSYS; */
/* 	return 0; */
/* } */

/* int */
/* _kill_r(struct _reent *reent, int pid, int sig) */
/* { */
/* 	reent->_errno = ENOSYS; */
/* 	return -1; */
/* } */

/* int */
/* _link_r(struct _reent *reent, const char *existing, const char *new) */
/* { */
/* 	reent->_errno = ENOSYS; */
/* 	return -1; */
/* } */

/* _off_t */
/* _lseek_r(struct _reent *reent, int file, _off_t ptr, int dir) */
/* { */
/* 	errno = ENOSYS; */
/* 	return -1; */
/* } */

/* int */
/* _open_r(struct _reent *reent, const char *file, int flags, int mode) */
/* { */
/* 	reent->_errno = ENOSYS; */
/* 	return -1; */
/* } */

/* _ssize_t */
/* _read_r(struct _reent *reent, int file, void *ptr, size_t len) */
/* { */
/* 	reent->_errno = ENOSYS; */
/* 	return -1; */
/* } */

/* int */
/* _readlink_r(struct _reent *reent, const char *path, char *buf, size_t bufsize) */
/* { */
/* 	reent->_errno = ENOSYS; */
/* 	return -1; */
/* } */

/* int */
/* _stat_r(struct _reent *reent, const char *path, struct stat *buf) */
/* { */
/* 	reent->_errno = EIO; */
/* 	return -1; */
/* } */

/* void * */
/* _sbrk_r(struct _reent *reent, ptrdiff_t incr) */
/* { */
/*   static unsigned long heap_end; */

/*   if (heap_end == 0) */
/*     { */
/*       long brk = __internal_syscall (SYS_brk, 0, 0, 0, 0, 0, 0); */
/*       if (brk == -1) */
/*         reent->_errno = -ENOMEM; */
/*         return (void *)__syscall_error (-ENOMEM); */
/*       heap_end = brk; */
/*     } */

/*   if (__internal_syscall (SYS_brk, heap_end + incr, 0, 0, 0, 0, 0) != heap_end + incr) */
/*     reent->_errno = -ENOMEM; */
/*     return (void *)__syscall_error (-ENOMEM); */

/*   heap_end += incr; */
/*   return (void *)(heap_end - incr); */
/* } */

/* int */
/* _unlink_r(struct _reent *reent, const char * path) */
/* { */
/* 	reent->_errno = EIO; */
/* 	return (-1); */
/* } */
