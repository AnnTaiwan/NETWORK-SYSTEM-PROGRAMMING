#include "message.h"
int main(int argc, char *argv[]) 
{

    // create router Process
    pid_t router_pid = fork();
    if (router_pid == 0) { //
        execl("./router", "router",  NULL); // execute Producer process
        errExit("Router execution failed");
    }
    // create client Processes
    pid_t clients[MAX_NUMBER_CLIENT];
    for (int i = 0; i < MAX_NUMBER_CLIENT; i++) {
        clients[i] = fork();
        if (clients[i] == 0) {
            char client_id[10];
            sprintf(client_id, "%d", i);
            execl("./client", "client", i + 1, NULL); // execute Consumer process
            errExit("Client execution failed");
        }
    }
    // send a signal to producer to inform it the consumers ready
    int sec = 5;
    sleep(sec);
    int src, dest;
    Message msg;
    while(scanf("Please input the sender and the receiver: %d %d", &src, &dest))
    {
        memset(msg.buffer, 0, sizeof(msg.buffer));
        if(src >= 1 && src <= MAX_NUMBER_CLIENT && dest == 0)
        {
            for(int i = 0; i < MAX_NUMBER_CLIENT; i++)
            {
                msg.src_id = src;
                msg.dest_id = dest;
               
            }
        }
        else if(src >= 1 && src <= MAX_NUMBER_CLIENT && dest >= 1 && dest <= MAX_NUMBER_CLIENT)
        {
            
        
        }
        else
        {
            printf("ERROR! This is not valaid src and dest id\n");
        }
    }
    
    
    
    
    
    waitpid(router_pid, NULL, 0);
    printf("\t[Router] wait %d clients to finish...\n", MAX_NUMBER_CLIENT);
    for (int i = 0; i < MAX_NUMBER_CLIENT; i++) {
        waitpid(clients[i], NULL, 0);
    }
    printf("\t[Router] all router and clients finished !\n");
    
    return 0;
}

