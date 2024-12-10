## SP_HW11
### Part1
* Set up a client and server while talk over **TCP AF_UNIX** sockets. The server performs the dictionary look up.
#### How to use?
```
make
./usock_server fixrec TUNNEL &
./usock_client TUNNEL
```
### Part2
* Set up a client and server while talk over **TCP AF_INET** sockets. The server performs the dictionary look up.
#### How to use?
```
make
./isock_server fixrec TUNNEL &
./isock_client localhost
```
### Part3
* Set up a client and server while talk over **UDP AF_INET** sockets. server performs the dictionary look up.
#### How to use?
```
make
./udp_server fixrec TUNNEL &
./udp_client localhost
```
