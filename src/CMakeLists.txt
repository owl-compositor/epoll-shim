if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  add_library(epoll-shim INTERFACE)
  add_library(epoll-shim::epoll-shim ALIAS epoll-shim)
  return()
endif()

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(tree-macros REQUIRED)
find_package(queue-macros REQUIRED)

macro(add_compat_target _name _condition)
  add_library(compat_${_name} STATIC compat_${_name}.c)
  set_property(TARGET compat_${_name} PROPERTY POSITION_INDEPENDENT_CODE ON)
  target_compile_options(
    compat_${_name}
    INTERFACE "SHELL:-include \"${CMAKE_CURRENT_LIST_DIR}/compat_${_name}.h\"")
  add_library(compat_enable_${_name} INTERFACE)
  if(${_condition})
    target_link_libraries(compat_enable_${_name} INTERFACE compat_${_name})
    string(TOUPPER "${_name}" _upper_name)
    target_compile_definitions(compat_enable_${_name}
                               INTERFACE COMPAT_ENABLE_${_upper_name})
  endif()
endmacro()

include(CheckSymbolExists)
# FreeBSD 13 supports native eventfd descriptors. Prefer them if available.
check_symbol_exists(eventfd "sys/eventfd.h" HAVE_EVENTFD)
check_symbol_exists(kqueue1 "sys/event.h" HAVE_KQUEUE1)
add_compat_target(kqueue1 "NOT;HAVE_KQUEUE1")
check_symbol_exists(sigandset "signal.h" HAVE_SIGANDSET)
check_symbol_exists(sigorset "signal.h" HAVE_SIGORSET)
check_symbol_exists(sigisemptyset "signal.h" HAVE_SIGISEMPTYSET)
add_compat_target(
  sigops "NOT;HAVE_SIGANDSET;AND;NOT;HAVE_SIGORSET;AND;NOT;HAVE_SIGISEMPTYSET")
include(CheckCSourceRuns)
check_c_source_runs(
  [=[
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
int main() {
  struct kevent kev;
  EV_SET(&kev, 0, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, 0, 0);
  int kq;
  return ((kq = kqueue()) >= 0 && kevent(kq, &kev, 1, NULL, 0, NULL) == 0) ? 0 : 1;
}
]=]
  ALLOWS_ONESHOT_TIMERS_WITH_TIMEOUT_ZERO)
add_library(evfilt_timer_quirks INTERFACE)
if(NOT ALLOWS_ONESHOT_TIMERS_WITH_TIMEOUT_ZERO)
  target_compile_definitions(
    evfilt_timer_quirks
    INTERFACE QUIRK_EVFILT_TIMER_DISALLOWS_ONESHOT_TIMEOUT_ZERO)
endif()

add_library(
  epoll-shim
  epoll_shim_ctx.c
  epoll.c
  epollfd_ctx.c
  kqueue_event.c
  timerfd.c
  timerfd_ctx.c
  signalfd.c
  signalfd_ctx.c
  timespec_util.c)
if(NOT HAVE_EVENTFD)
  target_sources(epoll-shim PRIVATE eventfd.c eventfd_ctx.c)
endif()
include(GenerateExportHeader)
generate_export_header(epoll-shim BASE_NAME epoll_shim)
target_link_libraries(
  epoll-shim
  PRIVATE Threads::Threads #
          queue-macros::queue-macros tree-macros::tree-macros #
          evfilt_timer_quirks compat_enable_kqueue1 compat_enable_sigops)
target_include_directories(
  epoll-shim
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  PUBLIC $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/install-include>)

set(_headers
    "epoll-shim/detail/common.h"
    "epoll-shim/detail/poll.h"
    "epoll-shim/detail/read.h"
    "epoll-shim/detail/write.h"
    "sys/epoll.h"
    "sys/signalfd.h"
    "sys/timerfd.h")
if(NOT HAVE_EVENTFD)
  list(APPEND _headers "sys/eventfd.h")
endif()
foreach(_header IN LISTS _headers)
  configure_file("${PROJECT_SOURCE_DIR}/include/${_header}"
                 "${PROJECT_BINARY_DIR}/install-include/${_header}" COPYONLY)
endforeach()

set_target_properties(epoll-shim PROPERTIES SOVERSION 0)
