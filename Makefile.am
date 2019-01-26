ACLOCAL_AMFLAGS=-I m4

CWD=$(shell /bin/pwd)
AM_CPPFLAGS=-I$(CWD)/src
if ENABLE_COMMON_ARCH
AM_CPPFLAGS+= -DA6_COMMON_MCNTX
endif

CC=gcc -std=gnu11
LIBS=-lpthread

lib_LTLIBRARIES=liba6.la
liba6_la_SOURCES= \
				  src/common/rbt.c \
				  src/common/tid.c \
				  src/core/iomonitor.c \
				  src/core/mthread.c \
				  src/core/scheduler.c \
				  src/core/swarm.c \
				  src/core/timer_queue.c \
				  src/core/uthread.c \
				  src/core/uthread_lifespan.c
if !ENABLE_COMMON_ARCH
liba6_la_SOURCES+= src/core/mcontext_x64.S
endif

a6includedir=$(includedir)/a6
a6include_HEADERS= \
					src/core/swarm.h
