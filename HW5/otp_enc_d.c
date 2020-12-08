
/*Christien Hotchkiss
 * CS 344
 * OTP
 * otp_enc_d.c
 * This is the daemon for the encryption program. It listens for oncoming connections with otp_enc. It reads in the length of key and message as well as the character arrays key and message. Then, it uses modulo % 27 to encrypt a plaintext file and outputs the result back to otp_enc by sending it */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

//setting up the server is code adopted for class lectures
int main(int argc, char *argv[])
{
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
		//arrays to store sent and received data
		char * myKey;
		char * myMessage;
		char * tmpBuffer;
		char * cipherText;
		char cipher;
		int i;			
		int characterValue;
		int characterValue2;
		int sum =0 ;
		//variables used in the while loop of recv statements to account for plaintext4
		int totalReceived = 0;
		int lengthReceived = 0;

establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) error("ERROR on accept");

		if (establishedConnectionFD >= 0){
			//use key and message. need to fork the process
			pid_t spawnPid = fork();
			if (spawnPid < 0){
				perror("Hull Breach!");
				exit(1);
			}
			//child process
			if (spawnPid == 0){
				//send an Encrypt message back to otp_enc. This ensures that otp_enc canot connect to otp_dec_d and vice versa
				charsRead = send(establishedConnectionFD, "Encrypt", 8, 0);	
				if (charsRead < 0) error("ERROR writing to socket");
				//receive the length of the message from otp_enc
				charsRead = recv(establishedConnectionFD, &lengthOfMessage, sizeof(int), 0);
				
				//allocate new memory for message and then receive the length of the key from otp_enc
				myMessage = malloc(lengthOfMessage);
				charsRead = recv(establishedConnectionFD, &lengthOfKey, sizeof(int), 0);
				//printf(" Received Length of Message: %d\n", lengthOfMessage);
				//printf("Received Length of Key: %d\n", lengthOfKey);
				//allocate memory for key and cipherText (which is same length as message) and initialize these arrays to NULL
				myKey = malloc(lengthOfKey);
				cipherText = malloc(lengthOfMessage);
				for(i = 0; i<lengthOfMessage; i++){
					myMessage[i] = '\0';
				//	myKey[i] = '\0';
					cipherText[i] = '\0';
				}
				for (i = 0; i<lengthOfKey; i++)
					myKey[i] = '\0';
				//lengthOfKey = lengthOfMessage;

				//use a while loop to receive information. This is mainly for plaintext4 but works for all sizes.
				totalReceived = 0;
				lengthReceived = lengthOfMessage-1;
				//necesarry to restore myMessage pointer after while loop increments it
				tmpBuffer = myMessage;
				//continue looping until characters received is equal to the expected amount (specified by lengthReceived)
				while(totalReceived < lengthReceived){
					//receive the message sent by otp_enc. This will be the text file to be encrypted
					charsRead = recv(establishedConnectionFD, myMessage, lengthReceived-totalReceived, 0);
					if (charsRead < 0) error("ERROR reading from socket");
					totalReceived = totalReceived + charsRead;					
					//increment myMesage pointer so next time through loop adds onto myMessage character aray
					if (totalReceived < lengthReceived)
						myMessage = myMessage + charsRead;
				}
				myMessage = tmpBuffer;
				//charsRead = recv(establishedConnectionFD, myMessage, lengthOfMessage-1, 0);
				//if (charsRead < 0) error("ERROR reading from soket");
				//printf("Encryption received message with %d characters. Length of message according to variable: %d\n", strlen(myMessage), lengthOfMessage-1);
				//printf("received message is: %s\n", myMessage);				
				//The following while loop is identical to the one above it.
				//Instead of populating the message array, it populates the key arary, which is sent from otp_enc.
				totalReceived = 0;
				lengthReceived = lengthOfKey - 1;
				tmpBuffer = myKey;
				while(totalReceived < lengthReceived){
					charsRead = recv(establishedConnectionFD, myKey, lengthReceived-totalReceived, 0);
					if (charsRead < 0) error("ERROR reading from socket");
					totalReceived = totalReceived + charsRead;
					if (totalReceived < lengthReceived)
						myKey = myKey + charsRead;
				}
				myKey = tmpBuffer;
//				charsRead = recv(establishedConnectionFD, myKey, lengthOfKey-1, 0);
//				if (charsRead < 0) error("ERROR reading from sokcet");
				//printf("received key is: %s\n", myKey);
				//do the encryption
				
				for (i = 0; i<lengthOfMessage; i++){
					//off set is only 6 for space because its ascii value is 32
					if (myMessage[i] == ' ')
						characterValue = myMessage[i] - 6;
					//otherwise offset is 65 because A = 65 and we are generating random numbers between 0 and 26
					else
						characterValue = myMessage[i] - 65;
					//do the same with myKey
					if (myKey[i] == ' ')
						characterValue2 = myKey[i] - 6;
					else
						characterValue2 = myKey[i] - 65;
					//printf("characterValue :%d, characterValue2: %d\n", characterValue, characterValue2);
					sum = characterValue + characterValue2;
					//mod27 and store result in cipherText
					sum = sum % 27;
					if (sum == 26)
						cipher = sum + 6;
					else
						cipher = sum + 65;
				//	printf("sum: %d\n", sum);
				//	printf("cipher: %c\n", cipher);
					cipherText[i] = cipher;		
		
				}
				//free dynamically allocated char arrays
				free(myMessage);
				free(myKey);
				
				//end the encryption			
				//printf("server is sending: %s\n", cipherText);	
				//send encrypted message otp_enc_d
				charsRead = send(establishedConnectionFD, cipherText, lengthOfMessage, 0);
				if (charsRead < 0) error("ERROR writing to scoket");
				free(cipherText);
				close(establishedConnectionFD); //close existing socket which is connected to the client
				close(listenSocketFD); //close listening socket
				
				exit(0);
						
	
			}

		}
	}
	return 0;
}
