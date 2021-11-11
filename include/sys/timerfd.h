#ifndef SHIM_SYS_TIMERFD_H
#define SHIM_SYS_TIMERFD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fcntl.h>
#include <time.h>

#define TFD_NONBLOCK O_NONBLOCK
#define TFD_CLOEXEC O_CLOEXEC

#define TFD_TIMER_ABSTIME 1
#define TFD_TIMER_CANCEL_ON_SET (1 << 1)

struct itimerspec {
	struct timespec it_interval;
	struct timespec it_value;
};

int timerfd_create(int, int);
int timerfd_settime(int, int, struct itimerspec const *, struct itimerspec *);
int timerfd_gettime(int, struct itimerspec *);


#include <epoll-shim/detail/common.h>
#include <epoll-shim/detail/poll.h>
#include <epoll-shim/detail/read.h>


#ifdef __cplusplus
}
#endif

#endif
