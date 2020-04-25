CXX=clang++ -std=c++11
CFLAGS=-lraylib -I./earcut.hpp/include/mapbox
#ASAN="-fsanitize=address -fno-omit-frame-pointer -fsanitize-address-use-after-scope -fsanitize=undefined"
#ASAN_LDFLAGS="-fsanitize=address "

return_voronoi :
	mkdir -p build
	clang -c voronoi/src/stb_wrapper.c -o build/stb_wrapper.o
	clang -c voronoi/src/return_voronoi.c -g -O0 -m64 -std=c99 -Wall -Weverything -Wno-float-equal -pedantic -lm -Isrc build/stb_wrapper.o -o build/return_voronoi.o

mapgen :
	$(CXX) -c voronoi/src/stb_wrapper.c -o build/stb_wrapper.o
	$(CXX) $(CFLAGS) -o mapgen -g -O0 -m64 -std=c++11 -w -Weverything -Wno-float-equal -pedantic -lm -Isrc src/main.cc build/stb_wrapper.o

clean :
	rm -f build/*
	rm -f mapgen
