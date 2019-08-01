raytracer: main.cpp
	rm -f bin/raytracer
	clang++ -Wall -Wextra -Wpedantic -O2 main.cpp -g -std=c++11 -o raytracer
	mkdir -p bin
	mv -f raytracer bin

run: raytracer
	bin/raytracer

debug: raytracer
	lldb bin/raytracer