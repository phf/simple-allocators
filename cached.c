#include "cached.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static size_t object_size;
static size_t max_cached;
static size_t num_cached;
static void **cache;

int
ca_setup(size_t osize, size_t csize)
{
	/* TODO allow osize == 0 following malloc? */
	if (osize == 0 || csize == 0) {
		return -1;
	}

	cache = calloc(csize, sizeof(*cache));
	if (!cache) {
		return -2;
	}

	object_size = osize;
	max_cached = csize;
	num_cached = 0;

	return 0;
}

void *
ca_alloc(void)
{
	assert(cache);

	if (num_cached > 0) {
		void *object = cache[--num_cached];
		return object;
	}

	return calloc(1, object_size);
}

void
ca_free(void *object)
{
	assert(cache);

	if (!object) {
		/* free(3) also does nothing for NULL */
		return;
	}

	if (num_cached < max_cached) {
		cache[num_cached++] = object;
		/* memset here to catch dangling references */
		memset(object, 0, object_size);
		return;
	}

	free(object);
}

void
ca_cleanup(void)
{
	for (size_t i = 0; i < num_cached; i++) {
		free(cache[i]);
	}
	free(cache);
	cache = NULL;
}
