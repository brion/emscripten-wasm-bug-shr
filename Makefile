all : test.html test.wat

test.html : test.c Makefile
	emcc -o test.html -O3 -g4 -s SIMD=1 test.c

test.wat : test.html
	wasm-dis test.wasm > test.wat

clean :
	rm -f test.html test.js test.wasm test.wat test.wasm.map
