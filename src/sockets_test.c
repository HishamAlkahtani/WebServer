#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>


int main() {
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);    
    printf("socket file descriptor: %d\n", socketfd);
    
    if (socketfd == -1) {
        printf("Failed call to socket(), exiting");
        return 1;
    }

    struct sockaddr *a;
    printf("")
}


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

*/