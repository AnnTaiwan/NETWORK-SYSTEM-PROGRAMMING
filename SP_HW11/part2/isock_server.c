/*
 * isock_server : listen on an internet socket ; fork ;
 *                child does lookup ; replies down same socket
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
	static struct sockaddr_in server;
	int sd,cd; //,n; // not used
	Dictrec tryit;

	if (argc != 2) {
		fprintf(stderr,"Usage : %s <datafile>\n",argv[0]);
		exit(1);
	}

	/* Create the socket.
	 * Fill in code. */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
         DIE("socket");
    int opt = 1;
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
        DIE("setsockopt failed");
	/* Initialize address.
	 * Fill in code. */
    /* fill in socket address structure */
    memset((char *) &server, '\0', sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY; // accept any resource's connection
     /* bind socket to a name */
    if (bind(sd, (struct sockaddr *) &server, sizeof(server)))
        DIE("bind");
	/* Name and activate the socket.
	 * Fill in code. */
    /* prepare to receive multiple connect requests */
    if ( listen(sd, 10) == -1 )
    {
        DIE("listen");
    }   
	/* main loop : accept connection; fork a child to have dialogue */
	for (;;) {

		/* Wait for a connection.
		 * Fill in code. */
        if ((cd = accept(sd, NULL, NULL)) == -1)
            DIE("accept");
		/* Handle new client in a subprocess. */
		switch (fork()) {
			case -1 : 
				DIE("fork");
			case 0 :
				close (sd);	/* Rendezvous socket is for parent only. */
				/* Get next request.
				 * Fill in code. */
				memset(&tryit, '\0', sizeof(tryit)); // reset it
				while (read(cd, tryit.word, sizeof(tryit.word))) { // read the requested word from client
					/* Lookup the word , handling the different cases appropriately */
					switch(lookup(&tryit,argv[1]) ) {
						/* Write response back to client. */
						case FOUND: 
							/* Fill in code. */
							//printf("\tFOUND word: %s. Write text to client.\n", tryit.word);
							write(cd, tryit.text, sizeof(tryit.text)); // send to client the result
							break;
						case NOTFOUND: 
							/* Fill in code. */
							//printf("\tNOTFOUND %s word.\n", tryit.word);
							write(cd, "XXXX", 5); // not found indicator
							break;
						case UNAVAIL:
							DIE(argv[1]);
					} /* end lookup switch */
				} /* end of client dialog */
				exit(0); /* child does not want to be a parent */

			default :
				close(cd);
				break;
		} /* end fork switch */
	} /* end forever loop */
} /* end main */
