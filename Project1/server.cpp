#include <iostream>
#include <netinet/in.h>



using namespace std;

int main(){

//once request is recieved in the handshake port 
//reply back with a random port number <r port> between 1024 and 65.... where it will be listening 





//receiving
int mysocket = 0;
mysocket=socket(AF_INET, SOCK_DGRAM, 0);
struct sockaddr_in server;



memset((char *) &server, 0, sizeof(server));


server.sin_family = AF_INET;
server.sin_port = htons(7123);
server.sin_addr.s_addr = htonl(INADDR_ANY);


//Binds the socket to port # and IP address

bind(mysocket,
 (struct sockaddr *)&server,
 sizeof(server));
cout<<"Random port chosen: <r_port>";

/*
listen using mysocket and store received data in payload
which has size 512 bytes
also
store client IP and port number in struct sockaddr*/
struct sockaddr_in client;
socklen_t clen = sizeof(client);
recvfrom(mysocket, payload, 512, 0,
 (struct sockaddr *)&client, &clen)


//close socket
close(mysocket);

    return 0;
}