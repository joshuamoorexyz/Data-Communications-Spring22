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
int bufsize;


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



//taken from https://www.cplusplus.com/reference/cstdio/fread/
//open file to read from (These files should be overwritten every runtime)
 FILE * pFile;
  long lSize;
  char * buffer;
  size_t result;

  pFile = fopen ( fileName , "rb" );
  if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

  // obtain file size:
  fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  bufsize=lSize;
  rewind (pFile);

  // allocate memory to contain the whole file:
  buffer = (char*) malloc (sizeof(char)*lSize);


  if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

  // copy the file into the buffer:
  result = fread (buffer,1,lSize,pFile);
  if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

  /* the whole file is now loaded in the memory buffer. */

 
//format for two logs files is one number per line TA will be using this to grade


 FILE * logfile1;
 FILE * logfile2;

 FILE * seqFile;
  long seqSize;
  char * seqbuffer;
  size_t seqresult;

  logfile1 = fopen ( "clientseqnum.log" , "rb" );
  logfile2=fopen("clientack.log","rb");
  

/*

 and send it using ARQ to the server
 https://youtu.be/raZYelgmyb0 good video about ARQ to use

*/
int totalcount=0;
//keep up with sequence either 0 or 1 to pass to packet

while(totalcount!=bufsize){
char tempbuf[30];
int loopcount=0;
for(int i=0;i<30;i++){

  if(buffer[totalcount+i]!=0){
  loopcount+=1;
  tempbuf[totalcount+i]=buffer[totalcount+i];}
  else{
    break;
  }

//check to see if clientseqnum exists if not create it and send packet with 0 seq num first
  seqFile = fopen ( "clientseqnum.log" , "rb" );
  if (seqFile==NULL) {
  

 packet p(1,0,loopcount,tempbuf);

//serialize and then send
char spacket[30];
 memset((char *) spacket, 0, bufsize);

p.serialize(spacket);


if (sendto(udp_socket, ackData, sizeof(ackData), 0, (struct sockaddr *)&emulatorSend, sizeof(emulatorSend)) == -1)
        {
          perror("Sending Error");
          return -1;
        }


//wait for ack from server


    memset(receiveData, 0, sizeof(receiveData));
    memset(charData, 0, sizeof(charData));
    memset(ackData, 0, sizeof(ackData));

    // error in receiving from client
    if (recvfrom(udp_socket, receiveData, sizeof(receiveData), 0, (struct sockaddr *)&client, &clen) == -1)
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






         
        ackpacket.printContents(); //print sent data
        printf("--------------------------------------\n");
        break; // exit
      }

      outfile << filepacket.getData(); // write to the file

      packet ackpacket(acktype, seqnum, 0, 0); // send only acktype and seq number
      ackpacket.serialize((char *)ackData);    // searilize to send data in the socket
      //send ack to server
      if (sendto(udp_socket, ackData, sizeof(ackData), 0, (struct sockaddr *)&emulatorSend, sizeof(emulatorSend)) == -1)
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
      sendto(udp_socket, ackData, sizeof(ackData), 0, (struct sockaddr *)&emulatorSend, sizeof(emulatorSend));
      ackpacket.printContents();
      printf("-------------------------------------\n");
    }
  }

  outfile.close(); //close the file




//write seq number to file



  }
  else{
    int sequencebuffer[bufsize];


    for(int i=1;i<bufsize;i++){

      if(sequencebuffer[i-1]=0){
    packet p(1,1,loopcount,tempbuf);


    //serialize and then send
p.serialize(p);

//wait for ack from server


//write seq number to file

      }
      if(sequencebuffer[i-1]=1){

 packet p(1,0,loopcount,tempbuf);



  


  }
  
  }


  totalcount+=loopcount;
}

}}

//send EOT packet to server





/*


after all contents of the file have been transmitted successfully
to the server and corresponding ACKs have been received, the client should send an
EOT packet to the server.  The EOT packet is in the same format
 (and it has a sequence number) as a regular data packet, except that its type 
 field is set to 3,its length is set to zero,and the data is set to null

 To ensure reliable transmission your program should implement ARQ like:

 The sequence numbers used will be 0 and 1.The first sequence number used is 0,
 then 1,then 0,then 1,etc.

 Once the client sends a packet,it waits for an ACK with the corresponding sequence number

 A timer of 2 seconds is used to recover from packet loss. When the timer expires,
 the client resends the outstanding packet.If there is no outstanding packet,the timer is stopped

 <heading>Interrupting the Timer</heading>

 The recvfrom() call will cause the client to block until a packet
 is received. This may cause problems from the corectness of your program unless
 you interrupt the recvfrom() call.

 For ex: imagine the case where the last packet from the client is sent and the client then calls
 recvfrom() to obtain the acknowledgement that should be returned from the server.
 However,assume that this last packet from the client is lost in transit. No acknowledgement will be 
 transmitted back from the server. The client, who is now waiting to 
 receive an ack, will block forever.

 You must interrupt the blocking call to check whether the timer has expired. It is up to you
 to decide how to do this. i recommend setsockopt() and using the option SO_RCVTIMEO

 (remember to reference source)

 **Whenever a packet is sent,its sequence number should be recorded in "clientseqnum.log" this includes the EOT packet sent 
 by the client


 The file "clientack.log" should record the sequence numbers of all the ACK packets and the EOT packet
 (from the server) that the client receives during the entire period of transmission.

 

*/

  // terminate
  fclose (pFile);
  free (buffer);

  close(mysocket); // close the socket
  return 0;
}


  	  

