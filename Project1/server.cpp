//Authors: Joshua Moore and Rojal 
//Program Description: 
//Compiler used: Vscode with g++




#include <iostream>
#include <sys/types.h>   // defines types (like size_t)
#include <sys/socket.h>  // defines socket class
#include <netinet/in.h>  // defines port numbers for (internet) sockets, some address structures, and constants
#include <time.h>        // used for random number generation
#include <string.h> // using this to convert random port integer to string
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>


using namespace std;




//file stream for data transfer
ofstream myfile;



int main(int argc, char *argv[]){
  


  char portnum[8];
  struct sockaddr_in server;
  struct sockaddr_in client;
  int mysocket = 0;
  int i = 0;
  socklen_t clen = sizeof(client);
  char payload[512];
  
  if ((mysocket=socket(AF_INET, SOCK_DGRAM, 0))==-1)
    cout << "Error in socket creation.\n";
  
 


  memset((char *) &server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(7123);
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(mysocket, (struct sockaddr *)&server, sizeof(server)) == -1)
    cout << "Error in binding.\n";
  

  for (i=0; i<5; i++) {
    cout << "I'm waiting on a request for a port number.\n";
    if (recvfrom(mysocket, payload, 512, 0, (struct sockaddr *)&client, &clen)==-1)
      cout << "Failed to receive.\n";
      break;
    //generate random port number between 1024 and 65535
  /* initialize random seed: */
  

  }
srand (time(NULL));
  
  int randnum = rand() % 65535 + 1024; 
  //convert integer to char 
  sprintf(portnum, "%d", randnum);


cout<<"Random port chosen:" << portnum;

if (sendto(mysocket, portnum, 64, 0, (struct sockaddr *)&client, clen)==-1){
    cout << "Error in sendto function.\n";
  }
 close(mysocket);









  // char ack[]="Got all that data, thanks!";
  // if (sendto(mysocket, ack, 64, 0, (struct sockaddr *)&client, clen)==-1){
  //   cout << "Error in sendto function.\n";
  // }
     
  // close(mysocket);














//send data phase










  // for (i=0; i<5; i++) {
  //   cout << "I'm waiting for a packet now.\n";
  //   if (recvfrom(mysocket, payload, 512, 0, (struct sockaddr *)&client, &clen)==-1)
  //     cout << "Failed to receive.\n";
  //   cout << "Received data: " << payload << endl;
  // }
  
  // char ack[]="Got all that data, thanks!";
  // if (sendto(mysocket, ack, 64, 0, (struct sockaddr *)&client, clen)==-1){
  //   cout << "Error in sendto function.\n";
  // }
     
  // close(mysocket);
  return 0;
}
