#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>


void init_sockaddr(struct sockaddr_in *addr) {
    memset(addr, 0, sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(8080);
    addr->sin_addr.s_addr = INADDR_ANY; // vs INADDR_LOOPBACK? what is this
}

int main() {
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);    
    printf("socket file descriptor: %d\n", socketfd);
    
    if (socketfd == -1) {
        printf("Failed call to socket(), exiting");
        return 1;
    }

    struct sockaddr_in addr;
    init_sockaddr(&addr);
    bind(socketfd, &addr, sizeof(struct sockaddr_in));    
    listen(socketfd, 1);

    struct sockaddr_in peeraddr;
    socklen_t peerlen = sizeof(struct sockaddr_in);
    int connection_fd = accept(socketfd, &peeraddr, &peerlen);  // why doesn't this accept stuff?

    char msg[2048];
    memset(msg, 0, 2084 * sizeof(char));
    msg[2047] = '\0';
    recv(connection_fd, msg, 2047 * sizeof(char), 0);

    printf("msg recievedd!!!:\n%s", msg);
        
    return 0;
}


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