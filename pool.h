#pragma once

#ifndef POOL_H_
#define POOL_H_

#include <stddef.h>

/**
 * Create pool with `slots` chunks of `object_size` bytes.
 * Return 0 on success, <0 on failure.
 */
int
pl_setup(size_t slots, size_t object_size);

/**
 * Allocate one chunk from the pool.
 * Return NULL on failure.
 */
void *
pl_alloc(void);

/**
 * Free a chunk of memory obtained from `pl_alloc`.
 */
void
pl_free(void *object);

/**
 * Destroy the pool, freeing all memory allocated in `pl_setup`.
 * Invalidates all pointers you may still be holding.
 */
void
pl_cleanup(void);

#endif
