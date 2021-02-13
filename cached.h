#pragma once

#ifndef CACHED_H_
#define CACHED_H_

#include <stddef.h>

/**
 * Create cache of size `slots` for allocating chunks of `object_size` bytes.
 * Return 0 on success, <0 on failure.
 */
int
ca_setup(size_t object_size, size_t slots);

/**
 * Allocate a chunk of memory.
 * Return NULL on failure.
 */
void *
ca_alloc(void);

/**
 * Free a chunk of memory obtained from `ca_alloc`.
 */
void
ca_free(void *object);

/**
 * Destroy the cache, freeing all memory still allocated.
 */
void
ca_cleanup(void);

#endif
