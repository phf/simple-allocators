#include "arena.h"
#include "cached.h"
#include "pool.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define UNUSED __attribute__((unused))

#define ALLOC_SIZE (4096)
#define ARENA_SIZE (1024 * ALLOC_SIZE)
#define POOL_SIZE (1024)
#define MEASURE_SECONDS (1)

static volatile sig_atomic_t running;

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
	alarm(MEASURE_SECONDS);
}

static size_t
bench_malloc(size_t allocs, size_t objsize)
{
	void *objs[allocs];
	size_t count = 0;

	while (running) {
		for (size_t i = 0; i < allocs; i++) {
			objs[i] = calloc(1, objsize);
			assert(objs[i]);
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
			assert(objs[i]);
			count++;
		}
		for (size_t i = 0; i < allocs; i++) {
			ca_free(objs[i]);
		}
	}

	return count;
}

static size_t
bench_pl_alloc(size_t allocs)
{
	void *objs[allocs];
	size_t count = 0;

	while (running) {
		for (size_t i = 0; i < allocs; i++) {
			objs[i] = pl_alloc();
			assert(objs[i]);
			count++;
		}
		for (size_t i = 0; i < allocs; i++) {
			pl_free(objs[i]);
		}
	}

	return count;
}

static size_t
bench_ar_alloc(size_t allocs, size_t size)
{
	void *objs[allocs];
	size_t count = 0;

	while (running) {
		for (size_t i = 0; i < allocs; i++) {
			objs[i] = ar_alloc(size);
			assert(objs[i]);
			count++;
		}
		ar_free(); /* feels like cheating but it's not really */
	}

	return count;
}

static void
bench_four(void)
{
	if (ca_setup(ALLOC_SIZE, 4096) < 0) {
		panic("failed to setup cached allocator");
	}

	set_alarm();

	size_t allocs = bench_ca_alloc(1024);

	printf("%-12s\t%9zu allocs/second\n", "huge_cache",
			allocs / MEASURE_SECONDS);

	ca_cleanup();
}

static void
bench_one(void)
{
	if (ca_setup(ALLOC_SIZE, 1024) < 0) {
		panic("failed to setup cached allocator");
	}

	set_alarm();

	size_t allocs = bench_ca_alloc(1024);

	printf("%-12s\t%9zu allocs/second\n", "large_cache",
			allocs / MEASURE_SECONDS);

	ca_cleanup();
}

static void
bench_two(void)
{
	if (ca_setup(ALLOC_SIZE, 16) < 0) {
		panic("failed to setup cached allocator");
	}

	set_alarm();

	size_t allocs = bench_ca_alloc(1024);

	printf("%-12s\t%9zu allocs/second\n", "small_cache",
			allocs / MEASURE_SECONDS);

	ca_cleanup();
}

static void
bench_three(void)
{
	set_alarm();

	size_t allocs = bench_malloc(1024, ALLOC_SIZE);

	printf("%-12s\t%9zu allocs/second\n", "just_malloc",
			allocs / MEASURE_SECONDS);
}

static void
bench_five(void)
{
	if (ar_setup(ARENA_SIZE) < 0) {
		panic("failed to setup arena allocator");
	}

	set_alarm();

	size_t allocs = bench_ar_alloc(1024, ALLOC_SIZE);

	printf("%-12s\t%9zu allocs/second\n", "ar_alloc",
			allocs / MEASURE_SECONDS);

	ar_cleanup();
}

static void
bench_six(void)
{
	if (pl_setup(POOL_SIZE, ALLOC_SIZE) < 0) {
		panic("failed to setup pool allocator");
	}

	set_alarm();

	size_t allocs = bench_pl_alloc(1024);

	printf("%-12s\t%9zu allocs/second\n", "pl_alloc",
			allocs / MEASURE_SECONDS);

	pl_cleanup();
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
	puts("");
	bench_five();
	puts("");
	bench_six();

	exit(EXIT_SUCCESS);
}
