#pragma once

#ifndef ARENA_H_
#define ARENA_H_

int
ar_setup(size_t total_size);

void *
ar_alloc(size_t object_size);

void
ar_free(void);

void
ar_cleanup(void);

#endif
