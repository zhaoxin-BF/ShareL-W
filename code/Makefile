all: main upload
main:main.cpp threadpool.hpp httpserver.hpp utils.hpp
	g++ -g -std=c++0x $^ -o $@ -pthread
upload:upload.cpp utils.hpp
	g++ -g -std=c++0x $^ -o $@ -pthread
