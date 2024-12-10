/* srvr3.c - Server - full example
 * AF_INET, SOCK_STREAM Time Server
 * Server sends its current time */
 
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
    struct utsname name;
    struct sockaddr_in socketname, client;
    int sd, ns, clientlen = sizeof(client);
    struct hostent *host;
    time_t today;

    /* determine server system name and internet address */
    if (uname(&name) == -1) {
        perror("uname");
        exit(1);
    }

    if ((host = gethostbyname(name.nodename)) == NULL) {
        perror("gethostbyname");
        exit(1);
    }

    /* fill in socket address structure */
    memset((char *) &socketname, '\0', sizeof(socketname));
    socketname.sin_family = AF_INET;
    socketname.sin_port = PORTNUM;
    memcpy((char *) &socketname.sin_addr, host->h_addr, host->h_length);

    /* open socket */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    /* bind socket to a name */
    if (bind(sd, (struct sockaddr *) &socketname, sizeof(socketname))) {
        perror("bind");
        exit(1);
    }

    /* prepare to receive multiple connect requests */
    if (listen(sd, 128)) {
        perror("listen");
        exit(1);
    }

    while (1) {
        if ((ns = accept(sd, (struct sockaddr *)&client, (unsigned int *)&clientlen)) == -1) {
            perror("accept");
            exit(1);
        }

        /* get current time and date */
        time(&today);

        /* send time through socket */
        if (write(ns, &today, sizeof(today)) == -1) {
            perror("write");
            exit(1);
        }
        close(ns);
    }
}

