/* srvr2.c - AF_UNIX, SOCK_DGRAM
 * Server reads a string from client and
 * sends back a greeting appended to the string */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#define BUFSZ  256
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
    
    //int comm;
    struct sockaddr_un client;
    int namesize = sizeof(struct sockaddr_un);
    char buf[BUFSZ];

    /* Create server's socket sd */
    if( (sd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        perror("srvr2.c:main:socket");
        exit(1);
    }

    /* Fill in server's address and bind it to sd */
    myname.sun_family = AF_UNIX;
    strcpy(myname.sun_path, "/tmp/socket2srvr");
    unlink(myname.sun_path);

    if( bind(sd, (struct sockaddr*)&myname, namesize) == -1 ) {
        perror("srvr2.c:main:bind");
        exit(1);
    }

    /* Infinite loop to accept client requests */
    while(1){
        if( recvfrom(sd, (char*)&buf, BUFSZ, 0, (struct sockaddr*)&client, (unsigned int *)&namesize) == -1) {
            perror("srvr2.c:main:recvfrom");
            exit(1);
        }
        char msg[102];
        snprintf(msg, 102, ", Hello from the server %ld", time(NULL));
        strcat(buf, msg);

        if( sendto(sd, buf, BUFSZ, 0, (struct sockaddr*)&client, namesize) == -1) {
            perror("srvr2.c:main:sendto");
            exit(1);
        }
    }

    /* Install a signal handler for cleanup including:
     * close(sd);
     * unlink(myname.sun_path);
     */
}

