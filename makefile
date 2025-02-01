CC = g++

all:
	$(CC) src/http.hpp src/HttpServerThread.cpp src/LoadBalancer.cpp src/sockets_test.cpp src/WebServer.cpp src/http.cpp -o bin/test -pthread -lpthread