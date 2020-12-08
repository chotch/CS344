
#include<stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

void fillFile(char* name){
	printf("in fillFile\n");
}

void createPopulateFiles(){
	FILE* fp = NULL;
	int i;
	int randRoom;
	int seenRooms[7] = {10, 10, 10, 10, 10, 10, 10};
	int haveSeen = 0;
	int j = 0;
	srand(time(NULL));
//	printf("in here");
	char* roomTypes[3] = {"START_ROOM", "MID_ROOM", "END_ROOM"};
	int randType;
	int haveStart = 0;
	int haveEnd = 0;
	int count = 0;
	char* rooms[10] = {"arcade", "ballroom", "library", "study", "kitchen", "playroom", "gym", "bedroom", "garage", "poolroom"};
	//printf("%s", rooms[1]);
	char* myRooms[7] = {"", "", "", "", "", "", ""};
	char* fileNames[7] = {"first", "second", "third", "fourth", "fifth", "sixth", "seventh"};
	for (i = 0; i<=6; i++){
		fp = fopen(fileNames[i], "a");
		do {
			haveSeen = 0;
			randRoom = rand() % 10;
			for (j = 0; j<=6; j++){
				if (randRoom == seenRooms[j])
					haveSeen = 1;
			}	
		
		} while(haveSeen == 1);
		seenRooms[i] = randRoom;
		fputs("ROOM NAME: ", fp);
		fputs(rooms[randRoom], fp);
		fputs("\n", fp);
		fputs("ROOM TYPE: ", fp);	
		myRooms[i] = rooms[randRoom];
		count = count+1;
		randType = rand() % 3;
		do{
			printf("in here\n");
			if (randType == 0 && haveStart == 0){
