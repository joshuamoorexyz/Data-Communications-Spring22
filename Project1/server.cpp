//Authors: Joshua Moore and Rojal 
//Josh NetID: jjm702
//Rojal NetID:
//Compiler used: Vscode with g++

//--------sources-----------
// c++ files https://www.cplusplus.com/reference/cstdio/FILE/
// Slides from class and example given


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
#include <ctype.h> //for toupper


using namespace std;





int main(int argc, char const *argv[])
{




	//----------------------handshake phase---------------------------------------------------



	//create socket for initial handshake
	int server = 0;
	if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0)	
	{
		cout<<"Error creating socket\n";
		return -1;
	}

	struct sockaddr_in serveraddress;
	memset((char*)&serveraddress, 0, sizeof(serveraddress));	
	serveraddress.sin_family = AF_INET; //ipv4
	serveraddress.sin_port = htons(atoi(argv[1]));	//take command line argument and transfer to int
	serveraddress.sin_addr.s_addr = htons(INADDR_ANY);
	//bind to this socket
	if ((bind(server, (struct sockaddr*)&serveraddress, sizeof(serveraddress))) <0)
	{
		cout<<"Error binding to address\n";
		return -1;}

	if((listen(server, 6)) < 0)
	{
		cout<<"Error listening on port: ";
		cout<<argv[1];
		cout<<"\n";
		return -1;
	}


	struct sockaddr_in clientaddress;
	socklen_t clen = sizeof(clientaddress);
	int sock1 = ( accept(server, (struct sockaddr*)&clientaddress, &clen));



	char package[256];	
	memset(package,0,256);
	if((read(sock1,package,5)) < 0)
	{
		cout<<"unable to get data";
		return -1;
	}

	//generate random number in range for port num
	srand((unsigned)time(0)); 
    int randNo = (rand()%65536)+1024;



	char r_port[256];
	memset(r_port,0,256);


	//convert to int
	sprintf(r_port,"%d",randNo);
    cout<< "random port chosen: "<<r_port<<endl;


	 //share with client

    if ((send(sock1, r_port, 256, 0)) < 0)
	{
		cout<<"Could not transfer to client";
		return -1;
	}
	//close sockets
	close(server);
	close(sock1);





	//----------------------transfer phase---------------------------------------------------

	//create socket for transfer phase
	int sock = 0;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		cout<<"Error creating socket\n";
		return -1;
	}
	serveraddress.sin_port = htons(atoi(r_port)); //port is used here for transfer

	//bind to socket
	if ((bind(sock, (struct sockaddr*)&serveraddress, sizeof(serveraddress))) <0)
	{
		cout<<"Unable to bind to socket\n";
		return -1;
	}


	char payload[4];
	//recieve file from client in chunks of 4 characters
	for(int i=0;i<14;i++)
	{
		if((recvfrom(sock,payload, 4, 0, (struct sockaddr*)&clientaddress, &clen))<0)
		{
			
			return -1;
		}
		
		
			char data [4];


		//create file to write to
		FILE *output;
		
		strcpy(data, payload);
		output = fopen("upload.txt", "w");
		
			fwrite(data, 4, sizeof(data), output);
      
		for (int i=0; i< 10; i++)
		{
			payload[i] = toupper(data[i]);
		}
		
		if((sendto(sock, payload, 4,0, (struct sockaddr*)&clientaddress, clen))<0)
		{
			cout<<"Aknowledge not sent";
			return -1;
		}
		
		fclose(output);
		
	}
	
close(sock);

}
