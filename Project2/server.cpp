// Maxwell Young
// Date: Sep. 20, 2021

#include<iostream>
#include <sys/types.h>   // defines types (like size_t)
#include <sys/socket.h>  // defines socket class
#include <netinet/in.h>  // defines port numbers for (internet) sockets, some address structures, and constants
#include <time.h>        // used for random number generation
#include <string.h> // using this to convert random port integer to string
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include "packet.h" // include packet class

using namespace std;

int main(int argc, char *argv[]){
  
  struct sockaddr_in server;
  struct sockaddr_in client;
  int mysocket = 0;
  int i = 0;
  socklen_t clen = sizeof(client);

  char payload[512];
  memset(payload, 0, 512);
  char serialized[512];
  memset(serialized, 0, 512); 
  
  if ((mysocket=socket(AF_INET, SOCK_DGRAM, 0))==-1)
    cout << "Error in socket creation.\n";
  
  memset((char *) &server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(7123);
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(mysocket, (struct sockaddr *)&server, sizeof(server)) == -1)
    cout << "Error in binding.\n";
  
  char ack[]="Got all that data, thanks!";
  
  packet rcvdPacket(0,0,0,payload);
  for (i=0; i<2; i++) { // loop is redundant
    if (recvfrom(mysocket, serialized, 512, 0, (struct sockaddr *)&client, &clen)==-1)
      cout << "Failed to receive.\n";
	
    cout << "Received packet and deserialized to obtain the following: " << endl << endl;
    rcvdPacket.deserialize(serialized);	
    rcvdPacket.printContents();
  }
  
  cout << "La la la I'm still running, but will now shut down.\n";
  
  close(mysocket);
  return 0;
}
