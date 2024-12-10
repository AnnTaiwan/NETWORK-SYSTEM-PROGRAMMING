## us_cl & us_sv
Server (srvr1.c)                      Client (clnt1.c)
--------------------------------      ---------------------------
sd = socket(AF_UNIX, SOCK_STREAM, 0);  sd = socket(AF_UNIX, SOCK_STREAM, 0);

strcpy(myname.sun_path, "/tmp/socket1");
unlink(myname.sun_path);
bind(sd, (struct sockaddr*)&myname, namesize);

listen(sd, 128);                      srvr.sun_path = "/tmp/socket1";
                                      connect(sd, (struct sockaddr*)&srvr, namesize);

accept(sd, &client, &clientsize);    <------>  交換數據  <-------->
read(comm, &dub, sizeof(dub));        write(sd, &dub, sizeof(dub));
dub += dub;
write(comm, &dub, sizeof(dub));       read(sd, &dub2, sizeof(dub2));

close(comm);                          close(sd);

## ud_cl & ud_sv
   伺服器 (Server)                      客戶端 (Client)
-----------------------------------------------------------
1. socket() 創建 sd                     1. socket() 創建 sd

2. unlink("/tmp/socket2srvr")           2. unlink("/tmp/socket2clnt")

3. bind(sd, "/tmp/socket2srvr")         3. bind(sd, "/tmp/socket2clnt")

4. 等待 recvfrom() (阻塞)               4. sendto(sd, "Deac", 5, 0, srvr)

5. recvfrom() 收到消息 "Deac"           5. 等待伺服器的響應

6. sendto(sd, "Deac, Hello from server") 

7. 客戶端通過 recvfrom() 收到回應       6. recvfrom(sd, buf)
                                       
8. 客戶端顯示 "CLIENT REC'D: Deac, Hello from the server"

## is_cl & is_sv
  [伺服器] (Server)                     [客戶端] (Client)
  ----------------------------------------------------------
   1. socket()                         1. socket()
   2. bind()  (綁定到IP:PORT)           2. gethostbyname() (尋找伺服器)
   3. listen() (開始監聽)               3. connect() (連接伺服器)
   4. accept() (接受連接)               4. read() (從伺服器接收數據)
   5. time() (獲取當前時間)              5. 輸出時間
   6. write() (將時間發送到客戶端)        6. close() (關閉連接)
   7. close() (結束與客戶端的連接)

## id_cl & id_sv
       [ 伺服器 ]                                   [ 客戶端 ]
---------------------------------------------------------------
 1️⃣    socket()                                    socket() 
        ↓                                           ↓
 2️⃣    bind()                                      -
        ↓                                           ↓
 3️⃣    recvfrom()  (等待客戶端的消息)                sendto() (發送消息到伺服器)
        ↓  <----------------------------消息-------------------
 4️⃣    處理並打印客戶端的消息                       -
        ↓                                           ↓
 5️⃣    sendto() (發送回覆消息)                     recvfrom() (等待伺服器的回覆)
        ----------------------------回覆消息------------------->
 6️⃣    繼續等待下一個客戶端的數據                   顯示伺服器的回覆

* use `sudo lsof -i :5678` to see the open port
* And, use `kill -9 <pid>` to delete that
