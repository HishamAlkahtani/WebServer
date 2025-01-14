#include <iostream>
#include <string>
#include <fstream>
#include <ostream>
#include <unistd.h>
#include <stdlib.h>
#include <filesystem>
#include <memory>
#include "http.hpp"
#include "WebServer.cpp"

int main()
{
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

/* call socket(), specify domain and type and protocol. AF_INET = internet domain.
    SOCK_STREAM is the type, and 0 is the default protocol (tcp). (see man 7 ip)

   socket() creates a socket in address space (look man socket), but no name is
   given to it for other processes to connect to it. we then use bind, to fully
   initiate the socket, and specify the port and all the necessary info through
   a the appropriate struct sockaddr... which is in our case struct sockaddr_in
   where we give it the socket family (always AF_INET for some reason) and port
   (in big endian, thus htons(80))

    we also have to set sin_addr which is of type struct in_addr... and wtf is that?


*/

/*

from "man socket"
Sockets of type SOCK_STREAM are full-duplex byte streams.  They do  not
preserve  record  boundaries.   A  stream socket must be in a connected
state before any data may be sent or received on it.  A  connection  to
another socket is created with a connect(2) call.  Once connected, data
may be transferred using read(2) and write(2) calls or some variant  of
the  send(2)  and  recv(2)  calls.  When a session has been completed a
close(2) may be performed.  Out-of-band data may also be transmitted as
described in send(2) and received as described in recv(2).

    look up "file descriptor table", that might be interseting... what is it exactly?

A file descriptor is an index in an array of pointers called file descriptor tables, these
pointers point to files somewhere in memory. since we are on linux, which treats everything
as a file, sockets are going to be just that, file descriptors that are an index to the same
table as files, and they point to somewhere in memory where you can read/write, and the os
handles the rest
*/
