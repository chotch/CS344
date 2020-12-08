/*Christien Hotchkiss
 * CS 344
 * OTP
 * otp_enc.c
 * This is the client program that opens and reads the argument files. Then it attempts to make a connection with otp_enc_d server. If a connection is secure, the length of message and key are sent and the acual contents are sent as  well. The encrypted file is received */
#include <fcntl.h>
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
	//printf("HERE!!\n");
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[256];
    	
	//if (argc < 3) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args
	//since command for this program must be otp_enc plaintextX keyX portX
	if (argc < 4){
		fprintf(stderr, "USAGE: %s hostname port\n", argv[0]);
		exit(0);
	}
	//file1 = plaintextX file
	//file2 = key file
	char * file1 = argv[1];
	char * file2 = argv[2];

/*	int f1 = open(file1, O_RDONLY);
	if (f1 == -1){
		printf("Failed to open file: %s\n", file1);
		exit(1);
	}

	int f2= open(file2, O_RDONLY);
	if (f2 == -1){
		printf("Failed to open file %s\n", file2);
		exit(1);
	}*/
	//open the two files.
	//and find the length of each	
	char ch;
	//read the plaintext file
	FILE *fptr = fopen(file1, "r");
	if (fptr == NULL){
		printf("Failed to open file: %s\n", file1);
		exit(0);
	}
	//initialize count for plaintext and count1 for key file
	int count = 0;
	int count1 = 0;
		
	
	//loop over file, counting each char and storing in count++
	while((ch=fgetc(fptr))!= EOF){
		count++;
	}
	int i = 0;
	//allocate memory for char array containing characters in myPlainText file
	//we do not want to include the trailing NULL character when sending to server
	char * myPlainText = malloc(count-1);
	fclose(fptr);
	//open the message file (plainTextX)
	fptr = fopen(file1, "r");
	while((ch=fgetc(fptr))!=EOF){
		if (ch != '\0')
			myPlainText[i] = ch;
		i++;
	}
	i = 0;
	fclose(fptr);

	//loop over all characters in myPlainText and check that
	//they are capital or spaces. Check for valid input to pass
	//plaintex5
	int num = 0;
	//printf("Checking for bad input\n");
	while(myPlainText[i] != '\0'){
		ch = myPlainText[i];
	//	printf("char is: %c\n", ch);
		//convert to 0 through 25. If the char is not a new line or space. Then ensure that it is between 0 (A) and 25 (Z)
		num = ch - 65;
	//	printf("num is: %d\n", num);
		if (myPlainText[i] != ' ' && myPlainText[i] != '\n'){
			if (num < 0 || num > 25){
				fprintf(stderr, "otp_enc error: input contains bad characters\n");
				exit(1);
			}
			
		}

		i++;
	}

	i = 0;
//	printf("Number of characters in the file is %d\n", count);

	//read the key file
	fptr = fopen(file2, "r");
	if (fptr == NULL){
		printf("File cannot be reached\n");
		exit(0);
	}
	//loop over file, counting each character
	while((ch=fgetc(fptr))!= EOF){
		count1++;
	}
	fclose(fptr);
	fptr = fopen(file2, "r");
	//populate the myKey array and alloate memory for it
	//we do not need to include the trailing NULL character
	char * myKey = malloc(count1-1);
	while((ch=fgetc(fptr))!=EOF){
		if (ch != '\0')
			myKey[i] = ch;
		i++;
	}
	fclose(fptr);
//	printf("Characters in key: %d\nCharacters in plainText: %d\n", count1, count);
//	printf("Number of characters in the file is %d\n", count);

	//length of key must be >= length of message. Print error message
	//if characters in plaintext is greater than chars in key
	if (count1 < count){
		fprintf(stderr, "Error: key '%s' is too short\n", file2);
		exit(1);
	}
	
	//read key and plaintextX files into character arrays
	//char * myKey = malloc(count1);
	//char * myPlainText = malloc(count);	
	

	//read from the file and store in these arrays
	


	
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

	///here
/*
	// Get input message from user
	printf("CLIENT: Enter text to send to the server, and then hit enter: ");
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
	fgets(buffer, sizeof(buffer) - 1, stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
	buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds

	// Send message to server
	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
*/
	//initialize character array for the encrypted message that will be sent by server
	char * receivedMessage = malloc(count-1);
	for (i = 0; i<count-1; i++)
		receivedMessage[i] = '\0';
	//1 receive server type
	//to ensure that we are correctly connected to the encryptin daemon
	char serv[8];
	int totalReceived = 0;
	int lengthReceived = 0;
	char * tmpBuffer;
//	charsWritten = 0;
//	receive server type from server 
	charsWritten = recv(socketFD, serv, 8, 0);
	
	if (charsWritten < 0) perror("Error reading from socket");
	//if the received data is Encrypt - this is what we want
	if (strcmp(serv, "Encrypt") == 0){
		
	//2 send message length to otp_enc_d
	//	printf("Sending message length: %d\n", count);
		charsWritten = send(socketFD, &count , sizeof(int), 0);
		if (charsWritten < 0) error("ERROR could not send length of PlainText file to socket");
		
	// send key length to otp_enc_d
	//	printf("Sending key length: %d\n", count1);
		charsWritten = send(socketFD, &count1, sizeof(int), 0);
		if (charsWritten < 0) error("ERROR could not send length of key file to socket");
		
	//3 send message (text file) to otp_enc_d
		//printf("Sending message with %d characters. Length of message according to variable = %d\n", strlen(myPlainText), count-1);
		charsWritten = send(socketFD, myPlainText, count-1, 0);
		if (charsWritten < 0) error("ERROR could not send PlainText file to socket");
		
	//4 send key to otp_enc_d
	//	printf("Sending key: %s\n", myKey);
		charsWritten = send(socketFD, myKey, count1-1, 0);
		if (charsWritten < 0) error("ERROR could not send key file to socket");

	//5 receive ciphertext
//		printf("%s\n", receivedMessage);
//		initialize variables and tmp buffer for while loop to receive
		totalReceived = 0;
		lengthReceived = count-1;
		tmpBuffer = receivedMessage;
		//loop whlie the total number of bytes received is less than the expected
		while(totalReceived < lengthReceived){
			//receive parts of message
			charsWritten = recv(socketFD, receivedMessage, lengthReceived-totalReceived, 0);
			if (charsWritten < 0) error("ERROR reading from socket");
			totalReceived = totalReceived + charsWritten;
			//increment receivedMessage(pointer) to the next char that needs to be saved and written to receivedMessage
			if (totalReceived < lengthReceived)
				receivedMessage = receivedMessage + charsWritten;
		}
		//reset receivedMessage its [0] position
		receivedMessage = tmpBuffer;
		//charsWritten = recv(socketFD, receivedMessage, count-1, 0);
		//if (charsWritten < 0) perror("Error reading from socket");
//		printf("%d\n", count-1);
//		printf("%s\n", receivedMessage);
		//comment out following line for testing plaintext4
		printf("%s\n", receivedMessage);	
//		printf("This is the returned message from encrpyt: %s\n", receivedMessage);
	}
	//return error if not connected to Encrypt server
	else{
		fprintf(stderr, "Error: could not contact otp_enc_d on port %d\n", portNumber);
		 exit(2);
	}

	//free allocated memory and close connection
	free(myPlainText);
	free(myKey);
	free(receivedMessage);
	//close(keyFD);
	close(socketFD);
	
	return 0;
	
	
}
