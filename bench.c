#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "cached.h"

#define UNUSED __attribute__((unused))

static sig_atomic_t running;

static void
panic(const char *msg)
{
	fprintf(stderr, "panic: %s\n", msg);
	exit(EXIT_FAILURE);
}

static void
alarm_handler(int sig UNUSED, siginfo_t *info UNUSED, void *arg UNUSED)
{
	running = 0;
}

static void
trap_signals(void)
{
	struct sigaction sa;

	sa.sa_sigaction = alarm_handler;
	sa.sa_flags = SA_SIGINFO;

	if (sigaction(SIGALRM, &sa, NULL) < 0) {
		panic("failed to setup signal handler");
	}
}

static void
set_alarm(void)
{
	running = 1;
	alarm(1);
}

static size_t
bench_malloc(size_t allocs, size_t objsize)
{
	void *objs[allocs];
	size_t count = 0;

	while (running) {
		for (size_t i = 0; i < allocs; i++) {
			objs[i] = calloc(1, objsize);
			count++;
		}
		for (size_t i = 0; i < allocs; i++) {
			free(objs[i]);
		}
	}

	return count;
}

static size_t
bench_ca_alloc(size_t allocs)
{
	void *objs[allocs];
	size_t count = 0;

	while (running) {
		for (size_t i = 0; i < allocs; i++) {
			objs[i] = ca_alloc();
			count++;
		}
		for (size_t i = 0; i < allocs; i++) {
			ca_free(objs[i]);
		}
	}

	return count;
}

static void
bench_four(void)
{
	if (ca_setup(256, 4096) < 0) {
		panic("failed to setup cached allocator");
	}

	set_alarm();

	size_t allocs = bench_ca_alloc(1024);

	printf("%-12s\t%9zu allocs/second\n", "huge_cache", allocs);

	ca_cleanup();
}

static void
bench_one(void)
{
	if (ca_setup(256, 1024) < 0) {
		panic("failed to setup cached allocator");
	}

	set_alarm();

	size_t allocs = bench_ca_alloc(1024);

	printf("%-12s\t%9zu allocs/second\n", "large_cache", allocs);

	ca_cleanup();
}

static void
bench_two(void)
{
	if (ca_setup(256, 16) < 0) {
		panic("failed to setup cached allocator");
	}

	set_alarm();

	size_t allocs = bench_ca_alloc(1024);

	printf("%-12s\t%9zu allocs/second\n", "small_cache", allocs);

	ca_cleanup();
}

static void
bench_three(void)
{
	set_alarm();

	size_t allocs = bench_malloc(1024, 256);

	printf("%-12s\t%9zu allocs/second\n", "just_malloc", allocs);
}

int
main(void)
{
	trap_signals();

	bench_three();
	puts("");
	bench_two();
	bench_one();
	bench_four();

	exit(EXIT_SUCCESS);
}
