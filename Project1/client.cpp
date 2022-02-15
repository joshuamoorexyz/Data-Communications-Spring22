//Authors: Joshua Moore and Rojal 
//Program Description: 
//Compiler used: Vscode with g++


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

//file stream for data transfer
// ofstream myfile;




using namespace std;


int main(int argc, char *argv[]){
  
  struct hostent *s; 
 
  //take command line argument 1 for server address
  s = gethostbyname(argv[1]);
  
  struct sockaddr_in server;
  int mysocket = 0;
  socklen_t slen = sizeof(server);
  char payload[512]="1248"; 
 
  
  //create UDP socket to the server
  if ((mysocket=socket(AF_INET, SOCK_DGRAM, 0))==-1)
    cout << "Error in creating socket.\n";

  
  memset((char *) &server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(7123);
  bcopy((char *)s->h_addr, 
	(char *)&server.sin_addr.s_addr,
	s->h_length);
 
  // obtained from https://stackoverflow.com/questions/32737083/extracting-ip-data-using-gethostbyname                                         
  struct in_addr a;
  printf("name: %s\n", s->h_name);
  while (*s->h_aliases)
      printf("alias: %s\n", *s->h_aliases++);
  while (*s->h_addr_list)
  {
      bcopy(*s->h_addr_list++, (char *) &a, sizeof(a));
      printf("address: %s\n", inet_ntoa(a));
  }

  

    
    if (sendto(mysocket, payload, 8, 0, (struct sockaddr *)&server, slen)==-1)
      cout << "Error in sendto function.\n";
  

  char portnum[512];
   recvfrom(mysocket, portnum, 512, 0, (struct sockaddr *)&server, &slen); 
 




  
 


  close(mysocket);







//file transfer phase

string filename = argv[2];
//setup file stream
ofstream filename;
//open the file
filename.open(filename);

//parse for newline character in file



struct sockaddr_in server;
  int mysocket = 0;
  socklen_t slen = sizeof(server);


  string line;
  ifstream myfile ("in.txt");
  if (myfile.is_open())
  { int i=0;
    while ( getline (myfile,line) )
    
    {
      line=payload+i[8]
      i++;
    }
    myfile.close();
  }

  else cout << "Unable to open file"; 
   
 
  
  //create UDP socket to the server
  if ((mysocket=socket(AF_INET, SOCK_DGRAM, 0))==-1)
    cout << "Error in creating socket.\n";

  
  memset((char *) &server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(portnum);
  bcopy((char *)s->h_addr, 
	(char *)&server.sin_addr.s_addr,
	s->h_length);






//send data

      if (sendto(mysocket, payload, 8, 0, (struct sockaddr *)&server, slen)==-1)
      cout << "Error in sendto function.\n";
  
  
  char  ack[512];
  recvfrom(mysocket, ack, 512, 0, (struct sockaddr *)&server, &slen); 
  cout << ack << endl;

  //char *k = inet_ntoa(server.sin_addr);
  //printf("IP address: %s\n", k);




  close(mysocket);

  return 0;
}
