/* clnt3.c - Client full example
 * AF_INET, SOCK_STREAM Client of Time Server */
 
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define PORTNUM 5998

int main() {
    int sd;
    struct sockaddr_in server;
    struct hostent *host;
    time_t srvrtime;
    struct utsname name;

    /* create the socket for talking to server */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    /* get server internet address and put into addr structure */
    memset((char *) &server, '\0', sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = PORTNUM;

    /* Server is local system. Get its name. */
    if (uname(&name) == -1) {
        perror("uname");
        exit(1);
    }

    if ((host = gethostbyname(name.nodename)) == NULL) {
        perror("gethostbyname");
        exit(1);
    }
    
    memcpy((char *) &server.sin_addr, host->h_addr, host->h_length);

    /* connect to server */
    if (connect(sd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("connect");
        exit(1);
    }

    /* read the time and date passed from the server */
    if (read(sd, &srvrtime, sizeof(srvrtime)) == -1) {
        perror("recv");
        exit(1);
    }

    close(sd);

    printf("Server's time is: %s", ctime(&srvrtime));
}

