#include <stdio.h>
//#include <conio.h>
#include <stdlib.h>

int main(){
/*	printf("Hello");
	int lengthOfKey = 22;
	char * cipherText = malloc(lengthOfKey);
	char * myMessage = "HELLO WORLD I LOVE YOU";
	char * myKey = "XMCKLW EWTE BCDEFDS IJ";
	int sum = 0;
	char * newMessage = malloc(lengthOfKey);
	int characterValue=0;
	int characterValue2=0;
	int i;
	char cipher;
	printf("myMessage: %s\n", myMessage);
	printf("myKey: %s\n", myKey);
	for (i = 0; i<lengthOfKey; i++){
		if (myMessage[i] == ' ')
			characterValue = myMessage[i] - 6;
		else
			characterValue = myMessage[i] - 65;
		if (myKey[i] == ' ')
			characterValue2 = myKey[i] - 6;
		else
			characterValue2 = myKey[i] - 65;
		printf("characterValue :%d, characterValue2: %d\n", characterValue, characterValue2);
		sum = characterValue + characterValue2;
		sum = sum % 27;
		if (sum == 26)
			cipher = sum + 6;
		else
			cipher = sum + 65;
		printf("sum: %d\n", sum);
		printf("cipher: %c\n", cipher);
		cipherText[i] = cipher;		
		
	}
	printf("cipherText: %s\n", cipherText);



	//now decrypt
	for (i = 0; i<lengthOfKey; i++){
		if (cipherText[i] == ' ')
			characterValue = cipherText[i] - 6;
		else
			characterValue = cipherText[i] - 65;
		if (myKey[i] == ' ')
			characterValue2 = myKey[i] - 6;
		else
			characterValue2 = myKey[i] - 65;

		sum = characterValue - characterValue2;
		if (sum < 0)
			sum = sum + 27;
		sum = sum % 27;
		if (sum == 26)
			cipher = sum + 6;
		else
			cipher = sum + 65;
		newMessage[i] = cipher;

	}
	printf("newMessage: %s\n", newMessage);*/
	//this counts the number of characters in file (including newline)
	char ch;
	FILE *fptr = fopen("mykey", "r");
	if (fptr == NULL){
		printf("File cannot be reached\n");
		exit(0);
	}
	int count = 0;
	while((ch=fgetc(fptr))!= EOF){
		count++;
	}
	fclose(fptr);
	printf("Number of characters in the file is %d\n", count);
	return 1;
}
