#pragma once

#ifndef CACHED_H_
#define CACHED_H_

int
ca_setup(size_t object_size, size_t cache_size);

void *
ca_alloc(void);

void
ca_free(void *object);

void
ca_cleanup(void);

#endif
