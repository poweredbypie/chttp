chttp: src/main.c src/router.c src/parse.c src/shared.c
	clang $^ -o $@ -lws2_32 -g -fsanitize=address

test: src/test.c src/router.c
	clang $^ -o $@ -g -Wincompatible-pointer-types-discards-qualifiers -fsanitize=address

.PHONY: run
run: chttp
	./chttp