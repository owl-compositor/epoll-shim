#ifndef SHIM_SYS_SIGNALFD_H
#define SHIM_SYS_SIGNALFD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h> /* IWYU pragma: keep */

#include <fcntl.h>
#include <signal.h>
#include <stdint.h>

#include <stddef.h>

#if 0
#define __NEED_sigset_t

#include <bits/alltypes.h>
#endif

#define SFD_CLOEXEC O_CLOEXEC
#define SFD_NONBLOCK O_NONBLOCK

int signalfd(int /*fd*/, const sigset_t * /*sigs*/, int /*flags*/);

struct signalfd_siginfo {
	uint32_t  ssi_signo;
	int32_t   ssi_errno;
	int32_t   ssi_code;
	uint32_t  ssi_pid;
	uint32_t  ssi_uid;
	int32_t   ssi_fd;
	uint32_t  ssi_tid;
	uint32_t  ssi_band;
	uint32_t  ssi_overrun;
	uint32_t  ssi_trapno;
	int32_t   ssi_status;
	int32_t   ssi_int;
	uint64_t  ssi_ptr;
	uint64_t  ssi_utime;
	uint64_t  ssi_stime;
	uint64_t  ssi_addr;
	uint16_t  ssi_addr_lsb;
	uint8_t   pad[128-12*4-4*8-2];
};


#ifndef SHIM_SYS_SHIM_HELPERS
#define SHIM_SYS_SHIM_HELPERS
#include <unistd.h> /* IWYU pragma: keep */

extern int epoll_shim_close(int);
#define close epoll_shim_close
#endif

#ifndef SHIM_SYS_SHIM_HELPERS_READ
#define SHIM_SYS_SHIM_HELPERS_READ
#include <unistd.h> /* IWYU pragma: keep */

extern ssize_t epoll_shim_read(int, void *, size_t);
#define read epoll_shim_read
#endif

#ifndef SHIM_SYS_SHIM_HELPERS_POLL
#define SHIM_SYS_SHIM_HELPERS_POLL
#include <poll.h> /* IWYU pragma: keep */

extern int epoll_shim_poll(struct pollfd *, nfds_t, int);
extern int epoll_shim_ppoll(struct pollfd *, nfds_t,
    struct timespec const *, sigset_t const *);
#define poll epoll_shim_poll
#define ppoll epoll_shim_ppoll
#endif


#ifdef __cplusplus
}
#endif

#endif
