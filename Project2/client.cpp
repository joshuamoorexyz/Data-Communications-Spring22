// Author: Maxwell Young
// Created for Fall 2021 Data Comm. for helping a student with serialization

// To test this code, you need two shells open on Pluto
// run ./server
// run ./client localhost


#include <stdlib.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sys/types.h>   // defines types (like size_t)
#include <sys/socket.h>  // defines socket class
#include <netinet/in.h>  // defines port numbers for (internet) sockets, some address structures, and constants
#include <netdb.h> 
#include <iostream>
#include <fstream>
#include <arpa/inet.h>   // if you want to use inet_addr() function
#include <string.h>
#include <unistd.h>
#include "packet.h" // include packet class
#include <math.h>


using namespace std;

//#define packetLen 50

int main(int argc, char *argv[]){

  int packetLen=50;
  struct hostent *s; 
  s = gethostbyname(argv[1]);
  
  struct sockaddr_in server;
  int mysocket = 0;
  socklen_t slen = sizeof(server); 
  
  if ((mysocket=socket(AF_INET, SOCK_DGRAM, 0))==-1)
    cout << "Error in creating socket.\n";
    
  memset((char *) &server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(7123);
  bcopy((char *)s->h_addr, 
	(char *)&server.sin_addr.s_addr,
	s->h_length);
      
  	  
  // now going to send serialize and send 3 packets
  char payloadA[512]="123"; // payload data

  char payloadB[512]="abcd"; // another payload

  
  char spacketA[packetLen];  // for holding serialized packet  
  memset(spacketA, 0, packetLen); // serialize the packet to be sent

  char spacketB[packetLen];
  memset(spacketB, 0, packetLen);

  char spacketC[packetLen];
  memset(spacketC, 0, packetLen);
  
  packet mySendPacket(1, 101, strlen(payloadA), payloadA); // make the packet to be serialized and sent

  packet mySendPacketB(1, 201, strlen(payloadB), payloadB);
  
  mySendPacket.serialize(spacketA); // serialize so spacket contains serialized contents of mySendPacket's payload
  cout << "Sending serialized packet with payload:" << payloadA << endl;
  cout << "This is what the serialized packet looks like: " << spacketA << endl;
  sendto(mysocket, spacketA, 50, 0, (struct sockaddr *)&server, slen);

  mySendPacketB.serialize(spacketB);
  cout << "Sending serialized packet with payloadB:" << payloadB << endl;
  cout << "This is what the serialized packet looks like: " << spacketB << endl;
  sendto(mysocket, spacketB, 50, 0, (struct sockaddr *)&server, slen);




  
  close(mysocket);
  return 0;
}
