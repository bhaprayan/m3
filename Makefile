CXX=g++ -std=c++11
CFLAGS-GHC=-I. /usr/lib/gcc/x86_64-redhat-linux/4.8.2/include/ -pg
CFLAGS=-lraylib -I./earcut.hpp/include/mapbox
OMP=-fopenmp -DOMP
#ASAN="-fsanitize=address -fno-omit-frame-pointer -fsanitize-address-use-after-scope -fsanitize=undefined"
#ASAN_LDFLAGS="-fsanitize=address "

return_voronoi :
	mkdir -p build
	clang -c voronoi/src/stb_wrapper.c -o build/stb_wrapper.o
	clang -c voronoi/src/return_voronoi.c -g -O3 -m64 -std=c99 -Wall -Weverything -Wno-float-equal -pedantic -lm -Isrc build/stb_wrapper.o -o build/return_voronoi.o

mapgen :
	$(CXX) -c voronoi/src/stb_wrapper.c -o build/stb_wrapper.o
	$(CXX) $(CFLAGS) $(OMP) -o mapgen -g -O3 -m64 -std=c++11 -w -Wno-float-equal -pedantic -lm -Isrc src/main.cc build/stb_wrapper.o

clean :
	rm build/*
	rm mapgen
