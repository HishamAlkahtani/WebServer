#include <iostream>
#include <string>
#include <fstream>
#include <ostream>
#include <unistd.h>
#include <stdlib.h>
#include <filesystem>
#include <memory>
#include <spdlog/spdlog.h>
#include "http.hpp"
#include "WebServer.cpp"

int main()
{
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%-16n] [%^%-5l%$] %v");
    spdlog::set_level(spdlog::level::trace);
    WebServer server(8080, 2);
    server.start();
}

/*int main() {
    initWorkingDirectory();
    std::cout << "Server started from with root directory: " << workingDirectory << std::endl;
    ServerSocket socket(8080);
    InternetHttpSocket peer = socket.getConnection();
    for (int i = 0; i <= 10; i++) {
        HttpRequest request = peer.recieve();

        std::cout << "\nMessage Recieved!\n";
        std::cout << " user wants path: " << request.getRequestedPath();

        peer.snd(createResponse(request));
    }

    std::cout << "Program terminating!";
    return 0;
}*/

/*
    A browser has two ways of knowing that the response from the server is complete,
    either the server closes the tcp connection after completion, or the server specifies a
    Content-Length header.
*/

/*  OK... another problem: SIGPIPE! how does your program know if the client closed the socket?
    if client closes socket and you try to write to it: SIGPIPE, are you willing to let that go by?
    because every session will be it's thread? so killing  a thread is fine? but this depends on thread pool implementation!
    maybe I'll have this be a HHTP/1.0 server?
*/

/* big problem! program only sends buffers when closed! Do i just have to close the socket
every time! this is ridiculous! for now a hacky fix is to close every socket after the response is send
(InternetHttpSocket destructor closes the socket, thus sending everything in its buffer).. but that would have
horrible performance! As launching a new thread for just to make a single response will be annoying...
or maybe the threads get passed sockets that they send one response over and then close them? but this means every
request has to requeue! This is confusing.
two options: 1- every connection responds to one request only then close the socket.. but this is the
                old way of doing things! HTTP/0.9! I want this to be HTTP/1.1
             2- somehow find a way to control (or force) when the data gets sent from the
                os buffer of the socket... IDK how to do that. maybe TCP_NODELAY? or maybe
                turnign TCP_NODELAY on and off to enforce a flush?

*/
