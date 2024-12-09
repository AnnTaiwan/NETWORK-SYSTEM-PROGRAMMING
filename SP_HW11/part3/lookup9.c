/*
 * lookup9 : does no looking up locally, but instead asks
 * a server for the answer. Communication is by Internet UDP Sockets
 * The name of the server is passed as resource. PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include "dict.h"

int lookup(Dictrec * sought, const char * resource) {
	static int sockfd;
	static struct sockaddr_in server;
	struct hostent *host;
	static int first_time = 1;

	if (first_time) {  /* Set up server address & create local UDP socket */
		first_time = 0;

		/* Set up destination address.
		 * Fill in code. */
        server.sin_family = AF_INET;
        server.sin_port = htons(PORT);
        
        if ((host = gethostbyname(resource)) == NULL)
            DIE("gethostbyname");
            
        memcpy((char *) &server.sin_addr, host->h_addr, host->h_length);
		/* Allocate a socket.
		 * Fill in code. */
		sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0)
            DIE("Socket creation failed");
	    
    }
	/* Send a datagram & await reply
	 * Fill in code. */
	int server_size = sizeof(struct sockaddr_in);
    int n = sendto(sockfd, sought -> word, sizeof(sought -> word), 0, (const struct sockaddr *)&server, server_size); // request info about this word to server
    if (n < 0) {
        DIE("sendto error");
    }
    memset(sought -> text, '\0', sizeof(sought -> text)); // clear it
    n = recvfrom(sockfd, sought -> text, sizeof(sought -> text), 0, NULL, NULL); // get the reply
    if (n < 0) {
        DIE("recvfrom error");
    }
	if (strcmp(sought->text,"XXXX") != 0) {
		return FOUND;
	}

	return NOTFOUND;
}
