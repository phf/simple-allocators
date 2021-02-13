#include "arena.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*
 * Alignment for largest scalar type. Before max_align_t was added in C11, code
 * like this usually declared a union of several long-ish types, resulting in a
 * suitable sizeof result. Hopefully anyway. This is cleaner.
 */
#define ALIGN (sizeof(max_align_t))

/*
 * We need this property below to round up to multiples of `ALIGN`. (Of course
 * it's almost certainly true anyway.)
 */
static_assert(ALIGN && !(ALIGN & (ALIGN - 1)), "ALIGN not a power of two");

static unsigned char *arena; /* memory allocated for the arena */
static size_t capacity; /* capacity of the arena in bytes */
static size_t used; /* bytes used so far (from beginning) */

/*
 * Round up `minimum` to nearest multiple of ALIGN.
 */
static inline size_t
round_up(size_t minimum)
{
	return (minimum + ALIGN - 1) & ~(ALIGN - 1);
}

int
ar_setup(size_t arena_size)
{
	assert(!arena);

	size_t adjusted = round_up(arena_size);

	void *p = calloc(1, adjusted);
	if (p == NULL) {
		return -1;
	}

	arena = p;
	capacity = adjusted;

	return 0;
}

void *
ar_alloc(size_t object_size)
{
	assert(arena);

	size_t adjusted = round_up(object_size); /* keep `used` aligned */
	if (used + adjusted > capacity) {
		return NULL; /* out of space */
	}

	void *p = arena + used;
	used += adjusted;

	return p;
}

void
ar_free(void)
{
	assert(arena);

	memset(arena, 0, capacity);
	used = 0;
}

void
ar_cleanup(void)
{
	assert(arena);

	free(arena);
	arena = NULL;

	capacity = 0;
	used = 0;
}
