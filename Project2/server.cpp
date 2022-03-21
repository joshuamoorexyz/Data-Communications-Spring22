// Author: Joshua Moore
// NetID: jjm702
// Author: Rojal
// NetID: 

// Server program for Data Comm Project 2.Spring 2022

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
#include <fstream>

using namespace std;

int main(int argc, char *argv[]){
  
//setup for writing to file

//overwrite on these files not just append

//open file to read and write to
ofstream myFile;
myFile.open(fileName);
if(myFile.is_open()){
  perror("open error");
}

//open file to generate log file.
ofstream myFile1;
myFile1.open(arrival.log);
if(myFile1.is_open()){
  perror("open error");
}


  struct sockaddr_in server;
  struct sockaddr_in client;
   int packetLen=50;
  struct hostent *emulatorName;
  int mysocket = 0;
  int i = 0;
  socklen_t clen = sizeof(client);

//get command line input from the user

//emulatorName: host address of the emulator
  emulatorName = gethostbyname(argv[1]);
  
//sendToEmulator: UDP port number used by the emulator to receive 
//data from the client
int sendToEmulator=atoi(argv[2]);
if(sendToEmulator < 1024 || sendToEmulator > 65355){
  cout<<"Invalid port number";
  cout<<endl;
  return -1;
}

//receiveFromEmulator: UDP port number used by the client to 
//recieve ACKS from the emulator
int receiveFromEmulator=atoi(argv[3]);
if(receiveFromEmulator < 1024 || receiveFromEmulator > 65355){
  cout<<"Invalid port number";
  cout<<endl;
  return -1;
}

//fileName: name of the file to be transferred in the given order
char* fileName=argv[4];



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
  


//recieve packets
  packet rcvdPacket(0,0,0,payload);
  for (i=0; i<2; i++) { // loop is redundant
    if (recvfrom(mysocket, serialized, 512, 0, (struct sockaddr *)&client, &clen)==-1)
      cout << "Failed to receive.\n";
    else{


     /*

      if packet is received from the client then

      check the sequence number of the packet

      if the sequence number is the one that it is expecting,
      it should send an ACK packet back to the client with the
      sequence number of the received packet

      In all other cases,it should discard the received packet
      and resend an ACK packet for the most recently received in
      order packet


      after the server has received all data packets and an 
      EOT from the client, it should send an EOT packet with the type
      field set to 2, and then exit.

      the server must also write the received data from the client to
      the file fileName

      write to log file,the sequence numbers of all the data
      packets and the EOT packet that the server receives during transmission


      */

   cout << "Received packet and deserialized to obtain the following: " << endl << endl;
    rcvdPacket.deserialize(serialized);	
    rcvdPacket.printContents();


      }

 

    }
	
  
  }
  
  cout << "La la la I'm still running, but will now shut down.\n";
  
  close(mysocket);
  return 0;
}
