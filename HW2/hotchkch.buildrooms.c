#include<stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

/*Christien Hotchkiss
 *hotchkch.buildrooms.c
 *February 7th 2020
 *Assignment 2
 */

//used to determine if a room connection already exists. Return 1 if it does
int alreadyExists(int a, int b[]){
	int i = 0;
	for (i = 0; i<=5; i++){
		if (b[i] == a)
			return 1;
	}
	return 0;

}

//function to populate a file with random roomnames, connections, and room types
void createPopulateFiles(){
	//file pointer
	FILE* fp = NULL;
	//initialize array values and integers to be used
	int i;
	int randRoom;
	int seenRooms[7] = {10, 10, 10, 10, 10, 10, 10};
	int haveSeen = 0;
	int j = 0;
	//so I can use random
	srand(time(NULL));
	//three room types
	char* roomTypes[3] = {"START_ROOM", "MID_ROOM", "END_ROOM"};
	int randType;
	int haveStart = 0;
	int haveEnd = 0;
	int count = 0;
	//initlaize arrays of all 10 possible rooms, file names, and myRooms to populate with 7 of 10 random rooms
	char* rooms[10] = {"arcade", "ballroom", "library", "study", "kitchen", "playroom", "gym", "bedroom", "garage", "poolroom"};
	char* myRooms[7] = {"", "", "", "", "", "", ""};
	char* fileNames[7] = {"first", "second", "third", "fourth", "fifth", "sixth", "seventh"};
	
	//populate all 7 files with the top line that says ROOM NAME: 1 of 10 in rooms array above
	for (i = 0; i<=6; i++){
		fp = fopen(fileNames[i], "a");
		//continue to do this until a you generate a random number that has not been seen
		do {
			haveSeen = 0;
			//generate random index between 0 and 9 to pick a room
			randRoom = rand() % 10;
			for (j = 0; j<=6; j++){
				if (randRoom == seenRooms[j])
					haveSeen = 1;
			}	
		
		} while(haveSeen == 1);
		//keep track of which room has been seen
		seenRooms[i] = randRoom;
		fputs("ROOM NAME: ", fp);
		//write a randomly generate room to file and populate the myRooms array
		fputs(rooms[randRoom], fp);
		fputs("\n", fp);
		myRooms[i] = rooms[randRoom];
		fclose(fp);
	}
//GENERATE ROOM GRAPH HERE
	//initialize variables
	int roomConnections[7][6];
	int satisfyConditions = 0;
	int minusOne = 0;
	//keeps track of which roomIndex is next one to be added
	int indToAdd[7] = {0, 0, 0, 0, 0, 0, 0};
	int r1;
	int r2;
	//initialize all roomConnections to index -1
	for (i = 0; i<= 6; i++){
		for (j = 0; j<=5; j++){
			roomConnections[i][j] = -1;
		}
	}

	//continue populating roomConnections until atleast 3 connections from every room are made
	while (satisfyConditions == 0){
		//generate two random numbers (this forms a connection as long as they are not the same)
		r1 = rand() % 7;
		r2 = rand() % 7;
		if (r1 != r2){
			//if this connection does not already exist and a room does not already have 6 connections then make the connection
			//by incremenintg the indeces to add and populating myRoomConnections
			if (indToAdd[r1] <= 5 && indToAdd[r2] <= 5 && alreadyExists(r1, roomConnections[r2]) == 0){
				roomConnections[r1][indToAdd[r1]] = r2;
				roomConnections[r2][indToAdd[r2]] = r1;
				indToAdd[r1] = indToAdd[r1] + 1;
				indToAdd[r2] = indToAdd[r2] + 1;
			}

		}
		//check to see if all the conditions are met
		satisfyConditions = 1;
		for (i = 0; i<= 6; i++){
			if (indToAdd[i] < 3)
				satisfyConditions = 0;
		}
	}

//GENERATE ROOM GRAPH ABOVE

//populate files with rooms!
	
	char* connectNum[6] = {"1", "2", "3", "4", "5", "6"};
	for (i = 0; i<=6; i++){
		//open up each file
		fopen(fileNames[i], "a");
		for (j = 0; j<=5; j++){
			//loop over all connections. If connection is not the default -1. Then write to a file the CONNECTION #: room
			//continue to do this until all connections are written
			if (roomConnections[i][j] != -1){
				fputs("CONNECTION ", fp);
				fputs(connectNum[j], fp);
				fputs(": ", fp);
				fputs(myRooms[roomConnections[i][j]], fp);
				fputs("\n", fp);

			}
		}
		fclose(fp);


	}

//determine mid room start room and end rooms
	//open all six files in append mode
	for (i = 0; i<=6; i++){
		fopen(fileNames[i], "a");
		fputs("ROOM TYPE: ", fp);
		count = count+1;
		do{
			//determine a random number between 0 and 2 for start, mid, and end room
			randType = rand() % 3;
			//if a start room has not been determined then write "START_ROOM" to last line of file
			if (randType == 0 && haveStart == 0){
				fputs(roomTypes[0], fp);
				haveStart = 1;
				break;
			}
			//if end room has not been determined then write "END_ROOM" to last line of file
			if (randType == 2 && haveEnd == 0){
				fputs(roomTypes[2], fp);
				haveEnd = 1;
				break;
			}
			//otherwise write mid room to file 
			if(count<5 || (haveEnd == 1 && haveStart == 1)){
				fputs(roomTypes[1], fp);
				break;
			}
		} while(1);
		fputs("\n", fp);
		//close fp (done writing to the file)
		fclose(fp);

	}	
}

void createDirectory(){
	//initalize variables where id will be new string to change directory to and dirname is the prefix we want to create
	int check;
	char* dirname = "hotchkch.rooms.";
	int size;
	char* id;
	
	//get the process ID
	pid_t pid = getpid();

	//get size of character array with dirname and pid
	size = snprintf(NULL, 0, "%s%d", dirname, pid);
	id = malloc(size+1);
	//store process id appended to hotch.rooms in id
	sprintf(id, "%s%d", dirname, pid);

	
	//make directory with read and write access
	check = mkdir(id, 0777);
	chdir(id);

	//call function to create all the buildroom files
	createPopulateFiles();

	
}

int main(){
	//create new Directory hotchkch.rooms.processID
	createDirectory();
	
	return 0;
}


