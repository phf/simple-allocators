#pragma once

#ifndef POOL_H_
#define POOL_H_

int
pl_setup(size_t object_size, size_t pool_size);

void *
pl_alloc(void);

void
pl_free(void *object);

void
pl_cleanup(void);

#endif
