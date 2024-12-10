## **1️⃣ us_cl & us_sv (UNIX Stream Sockets, `AF_UNIX`, `SOCK_STREAM`)**

| **伺服器 (Server: `us_sv`)**                 | **客戶端 (Client: `us_cl`)**                |
|--------------------------------------------|--------------------------------------------|
| 1️⃣ `sd = socket(AF_UNIX, SOCK_STREAM, 0);` | 1️⃣ `sd = socket(AF_UNIX, SOCK_STREAM, 0);` |
| 2️⃣ `strcpy(myname.sun_path, "/tmp/socket1");` <br> `unlink(myname.sun_path);` <br> `bind(sd, (struct sockaddr*)&myname, namesize);` | 2️⃣ `srvr.sun_path = "/tmp/socket1";` <br> `connect(sd, (struct sockaddr*)&srvr, namesize);` |
| 3️⃣ `listen(sd, 128);`                      |                                            |
| 4️⃣ `comm = accept(sd, &client, &clientsize);` | 🔁 **數據交換 (雙向)** 🔁                   |
| 5️⃣ `read(comm, &dub, sizeof(dub));`       | 5️⃣ `write(sd, &dub, sizeof(dub));`        |
| 6️⃣ `dub += dub;`                          |                                            |
| 7️⃣ `write(comm, &dub, sizeof(dub));`      | 6️⃣ `read(sd, &dub2, sizeof(dub2));`       |
| 8️⃣ `close(comm);`                         | 7️⃣ `close(sd);`                           |

---

## **2️⃣ ud_cl & ud_sv (UNIX Datagram Sockets, `AF_UNIX`, `SOCK_DGRAM`)**

| **伺服器 (Server: `ud_sv`)**                | **客戶端 (Client: `ud_cl`)**                |
|--------------------------------------------|--------------------------------------------|
| 1️⃣ `sd = socket(AF_UNIX, SOCK_DGRAM, 0);` | 1️⃣ `sd = socket(AF_UNIX, SOCK_DGRAM, 0);`  |
| 2️⃣ `unlink("/tmp/socket2srvr");` <br> `bind(sd, "/tmp/socket2srvr");` | 2️⃣ `unlink("/tmp/socket2clnt");` <br> `bind(sd, "/tmp/socket2clnt");` |
| 3️⃣ **等待接收消息**: <br> `recvfrom(sd, buf, sizeof(buf), 0, &client, &clientsize);` | 3️⃣ **發送消息**: <br> `sendto(sd, "Deac", 5, 0, (struct sockaddr*)&srvr, sizeof(srvr));` |
| 4️⃣ **回應客戶端**: <br> `sendto(sd, "Hello from server", 18, 0, (struct sockaddr *)&client, clientsize);` | 4️⃣ **等待伺服器的回覆**: <br> `recvfrom(sd, buf, sizeof(buf), 0, NULL, NULL);` |

---

## **3️⃣ is_cl & is_sv (Internet Stream Sockets, `AF_INET`, `SOCK_STREAM`)**

| **伺服器 (Server: `is_sv`)**                | **客戶端 (Client: `is_cl`)**                |
|--------------------------------------------|--------------------------------------------|
| 1️⃣ `sd = socket(AF_INET, SOCK_STREAM, 0);` | 1️⃣ `sd = socket(AF_INET, SOCK_STREAM, 0);` |
| 2️⃣ `server.sin_family = AF_INET;` <br> `server.sin_port = htons(PORT);` <br> `server.sin_addr.s_addr = INADDR_ANY;` <br> `bind(sd, (struct sockaddr*)&server, sizeof(server));` | 2️⃣ `gethostbyname()` (獲取伺服器 IP 地址) |
| 3️⃣ `listen(sd, 128);`                      | 3️⃣ `connect(sd, (struct sockaddr*)&srvr, sizeof(srvr));` |
| 4️⃣ `comm = accept(sd, &client, &clientsize);` | 🔁 **數據交換 (雙向)** 🔁                   |
| 5️⃣ **發送時間**: <br> `time(&now);` <br> `write(comm, &now, sizeof(now));` | 4️⃣ **讀取時間**: <br> `read(sd, &now, sizeof(now));` |
| 6️⃣ `close(comm);`                         | 5️⃣ `close(sd);`                           |

---

## **4️⃣ id_cl & id_sv (Internet Datagram Sockets, `AF_INET`, `SOCK_DGRAM`)**

| **伺服器 (Server: `id_sv`)**                | **客戶端 (Client: `id_cl`)**                |
|--------------------------------------------|--------------------------------------------|
| 1️⃣ `sd = socket(AF_INET, SOCK_DGRAM, 0);` | 1️⃣ `sd = socket(AF_INET, SOCK_DGRAM, 0);`  |
| 2️⃣ `bind(sd, (struct sockaddr*)&server, sizeof(server));` |                                            |
| 3️⃣ **等待接收消息**: <br> `recvfrom(sd, buf, sizeof(buf), 0, &client, &clientsize);` | 2️⃣ **發送消息**: <br> `sendto(sd, "Hello, Server!", 15, 0, (struct sockaddr*)&server, sizeof(server));` |
| 4️⃣ **回應客戶端**: <br> `sendto(sd, "Hello, Client!", 15, 0, (struct sockaddr *)&client, clientsize);` | 3️⃣ **接收回覆**: <br> `recvfrom(sd, buf, sizeof(buf), 0, NULL, NULL);` |
| 5️⃣ **伺服器繼續等待**                     | 4️⃣ **顯示伺服器的回覆**                   |

---

## 🛠️ **如何檢查開放的端口？**

**檢查是否有程式佔用某個端口**：
```bash
sudo lsof -i :5678
```

**結果範例**：
```
COMMAND   PID    USER   FD   TYPE DEVICE SIZE/OFF NODE NAME
server   12345  user    3u  IPv4  13245      0t0  TCP *:5678 (LISTEN)
```

**結束佔用該端口的程式**：
```bash
kill -9 12345  # 12345 是 lsof 命令中的 PID
```

---

## 📘 **關鍵函數說明**

| **函數**             | **描述**                            |
|---------------------|------------------------------------|
| `socket()`           | 創建一個**套接字**，返回一個文件描述符 |
| `bind()`             | 將**IP 和端口綁定**到套接字           |
| `listen()`           | 使伺服器進入**監聽狀態**              |
| `accept()`           | **接受客戶端的連接**，並返回通信用的套接字 |
| `connect()`          | **連接伺服器**，用於客戶端連接伺服器  |
| `sendto()` / `recvfrom()` | 用於**UDP 無連接通信**            |
| `write()` / `read()` | 用於**TCP 通信**                    |
| `close()`            | **關閉套接字**                      |

