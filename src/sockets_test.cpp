#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <iostream>
#include "garbage.h"

int main() {
    ServerSocket socket(8080);
  
    InternetSocket peer = socket.getConnection();

    char* msg = static_cast<char*>(calloc(2048, sizeof(char)));
    peer.recieve(msg, 2047 * sizeof(char));

    std::cout << "Message Recieved!\n";
    std::cout << msg;
    std::cout << "\nConnection Ip: " << peer.getIp();
    return 0;
}


// i just changed the file name to cpp and now I'm in so many rabbit holes!


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

// i think the bug because im gviovinmg stack buffer when it accepts heap[ buffer, it should be going the opposite way?] im using gdb and im more confused