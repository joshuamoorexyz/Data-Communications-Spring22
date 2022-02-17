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
#include <netdb.h> 
#include <iostream>
#include <fstream>
#include <arpa/inet.h>   // if you want to use inet_addr() function
#include <string.h>
#include <unistd.h>
#include <fstream> // for files
#include <chrono>
#include <thread>
#include <ctime>


using namespace std;
  



//start of main function
int main(int argc, char const *argv[])
{
	
	
	//----------------------handshake phase---------------------------------------------------


	struct hostent *s;

	//get hostname
	s = gethostbyname(argv[1]); //take command line argument for host address

	//create socket for handshake phase
	struct sockaddr_in server;
	memset((char*)&server, 0, sizeof(server));
	server.sin_family = AF_INET; //ipv4
	server.sin_port = htons(atoi(argv[2])); //take command line argument and convert to int
	bcopy((char*)s->h_addr, (char*)&server.sin_addr.s_addr, s->h_length);
	socklen_t slen = sizeof(server);
	int sock = 0;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		cout<<"Error creating socket";
		return -1;
	}


//bind to socket

 bind(sock, (struct sockaddr *)&server, sizeof(server));

 //connect to socket

	if ((connect(sock, (struct sockaddr*)&server, slen)) == -1)
	{
		
		return -1;
	}


//payload to initiate transfer
	char payload[256]="1248";
	memset(payload, 0, 256);
	sprintf(payload, "%d", 259);

	if ((send(sock, payload, 256, 0))<0)
	{
		cout<<"Error sending data";
		return -1;
	}

//get random port from the server
	char randPort[256];
	memset(randPort, 0, 256);
	int sock1 = 0;
	sock1 = socket(AF_INET, SOCK_STREAM, 0);
	int bytesRead = read(sock, randPort, 5);
	close(sock);





	//----------------------transfer phase---------------------------------------------------
	//needed data items
	FILE *data;
	char *buf;
	long dataLength;
	size_t result;

	//take filename from command line entry
	const char *myFile = argv[3];
	int sock2 = 0;
	if ((sock2 = socket(AF_INET, SOCK_DGRAM, 0)) <0)
	{
		cout<<"Error creating data socket";
		return -1;
	}



	//change port to one aggreed upon
	server.sin_port = htons(atoi(randPort));



	
	data = fopen(myFile, "r");

	//find end of file
	fseek(data, 0, SEEK_END);
	long lSize = ftell(data);
	rewind(data);

	


	for (int i = 0; i<14; i++)
	{
		
		buf = (char*)malloc(4);
		memset(buf, 0, 5);
		//read data from file
		dataLength = fread(buf, 1, 4, data);

	

		if ((sendto(sock2, buf, 4, 0, (struct sockaddr*)&server, slen))<0)
		{
			cout<<"Could not send data";
			return -1;
		}

		char acknowl[4];
		memset(acknowl, 0, 8);
		if ((recvfrom(sock2, acknowl, 4, 0, (struct sockaddr*)&server, &slen))<0)
		{
			cout<<"Error in aknowledge";
			return -1;
		}
		cout << acknowl << endl;
	}
	
	//close files and sockets
	fclose(data);
	close(sock2);
}

