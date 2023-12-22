#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>

int main(int argc, char** argv) {
    struct in_addr internetaddress;
    inet_aton("127.0.0.1", &internetaddress);
    struct sockaddr_in myaddr;
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = 80;
    myaddr.sin_addr = internetaddress;
    memset(&(myaddr.sin_zero), 0, 8);    

    int sfd = socket(AF_INET, SOCK_STREAM, 6);
    
    bind(sfd, &myaddr, sizeof(myaddr));
    struct sockaddr_in peeradr;
    memset(&peeradr, 0, sizeof(peeradr));
    socklen_t peeradrlen = sizeof(peeradr);
    while (accept(sfd, &peeradr, &peeradrlen) == -1);

    printf("socket sfile descriptor: %d", sfd);
    return 0;
}