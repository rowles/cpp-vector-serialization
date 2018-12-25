
FN="vec.out"

all: clean run

clean:
	rm -f test

build:
	clang++ -std=c++17 -O3 main.cpp -o test

run: build
	./test
	wc -c < ${FN}

