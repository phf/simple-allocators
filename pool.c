#include "pool.h"

#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static void *pool; /* the actual pool of memory */
static size_t blocks; /* number of blocks */
static size_t block_size; /* size of one block */

static uint64_t *used; /* bitmap to track allocated blocks */
static const size_t BITS = sizeof(*used) * CHAR_BIT; /* bits per slot */

/*
 * Allocate a bitmap with at least `size` bits.
 * Returns NULL on failure.
 */
static uint64_t *
bitmap(size_t size)
{
	assert(size > 0);

	const size_t slots = (size / BITS) + 1;

	uint64_t *p = calloc(slots, sizeof(*p));
	return p; /* this is fine */
}

/* Mark given `block` as free. */
static inline void
bitmap_free(size_t block)
{
	assert(block < blocks);

	const size_t slot = (block / BITS);
	const size_t bit = (block % BITS);

	used[slot] &= ~(1UL << bit);
}

/* Mark given `block` as used. */
static inline void
bitmap_used(size_t block)
{
	assert(block < blocks);

	const size_t slot = (block / BITS);
	const size_t bit = (block % BITS);

	used[slot] |= (1UL << bit);
}

/*
 * Find right-most zero bit. Returns 0 if there is none. Examples:
 *
 * 00000000	01010011	11111111	bits
 *
 * 11111111	10101100	00000000	~bits
 * 00000001	01010100	00000000	bits+1
 * --------	--------	--------
 * 00000001	00000100	00000000	~bits & (bits+1)
 */
static inline uint64_t
find_zero(const uint64_t bits)
{
	return ~bits & (bits + 1);
}

static inline uint64_t
ulog2(const uint64_t x)
{
	return ((BITS - 1) - __builtin_clzl(x)); /* count leading zeros */
}

/* Find a free block. Returns -1 if there is none. */
static int
bitmap_alloc(void)
{
	for (size_t i = 0; i < blocks / BITS; i++) {
		uint64_t free = find_zero(used[i]);
		if (free != 0) {
			return i * BITS + ulog2(free);
		}
	}
	return -1;
}

int
pl_setup(size_t pool_size, size_t object_size)
{
	assert(object_size > 0 && pool_size > 0);

	void *p = calloc(pool_size, object_size);
	if (!p) {
		return -1;
	}

	uint64_t *q = bitmap(pool_size);
	if (!q) {
		free(p);
		return -2;
	}

	pool = p;
	used = q;

	block_size = object_size;
	blocks = pool_size;

	return 0;
}

void *
pl_alloc(void)
{
	assert(pool);

	int block = bitmap_alloc();
	if (block < 0) {
		return NULL;
	}

	bitmap_used(block);

	const uintptr_t p = (uintptr_t)pool;
	const uintptr_t o = p + block * block_size;

	return (void *)o;
}

void
pl_free(void *object)
{
	assert(pool);

	if (!object) {
		/* free(3) also does nothing for NULL */
		return;
	}

	/* memset here to catch dangling references */
	memset(object, 0, block_size);

	const uintptr_t o = (uintptr_t)object;
	const uintptr_t p = (uintptr_t)pool;
	const uintptr_t offset = o - p;
	const size_t block = offset / block_size;

	bitmap_free(block);
}

void
pl_cleanup(void)
{
	assert(pool);

	free(used);
	free(pool);
	pool = NULL;
}
