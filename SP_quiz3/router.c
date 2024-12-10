#include "message.h"
void sigusr1_handler(int signo) {
    printf("[Router] Received SIGUSR1 signal.\n");
}
int main(int argc, char *argv[]) 
{
    int cd; // new fd after accept
    struct sockaddr_un server, client;
    int sd;
    int namesize = sizeof(struct sockaddr_un);
    int clientsize = sizeof(struct sockaddr_un);
    static int first_time = 1;
    Message msg;
    if(first_time)
    {
        first_time = 0;
        
              /* Create server's rendezvous socket sd */
            if( (sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) 
                errExit("router:socket");

            /* Fill in server's address and bind it to sd */
            server.sun_family = AF_UNIX;
            strcpy(server.sun_path, RESOURCE);
            unlink( server.sun_path );

            if( bind(sd, (struct sockaddr*)&server, namesize) == -1 )
                errExit("router:bind");

            /* Prepare to receive multiple connect requests */
            if( listen(sd, MAX_NUMBER_CLIENT) == -1 )
                errExit("router:listen");

        /* Infinite loop to accept client requests */
        for(int i = 0; i < MAX_NUMBER_CLIENT; i++)
        {
            cd = accept(sd, (struct sockaddr*)&client, (unsigned int *)&clientsize);
            if(cd == -1)
                errExit("usock_server.c:accept");
		    /* Handle new client in a subprocess. */
		    switch (fork()) {
			    case -1 : 
				    errExit("fork");
			    case 0 :
				    close (sd);	/* Rendezvous socket is for parent only. */
				    /* Get next request.
				     * Fill in code. */
				    memset(msg.buffer, 0, sizeof(msg.buffer));
				    while (read(cd, &msg, sizeof(msg))) { // read the requested word from client
                        
                        
                        
                        
                        

				    } /* end of client dialog */

				    /* Terminate child process.  It is done. */
				    exit(0);

			    /* Parent continues here. */
			    default :
				    close(cd);
				    break;
		    } /* end fork switch */
        }
    }
    
    exit(EXIT_SUCCESS);
}

