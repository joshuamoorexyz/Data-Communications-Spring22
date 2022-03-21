// Author: Maxwell Young
// Date: Feb 1, 2016

#ifndef PACKET_H
#define PACKET_H
 
//using namespace std; 
 
class packet{
	
private:
	
    int type;       
	// 0 if an ACK
	//1 if a data packet
	//2 if EOT packet from server to client
	//3 EOT packet from client to server

	int seqnum;     
	//either 0 or 1. sequence number of the first packet should be 0
	//For ACK packets, seqnum is the sequence number of the packet being acknowledge

	int length;    
	 // number of characters carried in data field 
	 //should be in the range of 0 to 30 taking the largest chunk of data
	 //per packet sent (take 30 characters if you can,less if you have reached
	 //the end of the file) Assume ASCII text file adn that spaces,exclamation points,
	 //newline symbols,etc each count as a character.

	 //for ACK packets,length should be set to zero and carry no data

	char * data;    // remember this should be 0 for ACK packets
 
public:
	
	packet(int t, int s, int l, char * d); // constructor
	int getType();
	int getSeqNum();
	int getLength();
	char * getData();
	void printContents(); // print function for testing
	void serialize(char * s);
	void deserialize(char * s);
};
 
#endif