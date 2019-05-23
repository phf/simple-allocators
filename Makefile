CFLAGS=-std=gnu11 -Wall -Wextra -Wpedantic -Og -g -D_DEFAULT_SOURCE \
       -fsanitize=undefined -fsanitize=address
LDFLAGS=-fsanitize=undefined -fsanitize=address

ALL=bench

all: $(ALL)

bench: bench.o arena.o cached.o pool.o

clean:
	$(RM) $(ALL) *.o

.PHONY:	all check clean format

check:
	cppcheck --enable=all --std=c11 *.[ch]
format:
	clang-format -i *.[ch]
