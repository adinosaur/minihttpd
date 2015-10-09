# dinosaur simple im system makefile.

CC = clang++-3.6

all:
	$(CC) --std=c++11 -Wall -g ./src/main.cc -o ./build/httpd.out

tst:
	$(CC) --std=c++11 -Wall -g ./test/test_http_request.cc -o ./build/test1.out
	$(CC) --std=c++11 -Wall -g ./test/test_http_response.cc -o ./build/test2.out

clean:
	rm ./build/*

