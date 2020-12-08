/*Christien Hotchkiss
 * CS 344
 * OTP
 * otp_dec.c
 * This files acts as the client for the decrypt program. It writes the key length and message length to otp_dec_d and then also writes the message (cipherText) and key to otp_dec_d. In return it receives the decrpyted message which should be the same as the plainText file that was first encrypted */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	//code adopted from lecture
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[256];
    
	//must have 4 arguments: program textFile key port number
	if (argc < 4) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

	//file1 is ciphertext file and file2 is key file
	char * file1 = argv[1];
	char * file2 = argv[2];

	char ch;

	//open cipherTextfile
	FILE * fptr = fopen(file1, "r");
	if (fptr == NULL){
		printf("Failed to open file %s\n", file1);
		exit(0);
	}

	int cipherCount = 0;
	int keyCount = 0;

	//count the number of characters in cipherText
	while((ch=fgetc(fptr)) != EOF)
		cipherCount++;

	int i = 0;
	fclose(fptr);
	fptr = fopen(file1, "r");
	char * cipherText = malloc(cipherCount-1);
	//store the contents of cipherText into cipherText characer array
	while ((ch=fgetc(fptr))!=EOF){
		if (ch != '\0')
			cipherText[i] = ch;
		i++;
	}

	i = 0;
	fclose(fptr);
	
	//open up key file
	fptr = fopen(file2, "r");
	if (fptr == NULL){
		printf("Failed to open file %s\n", file2);
		exit(0);
	}

	//count number of characters in the keyfile
	while((ch=fgetc(fptr)) != EOF)
		keyCount++;

	i = 0;
	fclose(fptr);
	fptr = fopen(file2, "r");
	char * keyText = malloc(keyCount-1);
	//store contents of keyfile into keyText charater array
	while ((ch=fgetc(fptr))!=EOF){
		if (ch != '\0')
			keyText[i] = ch;
		i++;
	}
	fclose(fptr);
	
	//esnure that length of key is atleast equal to that of cipherText
	if (keyCount < cipherCount){
		printf("Error: key '%s' is too short\n", file2);
		exit(1);
	}



	
	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	//allocate dyanmic memor for receivedMessage ( will be length of cipherText file - 1) because we dont care about NULL character
	char * receivedMessage = malloc(cipherCount-1);
	
	//iniitalize receivedMessage to null
	for (i = 0; i<cipherCount-1; i++)
		receivedMessage[i] = '\0';

	//initialize variables
	char serv[8];
	int totalReceived;
	int lengthReceived;
	char * tmpBuffer;
	//charsWritten = 0;
	//receive server type otp_dec_d.c 
	charsWritten = recv(socketFD, serv, 8, 0);
	if (charsWritten < 0) perror("Error reading from socket");
	if (strcmp(serv, "Decrypt") == 0){

		//2 send message length to otp_dec_d
		charsWritten = send(socketFD, &cipherCount, sizeof(int), 0);
		if (charsWritten < 0) error("ERROR could not send length of PlainText file to socket");

		// send key length to otp_dec_d
		charsWritten = send(socketFD, &keyCount, sizeof(int), 0);

		if (charsWritten < 0) error("ERROR could not send length of key file to socket");

		//3 send message to otp_dec_d
		//printf("Decrypt sending message with %d characters. Length of message variable = %d\n", strlen(cipherText), cipherCount-1);
		charsWritten = send(socketFD, cipherText, cipherCount-1, 0);
		
		if (charsWritten < 0) error("ERROR could not send cipherText file to socket");
		//4 send key to otp_dec_d
		charsWritten = send(socketFD, keyText, keyCount-1, 0);

		if (charsWritten < 0) error("ERROR could not send key file to socket");
		//received decrpyted (5)
		//initialize variables for use inside while loop for recv
		totalReceived = 0;
		lengthReceived = cipherCount-1;
		tmpBuffer = receivedMessage;
		//continue to loop while received characters is less than number of expected received characters
		while(totalReceived < lengthReceived){
			//receive decrpyted message from server
			charsWritten = recv(socketFD, receivedMessage, lengthReceived-totalReceived, 0);
			if (charsWritten < 0) error("ERROR reading from socket");
			totalReceived = totalReceived + charsWritten;
			//if number of characters isn't yet received then increment pointer counter 
			if (totalReceived < lengthReceived)
				receivedMessage = receivedMessage + charsWritten;
		}
		//restore receivedMessage to point back to beginning
		receivedMessage = tmpBuffer;

		//5 receive decrypted
	//	charsWritten = recv(socketFD, receivedMessage, cipherCount-1,0);

	//	if (charsWritten < 0) perror("ERROR reading from socket");
		//printf("This is the returned message from decrpyt: %s\n", receivedMessage);
		//comment out next line for testing plaintext4
		printf("%s\n", receivedMessage);
	}
	else fprintf(stderr, "Error: could not contact otp_dec_d on port %d\n", portNumber); exit(2);
	
	//free dynamic memory allocated throughout program and close socket
	free(cipherText);
	free(keyText);
	free(receivedMessage);
	close(socketFD);

	return 0;


	// Get input message from user
/*	printf("CLIENT: Enter text to send to the server, and then hit enter: ");
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
	fgets(buffer, sizeof(buffer) - 1, stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
	buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds

	// Send message to server
	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	// Get return message from server

	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse

*/

	
}
