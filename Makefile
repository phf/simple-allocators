CFLAGS+=-std=c11 -Wall -Wextra -Wpedantic
CFLAGS+=-D_DEFAULT_SOURCE

ALL=bench

dev: CFLAGS+=-Og -g3
dev: CFLAGS+=-fsanitize=undefined -fsanitize=address
dev: LDFLAGS+=-fsanitize=undefined -fsanitize=address
dev: $(ALL)

prod: CFLAGS+=-O2 -s
prod: LDFLAGS+=-s
prod: $(ALL)

musl: CC=musl-gcc
musl: CFLAGS+=-Os -s
musl: LDFLAGS+=-static -s
musl: $(ALL)

bench: bench.o arena.o cached.o pool.o

bench.o: bench.c arena.h cached.h pool.h

arena.o: arena.c arena.h
cached.o: cached.c cached.h
pool.o: pool.c pool.h

clean:
	$(RM) $(ALL) *.o
check:
	cppcheck --enable=all --std=c11 *.[ch]
format:
	clang-format -i *.[ch]

.PHONY:	dev prod musl  check clean format
