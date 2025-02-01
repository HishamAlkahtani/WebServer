CC = g++

all:
	$(CC) src/http.hpp src/HttpServerThread.cpp src/LoadBalancer.cpp src/sockets_test.cpp src/WebServer.cpp src/HttpRequestHandler.cpp src/HttpRequestHandler.hpp src/http.cpp -o bin/test -pthread -lpthread