// Author: Joshua Moore and Rojal Bishwokarma
// NetID: jjm702 & rb2298
// Program for Data Communications Spring 2022
// Description: demonstrate GBN protocol


//citations: 
//Maxwell Young emulation-hookup files for majority of structure [canvas]
//Maxwell Young packet header and cpp [canvas]
//https://www.cplusplus.com/reference/fstream/ofstream/ [ofstream reference]
//https://stackoverflow.com/questions/13547721/udp-socket-set-timeout [timer reference]
//https://www.cplusplus.com/reference/cstring/memset/ [memset] 
//https://www.geeksforgeeks.org/multidimensional-arrays-c-cpp/ [2d array reference]
//https://man7.org/linux/man-pages/man2/select.2.html [select reference]



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

int main(int argc, char *argv[]){
  int type = 1;
  int seqnum = 0;
  int acktype = 0;
  int expectseq = 0;
  char receiveData[37]; // to receive from packet
  char charData[30];    // character in the packet
  char ackData[42];     // to send ack data to client

  
	// ******************************************************************
	// ******************************************************************
	
	//get the right number of inputs from the user or end program execution
	if (argc != 5)
	{
		cout<<"Incorrect number of args..exiting ...\n";
		return 1;
	}

	// ******************************************************************
	// ******************************************************************


	// sets up datagram socket for receiving from emulator
	int ESSocket = socket(AF_INET, SOCK_DGRAM, 0);  
	if(ESSocket < 0){
		cout << "Error: failed to open datagram socket.\n";
	}

	// set up the sockaddr_in structure for receiving
	struct sockaddr_in ES; 
	socklen_t ES_length = sizeof(ES);
	bzero(&ES, sizeof(ES)); 
	ES.sin_family = AF_INET;	
	ES.sin_addr.s_addr = htonl(INADDR_ANY);	
	char * end;
	int sr_rec_port = strtol(argv[2], &end, 10);  // server's receiving port and convert to int
	ES.sin_port = htons(sr_rec_port);             // set to emulator's receiving port
		
	// do the binding
	if (bind(ESSocket, (struct sockaddr *)&ES, ES_length) == -1)
		cout << "Error in binding.\n";		
		
	// ******************************************************************
	// ******************************************************************
	
	// declare and setup server
	struct hostent *em_host;            // pointer to a structure of type hostent
	em_host = gethostbyname(argv[1]);   // host name for emulator
	
	if(em_host == NULL){                // failed to obtain server's name
		cout << "Failed to obtain server.\n";
		exit(EXIT_FAILURE);
	}

	int SESocket = socket(AF_INET, SOCK_DGRAM, 0);
	if(SESocket < 0){
		cout << "Error in trying to open datagram socket.\n";
		exit(EXIT_FAILURE);
	}
		
	// setup sockaddr_in struct  
	struct sockaddr_in SE;	
	memset((char *) &SE, 0, sizeof(SE));
	SE.sin_family = AF_INET;
	bcopy((char *)em_host->h_addr, (char*)&SE.sin_addr.s_addr, em_host->h_length);
	int em_rec_port = strtol(argv[3], &end, 10);
	SE.sin_port = htons(em_rec_port);
	

	
	// ******************************************************************
	// ******************************************************************


//setup files to write to
  ofstream outfile;
  outfile.open(argv[4]); // opens a file
  if (!outfile.is_open())
  {
    cout<<"Could not open file to write to\n";
	return 1;
  }

  // ack file 
  ofstream ackFile;
  ackFile.open("arrival.log");

  if (!ackFile.is_open())
  {
     cout<<"Could not open file to write to\n";
	return 1;
  }

  packet filepacket(type, seqnum, sizeof(charData), charData);



	// ******************************************************************
	// ******************************************************************


	//start loop de encapsulation

  while (1)
  {
	//set all to 0
    memset(receiveData, 0, sizeof(receiveData));
    memset(charData, 0, sizeof(charData));
    memset(ackData, 0, sizeof(ackData));

    // error in receiving from client
    if (recvfrom(ESSocket, receiveData, sizeof(receiveData), 0, (struct sockaddr *)&ES, &ES_length) == -1)
    {
      perror("Receiving\n");
      return -1;
    }

    filepacket.deserialize((char *)receiveData); // deserialize to see sent data
    type = filepacket.getType();                 //get Type
    seqnum = filepacket.getSeqNum();
	//write to ack file
    ackFile << seqnum << "\n";
    
	cout<<endl;

  	//  if the seq num matches

    if (expectseq == seqnum) 
    {
	//  EOT
      if (filepacket.getType() == 3) 
      {
		// set acktype to 2
        acktype = 2; 
        packet ackpacket(acktype, seqnum, 0, 0);
        ackpacket.serialize(ackData); 

		//send to client via emulator
        if (sendto(SESocket, ackData, sizeof(ackData), 0, (struct sockaddr *)&SE, sizeof(SE)) == -1)
        {
          perror("error sending");
          return 1;
        }
       	cout<<endl;

        break; 
      }
		//put data into file
      outfile << filepacket.getData(); 
	  
	  
      packet ackpacket(acktype, seqnum, 0, 0); 
      ackpacket.serialize((char *)ackData);    

      //send ack to client via emulator
      if (sendto(SESocket, ackData, sizeof(ackData), 0, (struct sockaddr *)&SE, sizeof(SE)) == -1)
      {
        perror("error sending");
          return 1;
      }

	  //update seqnum
      expectseq = (expectseq + 1) % 8;
    }
    else 
    {
      packet ackpacket(acktype, expectseq, 0, 0);
      ackpacket.serialize((char *)ackData);
      sendto(SESocket, ackData, sizeof(ackData), 0, (struct sockaddr *)&SE, sizeof(SE));
     
    }
  }



  //close file and sockets


  outfile.close(); 
  close(SESocket); 
}