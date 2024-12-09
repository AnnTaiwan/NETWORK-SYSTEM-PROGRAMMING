/*
 * usock_server : listen on a Unix socket ; fork ;
 *                child does lookup ; replies down same socket
 * argv[1] is the name of the local datafile
 * PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include "dict.h"

int main(int argc, char **argv) {
    struct sockaddr_un server, client;
    int sd,cd,n;
    Dictrec tryit;
    // I add
    n = sizeof(struct sockaddr_un); // client's addr size
    
    if (argc != 3) {
      fprintf(stderr,"Usage : %s <dictionary source> "
          "<Socket name>\n",argv[0]);
      exit(errno);
    }

    /* Setup socket.
     * Fill in code. */
    if( (sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        DIE("usock_server.c:socket");
    /* Initialize address.
     * Fill in code. */
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, argv[2]);
    unlink(server.sun_path);

    if( bind(sd, (struct sockaddr*)&server, sizeof(struct sockaddr_un)) == -1 ) 
        DIE("usock_server.c:bind");

   
    /* Name and activate the socket.
     * Fill in code. */
     /* Prepare to receive multiple connect requests */
    if( listen(sd, 10) == -1 )
        DIE("usock_server.c:listen");
    /* main loop : accept connection; fork a child to have dialogue */
    for (;;) {
		/* Wait for a connection.
		 * Fill in code. */
        cd = accept(sd, (struct sockaddr*)&client, (unsigned int *)&n);
        if(cd == -1)
            DIE("usock_server.c:accept");
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
                    //printf("[Server] receives word: %s\n", tryit.word);
					/* Lookup request. */
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

				/* Terminate child process.  It is done. */
				exit(0);

			/* Parent continues here. */
			default :
				close(cd);
				break;
		} /* end fork switch */
    } /* end forever loop */
} /* end main */
