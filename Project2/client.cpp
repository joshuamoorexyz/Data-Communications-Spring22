// Author: Joshua Moore
// NetID: jjm702
// Author: Rojal
// NetID: 

// Client program for Data Comm Project 2.Spring 2022



#include <stdlib.h>
#include <stdio.h>
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
#include <string>



using namespace std;



int main(int argc, char *argv[]){

  int packetLen=50;
  struct hostent *emulatorName; 
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



//create socket

  struct sockaddr_in server;
  int mysocket = 0;
  socklen_t slen = sizeof(server); 
  
  if ((mysocket=socket(AF_INET, SOCK_DGRAM, 0))==-1)
    cout << "Error in creating socket.\n";
    
  memset((char *) &server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(sendToEmulator);
  bcopy((char *)emulatorName->h_addr, 
	(char *)&server.sin_addr.s_addr,
	emulatorName->h_length);
  server.sin_addr.s_addr = INADDR_ANY;



//bind socket
if ((bind(mysocket, (struct sockaddr *)&server, sizeof(server))) < 0) // if binding error occurs
  {
    perror("binding");
    return -1;
  }


//open file to read from
ofstream myFile;
myFile.open(fileName);
if(myFile.is_open()){
  perror("open error");
}

//file for acks
ofstream ackFile;
ackFile.open("arrival.log");
if(ackFile.is_open()){
  perror("open error ackfile");

}


char receiveData[37]; // to receive from packet
  char charData[30];    // character in the packet
  char ackData[42];     // to send ack data to client

  int type = 1;
  int seqnum = 0;
  int acktype = 0;
  int expectseq = 0;

  packet filepacket(type, seqnum, sizeof(charData), charData);

  while (1)
  {
    memset(receiveData, 0, sizeof(receiveData));
    memset(charData, 0, sizeof(charData));
    memset(ackData, 0, sizeof(ackData));

    // error in receiving from client
    if (recvfrom(mysocket, receiveData, sizeof(receiveData), 0, (struct sockaddr *)&server, &slen) == -1)
    {
      perror("Receiving\n");
      return -1;
    }

    filepacket.deserialize((char *)receiveData); // deserialize to see sent data
    type = filepacket.getType();                 //get Type
    seqnum = filepacket.getSeqNum();

    ackFile << seqnum << "\n";
    printf("\n--------------------------------------\n");
    filepacket.printContents();

    printf("\nExpecting Rn: %d\n", expectseq);
    printf("sn: %d\n", seqnum);

    if (expectseq == seqnum) //  if expected sequence matches sequence number from client file
    {

      if (filepacket.getType() == 3) // if type sent by client is 3 then do EOT
      {

        acktype = 2; // set acktype to 2
        packet ackpacket(acktype, seqnum, 0, 0);
        ackpacket.serialize(ackData); //searialize to send data
        if (sendto(mysocket, ackData, sizeof(ackData), 0, (struct sockaddr *)&sendToEmulator, sizeof(sendToEmulator)) == -1)
        {
          perror("Sending Error");
          return -1;
        }
        ackpacket.printContents(); //print sent data
        printf("--------------------------------------\n");
        break; // exit
      }

      myFile << filepacket.getData(); // write to the file

      packet ackpacket(acktype, seqnum, 0, 0); // send only acktype and seq number
      ackpacket.serialize((char *)ackData);    // searilize to send data in the socket
      //send ack to server
      if (sendto(mysocket, ackData, sizeof(ackData), 0, (struct sockaddr *)&sendToEmulator, sizeof(sendToEmulator)) == -1)
      {
        printf("Sending Error\n");
        return -1;
      }

      ackpacket.printContents();
      expectseq = (expectseq + 1) % 8;
    }
    else // if sequence number doesnot resend the ack to the server
    {
      packet ackpacket(acktype, expectseq, 0, 0);
      ackpacket.serialize((char *)ackData);
      sendto(mysocket, ackData, sizeof(ackData), 0, (struct sockaddr *)&sendToEmulator, sizeof(sendToEmulator));
      ackpacket.printContents();
      printf("-------------------------------------\n");
    }
  }

  myFile.close(); //close the file

  close(mysocket); // close the socket

}


  	  
//   // now going to send serialize and send 3 packets
//   char payloadA[512]="123"; // payload data

//   char payloadB[512]="abcd"; // another payload

  
//   char spacketA[packetLen];  // for holding serialized packet  
//   memset(spacketA, 0, packetLen); // serialize the packet to be sent

//   char spacketB[packetLen];
//   memset(spacketB, 0, packetLen);

//   char spacketC[packetLen];
//   memset(spacketC, 0, packetLen);
  
//   packet mySendPacket(1, 101, strlen(payloadA), payloadA); // make the packet to be serialized and sent

//   packet mySendPacketB(1, 201, strlen(payloadB), payloadB);
  
//   mySendPacket.serialize(spacketA); // serialize so spacket contains serialized contents of mySendPacket's payload
//   cout << "Sending serialized packet with payload:" << payloadA << endl;
//   cout << "This is what the serialized packet looks like: " << spacketA << endl;
//   sendto(mysocket, spacketA, 50, 0, (struct sockaddr *)&server, slen);

//   mySendPacketB.serialize(spacketB);
//   cout << "Sending serialized packet with payloadB:" << payloadB << endl;
//   cout << "This is what the serialized packet looks like: " << spacketB << endl;
//   sendto(mysocket, spacketB, 50, 0, (struct sockaddr *)&server, slen);



// //close file
// myFile.close();
//   //close socket
//   close(mysocket);
//   return 0;
// }
