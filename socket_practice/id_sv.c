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
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // 1️⃣ 創建套接字 (SOCK_DGRAM - UDP)
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2️⃣ 初始化伺服器地址
    server_addr.sin_family = AF_INET;         // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // 監聽本機的所有 IP
    server_addr.sin_port = htons(PORT);      // 端口號，轉換為網絡字節序

    // 3️⃣ 綁定地址和端口到套接字
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server is running and waiting for messages on port %d...\n", PORT);

    while (1) {
        // 4️⃣ 等待來自客戶端的消息 (recvfrom)
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
        if (n < 0) {
            perror("recvfrom error");
            break;
        }

        buffer[n] = '\0'; // 確保以 null 結尾的字符串
        printf("Received from client: %s\n", buffer);

        // 5️⃣ 回覆客戶端 (sendto)
        const char *response = "Hello from the server!";
        sendto(sockfd, response, strlen(response), 0, (const struct sockaddr *)&client_addr, client_len);
        printf("Sent response to client.\n");
    }

    // 6️⃣ 關閉套接字
    close(sockfd);
    return 0;
}

