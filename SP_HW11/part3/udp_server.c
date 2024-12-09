/*
 * udp_server : listen on a UDP socket ;reply immediately
 * argv[1] is the name of the local datafile
 * PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>

#include "dict.h"

int main(int argc, char **argv) {
	static struct sockaddr_in server,client;
	int sockfd,siz;
	Dictrec dr, *tryit = &dr;

	if (argc != 2) {
		fprintf(stderr,"Usage : %s <datafile>\n",argv[0]);
		exit(errno);
	}

	/* Create a UDP socket.
	 * Fill in code. */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        DIE("Socket creation failed");
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
        DIE("setsockopt failed");
	/* Initialize address.
	 * Fill in code. */
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY; // 伺服器的 IP 地址
	/* Name and activate the socket.
	 * Fill in code. */
    if (bind(sockfd, (const struct sockaddr *)&server, sizeof(server)) < 0)
    {
        DIE("Bind failed");
    }
	for (;;) { /* await client packet; respond immediately */

		siz = sizeof(client); /* siz must be non-zero */

		/* Wait for a request.
		 * Fill in code. */
        int n; // received bytes
		while ((n = recvfrom(sockfd, tryit -> word, sizeof(tryit -> word), 0, (struct sockaddr *)&client, (unsigned int*)&siz)) > 0) {
		    tryit -> word[n] = '\0';
			/* Lookup request and respond to user. */
			switch(lookup(tryit,argv[1]) ) {
				case FOUND: 
					/* Fill in code. */
					//printf("\tFOUND word: %s. Write text to client.\n", tryit.word);
					sendto(sockfd, tryit -> text, sizeof(tryit -> text), 0, (const struct sockaddr *)&client, siz);
					break;
				case NOTFOUND: 
					/* Fill in code. */
					//printf("\tNOTFOUND %s word.\n", tryit.word);
					sendto(sockfd, "XXXX", 5, 0, (const struct sockaddr *)&client, siz);
					break;
				case UNAVAIL:
					DIE(argv[1]);
			} /* end lookup switch */
		} /* end while */
	} /* end forever loop */
} /* end main */
