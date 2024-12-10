#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr;

    // 1️⃣ 創建套接字 (SOCK_DGRAM - UDP)
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2️⃣ 設定伺服器的 IP 地址和端口
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 伺服器的 IP 地址

    // 3️⃣ 發送數據到伺服器 (sendto)
    const char *message = "Hello from the client!";
    sendto(sockfd, message, strlen(message), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("Message sent to server.\n");

    // 4️⃣ 接收來自伺服器的回覆 (recvfrom)
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
    if (n < 0) {
        perror("recvfrom error");
    }

    buffer[n] = '\0'; // 確保字符串的結尾
    printf("Received from server: %s\n", buffer);

    // 5️⃣ 關閉套接字
    close(sockfd);
    return 0;
}

