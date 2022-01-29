
//Author: Joshua Moore
//jjm702


#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <sys/un.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/in.h>

using namespace std;



//notes
// lookup more information about fd opened from socket() call


int main(int argc,char* argv[2]){

    //<server_address
argv[0]="127.0.0.1"

//n port
argv[1]=



// Handshake phase

//create socket
//used in class slides and https://docs.oracle.com/cd/E19620-01/805-4041/6j3r8iu2l/index.html


//s = socket(domain, type, protocol);
// Internet domain ,  UDP uses SOCK_DGRAM https://stackoverflow.com/questions/5815675/what-is-sock-dgram-and-sock-stream, 
//This call results in a stream socket with the TCP protocol providing the underlying communication. The following creates a datagram socket for intramachine use

mysocket=socket(AF_INET, SOCK_DGRAM, 0);

//get host ip address
//holds IP address of destination host
// resolves host name to IP address
struct hostent *s;
s = gethostbyname(argv[1]);





//setting destination
//sockaddr_in is structure for containing internet address 

struct sockaddr_in server;
memset((char *) &server, 0, sizeof(server));
server.sin_family = AF_INET;
//htons() converts port number to network byte order 

server.sin_port = htons(7123);
//bcopy(…) stores IP address as server.sin_addr

bcopy((char *)s->h_addr,
 (char *)&server.sin_addr.s_addr,
 s->h_length);




//sending information 
socklen_t slen = sizeof(server);

char payload[512]=“Hello”;
sendto(mysocket, payload, 512, 0, (struct
sockaddr *)&server, slen)

//close socket
close(mysocket);
    return 0;
}