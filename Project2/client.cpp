
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
int main(int argc, char *argv[])
{
	int type = 0;	  			//packet type start off with 0 then 1 and so on
	int length = 30;			//packet length int
	int seqnum = -1;  			//packet seqnum
	int nextSeqNum = 1; 		//next sequence number
	int base = 0;				//base int
	int N = 7;					//window size int
	char data[8][31]; 			//char array to hold data

	char dataPacket[42];						  
	memset(dataPacket, '\0', sizeof(dataPacket)); 
	
	 bool EOFile = false; 

	int outstandingPacket = 0;	   //no of outstanding packet at the given time
	int receivedData[42];

		//timer
	struct timeval timer;
	timer.tv_sec = 2;
	timer.tv_usec = 0;
	


	//get the right number of inputs from the user or end program execution
	if (argc != 5)
	{
		cout<<"Incorrect number of args..exiting ...\n";
		return 1;
	}



	//get host and convert to ip by using gethostbyname function
		// declare and setup server
	struct hostent *em_host;            // pointer to a structure of type hostent
	em_host = gethostbyname(argv[1]);   // host name for emulator
	if(em_host == NULL){                // failed to obtain server's name
		cout << "Failed to obtain server.\n";
		exit(EXIT_FAILURE);
	}


	//take receiving port number and convert to int
	int portNo = atoi(argv[2]);

	//check to see if in correct range
	if(portNo < 1024 || portNo > 65355){
		cout << "Port Error!!!! Must be between 1024 and 65355!!" << endl;
		exit(1);
	}

  // ******************************************************************
  // ******************************************************************

     // client sets up datagram socket for sending
  int CESocket = socket(AF_INET, SOCK_DGRAM, 0);  
  if(CESocket < 0){
  	cout << "Error: failed to open datagram socket.\n";
  }

// set up the sockaddr_in structure for sending
  struct sockaddr_in CE; 
  socklen_t CE_length = sizeof(CE);
  bzero(&CE, sizeof(CE)); 
  CE.sin_family = AF_INET;	
  bcopy((char *)em_host->h_addr, (char*)&CE.sin_addr.s_addr, em_host->h_length);  // both using localhost so this is fine
  char * end;
  int em_rec_port = strtol(argv[2], &end, 10);  // get emulator's receiving port and convert to int
  CE.sin_port = htons(em_rec_port);             // set to emulator's receiving port


  // client sets up datagram socket for receiving
  int ECSocket = socket(AF_INET, SOCK_DGRAM, 0);  
  if(ECSocket < 0){
  	cout << "Error: failed to open datagram socket.\n";
  }

  // set up the sockaddr_in structure for receiving
  struct sockaddr_in EC; 
  socklen_t EC_length = sizeof(EC);
  bzero(&EC, sizeof(EC)); 
  EC.sin_family = AF_INET;	
  EC.sin_addr.s_addr = htonl(INADDR_ANY);	
  char * end2;
  int cl_rec_port = strtol(argv[3], &end2, 10);  // client's receiving port and convert to int
  EC.sin_port = htons(cl_rec_port);             // set to emulator's receiving port

  // do the binding
  if (bind(ECSocket, (struct sockaddr *)&EC, EC_length) == -1){
  		cout << "Error in binding.\n";
  } 
  
 // ******************************************************************
  // ******************************************************************
	//file descriptors for select
	fd_set fileDescriptors;
	FD_ZERO(&fileDescriptors);
	FD_SET(ECSocket, &fileDescriptors);


   

	//open file to read from

	FILE *file;					
	file = fopen(argv[4], "r"); 
	//check FILE *
	if (file == NULL)
	{
		cout << "Error opening file\n";
		return 1;
	}


	//log files to write to
	ofstream seqfile; 
	ofstream ackfile;	

	//now open the files
	seqfile.open("clientseqnum.log"); 
	ackfile.open("clientack.log");		

	//check to see if ofstream opened files did so correctly 
	if (!seqfile.is_open())
	{
		cout << "Error opening file\n";
		return 1;
	}
	if (!ackfile.is_open())
	{
		cout << "Error opening file\n";
		return 1;
	}

	


 // ******************************************************************
  // ******************************************************************

	//set all elements to zero in the array
	for (int j = 0; j < 8; j++)
	{
		//memeset call for each element setting 0
		memset(data[j], '\0', sizeof(data[j]));
	}



	//creating packet
	packet pack = packet(0, 0, 0, NULL);


 // ******************************************************************
  // ******************************************************************
//encapsulation
  //start loop
	while (1)
	{
		
		//if EOF and oustanding pack
		if (EOFile && outstandingPacket == 0)
		{
			
			seqnum++;
			seqnum = seqnum % 8;
			//type equals 3 when making pack
			pack = packet(3, seqnum, 0, NULL); 
			pack.serialize(dataPacket);		   
			sendto(CESocket, dataPacket, sizeof(dataPacket), 0, (struct sockaddr *)&CE, sizeof(CE));
			//push seqnumber into file and put new line
			seqfile << pack.getSeqNum() << "\n";
		}

		//while there are less than 7 outstanding packets and EOT has not been reached do this
		while (outstandingPacket < 7 && !EOFile)
		{
			
			//setup for data packet type which is type 1
			seqnum++;
			seqnum = seqnum % 8;
			type = 1; 


			//setting datapacket == 0
			memset(&dataPacket, 0, sizeof(dataPacket));

			//read up to 30 characters from file and put into the data array
			int readCount = fread(data[seqnum], 1, length, file);
			if (readCount == length)
			{	

				//make the pack that will have up to 30 chars from the file and send to emulator
				pack = packet(type, seqnum, length, data[seqnum]);
				pack.serialize(dataPacket);	//serializing a packet
				sendto(CESocket, dataPacket, sizeof(dataPacket), 0, (struct sockaddr *)&CE, sizeof(CE));
				

				//add the outstanding packet and set the next seq number; ie if 0 it will be 1 from the math below
				outstandingPacket = (outstandingPacket + 1) % 8;
				nextSeqNum = (seqnum + 1) % 8;
				//write sequence number to file with a new line added
				seqfile << pack.getSeqNum() << "\n";
			}
			else
			{
				//set data array == 0 for all elements
				data[seqnum][readCount] = '\0';

				//seqnum will increment before and allow us to send last packet
				pack = packet(type, seqnum, readCount, data[seqnum]);
				pack.serialize(dataPacket);
				sendto(CESocket, dataPacket, sizeof(dataPacket), 0, (struct sockaddr *)&CE, sizeof(CE));
			

				//writing to seqfile with newline
				seqfile << pack.getSeqNum() << "\n";

				//same as last if
				EOFile = true;
				nextSeqNum = (seqnum + 1) % 8;
				outstandingPacket = (outstandingPacket + 1) % 8;
				break;
			}
		}

		

  // ******************************************************************
  // ******************************************************************
		//de encapsulation point 

		//setup timer
		int m = ECSocket;
		int setTimer = select(m + 1, &fileDescriptors, NULL, NULL, &timer);
		if (setTimer > 0)
		{	

			//receiving data
			int c = recvfrom(ECSocket, receivedData, sizeof(receivedData), 0, (struct sockaddr *)&EC, &EC_length);
			if (c < 0)
			{

				perror("recvfrom\n");
			}

			//packing data
			packet pack(0, 0, 0, NULL);
			pack.deserialize((char *)receivedData);



			//if EOT then ...
			if (pack.getType() == 2)
			{
				ackfile << pack.getSeqNum() << endl;
				break;
			}
			else
			{	
				//get the sequence number of the packet
				int ackseq = pack.getSeqNum();

				//if else with sequence number to determine what step to take

				if (ackseq >= base || (ackseq < (base + N) % 8 && ackseq >= 0))
				{
					base = (pack.getSeqNum() + 1) % 8;

					if (nextSeqNum > pack.getSeqNum())
					{
						outstandingPacket = (nextSeqNum - base) % 8;
					}
					else
					{
						outstandingPacket = (N - (abs(nextSeqNum - ackseq))) % 8;
					}
				}

				//stream seqnumber and a carriage return into ackfile
				ackfile << pack.getSeqNum() << endl;
			}
		}
		//error
		else if (setTimer < 0)
		{
			cout << "udp socket error has occured .. \n";
			return 1;
		}
		//timeout occured
		else
		{
			
			cout << "Resending packets due to timeout\n";


			//resend

			for (int i = (seqnum - 6) % 8; i < seqnum + 1; ++i)
			{
				//if there is data in the element create packet to send to emulator
				if (data[i] != NULL)
				{
					pack = packet(type, i, length, data[i]);
					pack.serialize(dataPacket);

					//send to emulator

					sendto(CESocket, dataPacket, sizeof(dataPacket), 0, (struct sockaddr *)&EC, sizeof(EC));
					seqfile << pack.getSeqNum() << endl;
				}
			}
		}
	}

	//closing the files and sockets
	seqfile.close();
	ackfile.close();
	close(ECSocket);
	close(CESocket);
}