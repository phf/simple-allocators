#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "arena.h"

union align {
	long long int l;
	long double d;
	void *p;
};

/* alignment for a long-ish builtin type? */
#define ALIGN (sizeof(union align))
/* ensure it's a power of two */
static_assert(ALIGN && !(ALIGN & (ALIGN - 1)), "ALIGN not power of two");

static unsigned char *data;
static size_t total;
static size_t used;

/*
 * round up to nearest multiple of ALIGN
 */
static size_t
round_up(size_t minimum)
{
	return (minimum + ALIGN - 1) & ~(ALIGN - 1);
}

int
ar_setup(size_t total_size)
{
	assert(data == NULL);

	size_t adjusted = round_up(total_size);

	void *p = calloc(1, adjusted);
	if (p == NULL) {
		return -1;
	}

	data = p;
	total = adjusted;

	return 0;
}

void *
ar_alloc(size_t object_size)
{
	assert(data != NULL);

	size_t adjusted = round_up(object_size);
	if (used + adjusted > total) {
		return NULL;
	}

	void *p = data + used;
	used += adjusted;

	return p;
}

void
ar_free(void)
{
	assert(data != NULL);

	memset(data, 0, total);
	used = 0;
}

void
ar_cleanup(void)
{
	assert(data != NULL);
	free(data);
	data = NULL;
	total = 0;
	used = 0;
}
