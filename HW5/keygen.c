/*Christien Hotchkiss
 * CS 344
 * OTP
 * Keygen.c
 * This program randomly generates a string with letters A - Z and including
 * a space. It is printed to stdout */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main(int argc, char ** argv){
	//Check for the correct command line arguments
	if (argc != 2){
		printf("When using keygen, syntax must be: keygen keyLength\n");
		exit(1);
	}

	int keyLength = 0;
	//ensure that the length is not negative
	if (atoi(argv[1]) <= 0){
		printf("keyLength must be atleast 1\n");
		exit(1);
	}
	else {
		//convert character pointer to an integer
		keyLength = atoi(argv[1]);
//	printf("key length is %d\n", keyLength);
	}

	//initialize random seed
	srand(time(NULL));
	int randNum = 0;
	int i = 0;
	char toAdd[keyLength+1];
	//loop over the amount of characters to ad
	for (i = 0; i< keyLength; i++){
		//generate a random number between 0 and 26 then add 65 to 
		//convert to ascii character values A - Z
		randNum = rand() % 27;
		toAdd[i] = (char)(randNum+65);
		if (toAdd[i] == '['){
			toAdd[i] = ' ';
		}
	}	
	//terminating null character and then print to the screen
	toAdd[keyLength] = '\0';

//	printf("\n");
	printf("%s\n", toAdd);
	return 0;

}
