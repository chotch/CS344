/*Christien Hotchkiss
 * CS 344
 * OTP
 * otp_dec_d.c
 * This is the daemon for the decryption program. It listens for oncoming connections with otp_dec. It reads in the length of key and message as well as the character arrays key and message. Then, it uses modulo % 27 decryption to decrypt a cipherText and outputs the result back to otp_enc by sending it */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	//most of this taken from code given to us in lecture
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[256];
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	// Accept a connection, blocking if one is not available until one connects
	sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect


	//create infinite loop to accept oncoming connections. Blocks until one is able to connect
	while (1){
		//initialize variables
		int lengthOfMessage = 0;
		int lengthOfKey = 0;
		//character arrays to store received/sent data
		char * myKey;
		char * myMessage;
		char * newMessage;
		char cipher;
		int i;			
		int characterValue;
		int characterValue2;
		int sum =0 ;
		//the next three variables are useful when looping over recv calls
		//this is only absolutely necessary for plaintext4, but since it
		//still works for the smaller files, I used this method for all of them.
		//it also makes the code simpler
		int lengthReceived = 0;
		int totalReceived = 0;
		char * tmpBuffer;

		//create a connection
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) error("ERROR on accept");

		if (establishedConnectionFD >= 0){
			//use key and message. need to fork the process
			pid_t spawnPid = fork();
			if (spawnPid<0){
				perror("Hull Breach!");
				exit(1);
			}
				//if child process
				if(spawnPid==0){
			
					//send the message Decrpy. This ensures that otp_dec cannot connect to port associated with otp_dec_d and vice versa		
					charsRead = send(establishedConnectionFD, "Decrypt", 8, 0);	
					if (charsRead < 0) error("ERROR writing to socket");
					//recveive the length of the message otp_dec
					//store it in variable length of message
					charsRead = recv(establishedConnectionFD, &lengthOfMessage, sizeof(int), 0);
					//allocate memory from myMessage character pointer
					myMessage = malloc(lengthOfMessage);
	
					//receive the legnth of the key sent by otp_dec and store it				
					charsRead = recv(establishedConnectionFD, &lengthOfKey, sizeof(int), 0);
					
					//allocate memory for myKey and newMessage.
					//newMessage is the same length as the received message
					//newMessage will store the encryption (ciphertext file)
					myKey = malloc(lengthOfKey);
					newMessage = malloc(lengthOfMessage);
					lengthOfKey = lengthOfMessage;
					//initialize the character arrays to null
					for(i = 0; i<lengthOfMessage; i++){
						myMessage[i] = '\0';
						//myKey[i] = '\0';
						newMessage[i] = '\0';
					}
					for (i = 0; i< lengthOfKey; i++)
						myKey[i] = '\0';
	
					//printf("Decrpyt received message with %d characters. Length of message variable = %d\n", strlen(myMessage), lengthOfMessage-1);	
					
					//reset characters received to 0 and set the amount we want to receive to the lengthOfMessage - 1 (we dont need NULL char)
					totalReceived = 0;
					lengthReceived = lengthOfMessage-1;
					//temporary pointer to the beginning of myMessage
					tmpBuffer = myMessage;
					//loop while we have not received as many characters as expected
					while(totalReceived < lengthReceived){
						//receive the message from otp_dec
						charsRead = recv(establishedConnectionFD, myMessage, lengthReceived-totalReceived, 0);
						if (charsRead < 0) error("ERROR reading from socket");
						//sum the ammount of characters received
						totalReceived = totalReceived + charsRead;
						//if we still haven't received all the characters
						//use pointer arithmetic on myMessage to increment where next data gets stored at
						if (totalReceived < lengthReceived)
							myMessage = myMessage + charsRead;
					}
					//restore mymessage pointer to its beginning
					myMessage = tmpBuffer;
				//	charsRead = recv(establishedConnectionFD, myMessage, lengthOfMessage-1, 0);
				//	if (charsRead < 0) error("ERROR reading from socket");
					
					/*The next block of code is idnetical to the one above it
 * It l	oops over a recv instruction to ensure this program recevies everything that otp_dec sent. This is only neeeded for plaintext4. This loop populates the myKey character array*/
					totalReceived = 0;
					lengthReceived = lengthOfKey-1;
					tmpBuffer = myKey;
					while(totalReceived < lengthReceived){
						charsRead = recv(establishedConnectionFD, myKey, lengthReceived-totalReceived, 0);
						if (charsRead < 0) error("ERROR reading from socket");
						totalReceived = totalReceived + charsRead;
						if (totalReceived < lengthReceived)
							myKey = myKey + charsRead;
					}
					myKey = tmpBuffer;
	
//					charsRead = recv(establishedConnectionFD, myKey, lengthOfKey-1, 0);
//					if (charsRead < 0) error("ERROR reading from sokcet");
		
					//do the decryption
					//loop over each character in key
					for (i = 0; i<lengthOfKey; i++){
						//if message is space, the offset is only 6 because space in ascii is 32 - 6 = 26 which is one of possible random generations in mod 27
						if (myMessage[i] == ' ')
							characterValue = myMessage[i] - 6;
						else
							characterValue = myMessage[i] - 65;
						//if key value is space, only need to subtract 6 to generate correct offset
						if (myKey[i] == ' ')
							characterValue2 = myKey[i] - 6;
						else
							characterValue2 = myKey[i] - 65;
						//begin decrpytion
						sum = characterValue - characterValue2;
						//add 27 if sum is negative
						if (sum < 0)
							sum = sum + 27;
						//mod 27 and then determine the cipher character and populate newMessage with this character
						sum = sum % 27;
						if (sum == 26)
							cipher = sum + 6;
						else
							cipher = sum + 65;
						newMessage[i] = cipher;
					}
					//free memory on the heap
					free(myMessage);
					free(myKey);
					
					//send decryption to otp_dec
					charsRead = send(establishedConnectionFD, newMessage, lengthOfKey, 0);
					if (charsRead < 0) error("ERROR writing to scoket");
					free(newMessage);
					close(establishedConnectionFD); //close existing socket which is connected to the client
					close(listenSocketFD); //close listening socket
					
					exit(0);
				}	
				///default:
				//	break;	
				

		}
	}
	return 0;
}
