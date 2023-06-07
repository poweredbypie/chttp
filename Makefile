CFLAGS := -g -fsanitize=address
CC := clang

build/chttp: src/main.c src/router.c src/parse.c src/slice.c
	mkdir -p build
	$(CC) $^ -o $@ -lws2_32 $(CFLAGS)

build/test: src/test.c src/router.c
	mkdir -p build
	$(CC) $^ -o $@ $(CFLAGS)

.PHONY: run
run: build/chttp
	./$<