/* srvr1.c -  AF_UNIX, SOCK_STREAM
 * Server reads a value from client and
 * sends back twice that value */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int sd; // 這是你的套接字描述符
struct sockaddr_un myname; // 這是用於 bind 的地址

void cleanup(int signum) {
    printf("\nCleaning up resources...\n");

    // 關閉套接字
    if (sd != -1) {
        close(sd);
        printf("Socket closed.\n");
    }

    // 刪除套接字文件
    if (myname.sun_path[0] != '\0') {
        unlink(myname.sun_path);
        printf("Socket file %s unlinked.\n", myname.sun_path);
    }

    exit(0); // 正常退出程序
}
int main() {

    // 設定信號處理函數，當 Ctrl+C 發生時會調用 cleanup 函數
    signal(SIGINT, cleanup);
    
    int comm;
    struct sockaddr_un client;
    int namesize = sizeof(struct sockaddr_un);
    int clientsize = sizeof(struct sockaddr_un);
    double dub;

    /* Create server's rendezvous socket sd */
    if( (sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("srvr1.c:main:socket");
        exit(1);
    }

    /* Fill in server's address and bind it to sd */
    myname.sun_family = AF_UNIX;
    strcpy(myname.sun_path, "/tmp/socket1");
    unlink( myname.sun_path );

    if( bind(sd, (struct sockaddr*)&myname, namesize) == -1 ) {
        perror("srvr1.c:main:bind");
        exit(1);
    }

    /* Prepare to receive multiple connect requests */
    if( listen(sd, 128) == -1 ) {
        perror("srvr1.c:main:listen");
        exit(1);
    }

    /* Infinite loop to accept client requests */
    while(1) {
        comm = accept(sd, (struct sockaddr*)&client,(unsigned int *)&clientsize);
        if( comm == -1) {
            perror("srvr1.c:main:accept");
            exit(1);
        }
        printf("Receive one msg\n");
        read(comm, &dub, sizeof(dub));
        dub += dub;
        write(comm, &dub, sizeof(dub));
        close(comm);
    }

    /* Install a signal handler for cleanup including:
     * close(sd);
     * unlink(myname.sun_path);  */
}

