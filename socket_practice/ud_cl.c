/* clnt2.c - AF_UNIX, SOCK_DGRAM */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#define BUFSZ 256

int main() {
    int namesize = sizeof(struct sockaddr_un);
    int sd;
    struct sockaddr_un clnt, srvr;
    char buf[BUFSZ];

    /* Create client's socket sd */
    if( (sd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        perror("clnt2.c:main:socket");
        exit(1);
    }

    /* Fill in client's address and bind it to sd */
    clnt.sun_family = AF_UNIX;
    strcpy(clnt.sun_path, "/tmp/socket2clnt");
    unlink(clnt.sun_path);

    if( bind(sd, (struct sockaddr*)&clnt, namesize) == -1 ) {
        perror("clnt2.c:main:bind");
        exit(1); 
    }

    /* Fill in server's address for sendto server */
    srvr.sun_family = AF_UNIX;
    strcpy(srvr.sun_path, "/tmp/socket2srvr");

    /* Communicate with server */
    if(sendto(sd, "Deac", 5, 0, (struct sockaddr*)&srvr, namesize) == -1) {
        perror("clnt2.c:main:sendto");
        exit(1);
    }

    if( recvfrom(sd, buf, BUFSZ, 0, (struct sockaddr*)&srvr, (unsigned int *)&namesize) == -1) {
        perror("clnt2.c:main:recvfrom");
        exit(1);
    }
    printf("CLIENT REC'D: %s\n", buf);
    close(sd);
    unlink(clnt.sun_path);
}

