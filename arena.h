#pragma once

#ifndef ARENA_H_
#define ARENA_H_

#include <stddef.h>

/**
 * Create arena holding up to `arena_size` bytes.
 * Return 0 on success, <0 on failure.
 */
int
ar_setup(size_t arena_size);

/**
 * Allocate `object_size` bytes from the arena.
 * Return NULL on failure.
 */
void *
ar_alloc(size_t object_size);

/**
 * Free all allocations made in the arena.
 * Invalidates all pointers you may still be holding.
 * The arena still exists, ready for `ar_alloc` calls.
 */
void
ar_free(void);

/**
 * Destroy the arena, freeing all memory allocated in `ar_setup`.
 * Invalidates all pointers you may still be holding.
 */
void
ar_cleanup(void);

#endif
