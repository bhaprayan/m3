CXX=g++ -std=c++11
CFLAGS=-I.
#ASAN="-fsanitize=address -fno-omit-frame-pointer -fsanitize-address-use-after-scope -fsanitize=undefined"
#ASAN_LDFLAGS="-fsanitize=address "

return_voronoi :
	mkdir -p build
	clang -c voronoi/src/stb_wrapper.c -o build/stb_wrapper.o
	clang -o build/main -g -O0 -m64 -std=c99 -Wall -Weverything -Wno-float-equal -pedantic -lm -Isrc build/stb_wrapper.o voronoi/src/main.c
	clang -o build/return_voronoi.o -g -O0 -m64 -std=c99 -Wall -Weverything -Wno-float-equal -pedantic -lm -Isrc build/stb_wrapper.o voronoi/src/return_voronoi.c

mapgen : src/main.cc return_voronoi
	$(CXX) -o mapgen src/main.cc

clean :
	rm mapgen build/*
