#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
/*Christien Hotchkiss
 *hotchkch.adventure.c
 *February 7th 2020
 *Assignment2
 */

pthread_mutex_t timeMutex;


void printTime(){
	//unlock the mutex
	pthread_mutex_unlock(&timeMutex);
	//to print day of week and month in character format
	char* months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
	char* daysOfWeek[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};	
	
	time_t now;
	
	//get local time using struct tm
	struct tm *local = localtime(&now);
	time(&now);
	local = localtime(&now);
	
	//if the hour is prior to 12 then it is am, else it is pm
	if (local->tm_hour < 12)
		printf("  %02d:%02dam, ", local->tm_hour, local->tm_min);
	else
		printf("  %02d:%02dpm, ", local->tm_hour - 12, local->tm_min);
	//print day of week and month/year as specified by assignment descriptions
	printf("%s, ", daysOfWeek[local->tm_wday]);
	printf("%s %d, %d\n\n", months[local->tm_mon], local->tm_mday, local->tm_year+1900);
	
	//lock the mutex
	pthread_mutex_lock(&timeMutex);
}

void playGame2(){
	//initialize variables	
	char* myRooms[7]; 
	//character arrays of potential rooms and file names
	char* rooms[10] = {"arcade", "ballroom", "library", "study", "kitchen", "playroom", "gym", "bedroom", "garage", "poolroom"};
	char* fileNames[7] = {"first", "second", "third", "fourth", "fifth", "sixth", "seventh"};
	FILE* fileStream;
	//character array of potential room types
	char* roomTypes[3] = {"START_ROOM", "MID_ROOM", "END_ROOM"};
	char* myRoomTypes[7];
	
	//fileText to read lines from files
	char fileText[100];
	int i;
	int k;
	//integer array to keep track of what index the next room connection will be added at in myRoomConnections
	int indToAdd[7] = {0, 0, 0, 0, 0, 0, 0};
	char** myRoomConnections[7][6];

	//open and read the first line of every file to determine which room names are present
	for (k = 0; k<=6; k++){
		fileStream = fopen(fileNames[k], "r");
		fgets(fileText, 100, fileStream);
		for (i = 0; i<=9; i++){
			//populate the myRooms string array with rooms
			if(strstr(fileText,rooms[i]) != NULL)
				myRooms[k] = rooms[i];
			 
		}
		fclose(fileStream);
	}
	
	//populate the myRoomTypes by opening up each file and reading the last line
	for (k = 0; k<=6; k++){
		fileStream = fopen(fileNames[k], "r");
		//get to last line of the file
		while(!feof(fileStream))
			fgets(fileText, 100, fileStream);

		for (i = 0; i<=2; i++){
			//assign myRoomTypes to the name of the roomtype as specified in the file
			if(strstr(fileText,roomTypes[i]) != NULL)
				myRoomTypes[k] = roomTypes[i];
		}
		fclose(fileStream);
	}	
	
	//initialize the myRoomConnectiosn doulbe array to all point to NULL
	for (k = 0; k<=6; k++){
		for (i = 0; i<=5; i++){
			myRoomConnections[k][i] = NULL;
		}
	}	

	//populate myRoomConnections by opening and reading each file
	for (k = 0; k<= 6; k++){
		fileStream = fopen(fileNames[k], "r");
		while(!feof(fileStream)){
			//read lines one at a time and search for the keyword CONNECTION
			fgets(fileText, 100, fileStream);
			if(strstr(fileText,"CONNECTION") != NULL){
				//then loop over all the rooms and populate myRoomConnections if there is a match between line in file and room name
				for (i = 0; i<=6; i++){
					if(strstr(fileText, myRooms[i]) != NULL){
						myRoomConnections[k][indToAdd[k]] = myRooms[i];
						//increment index to add so I know which index needs to be populate of myRoomConnections
						indToAdd[k] = indToAdd[k] + 1;
						
					}

				}
			}
		}
	
	}
	
	//strings to keep track of what the current room is 
	char currentRoom[8];
	
	int canPlay = 1;
	char endRoom[8];
	//initalize a path variable to keep track of room indeces
	int path[100];
	for (i = 0; i<100; i++)
		path[i] = 10;
	//determines which room user is at in rooms array
	int currentRoomIndex;
	char playerInput[100];
	//used to determine if game should keep going and if user picked valid room
	int canMoveOn = 0;
	int isValidInput = 0;

	//keep track of how many turns it takes user to get
	int nSteps = 0;
	
	//loop to determine name of room that is the start and ending room and store in variables
	for (i = 0; i<=6; i++){
		if (!strcmp("START_ROOM", myRoomTypes[i])){
			strcpy(currentRoom, myRooms[i]);
			currentRoomIndex = i;
		}
		if (!strcmp("END_ROOM", myRoomTypes[i]))
			strcpy(endRoom, myRooms[i]);
	}

	//main game loop
	while (canPlay == 1){
		do{
			isValidInput = 0;
			//print out the current location and all the possible connections by looping through myRoomConnections
			printf("CURRENT LOCATION: %s\n", currentRoom);
			printf("POSSIBLE CONNECTIONS: %s", myRoomConnections[currentRoomIndex][0]);
			for (i = 1; i<=5; i++)
				if (myRoomConnections[currentRoomIndex][i] != NULL)
					printf(", %s", myRoomConnections[currentRoomIndex][i]);
			//get user input on where he/she wants to go
			printf(".\nWHERE TO? >");
			gets(playerInput);
			printf("\n");
		
			//if the user enters "time", call printTime function which uses mutex and prints local time
			if (!strcmp(playerInput, "time")){
				printTime();
				isValidInput = 1;
			}
			//loop over all the RoomConnections at currentRoomIndex to determine if user input was valid
			for (i = 0; i<=5; i++){
				if (myRoomConnections[currentRoomIndex][i] != NULL){
					if (!strcmp(playerInput, myRoomConnections[currentRoomIndex][i])){
						//is Valid room so now determine the future room by setting currentRoomIndex to the desired new room
						isValidInput = 1;
						for(k = 0; k<=6; k++){
							if (!strcmp(playerInput, myRooms[k]))
								currentRoomIndex = k;
						}
						//update the path and numSteps variables to keep track of path to END_ROOM
						path[nSteps] = currentRoomIndex;
						nSteps++;
						strcpy(currentRoom, myRooms[currentRoomIndex]);
						//user can go to the next turn
						canMoveOn = 1;
					}
				}
			}
			//print message that user entered an invalid room name
			if (isValidInput == 0)
				printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
		} while (canMoveOn == 0);
		//determine if the game is over - if current room == endRoom then they win
		if (!strcmp(currentRoom, endRoom)){
			printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
			canPlay = 0;
			printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", nSteps);
		}	


	}
	//print path to victory (does not include starting room)
	for (i = 0; i<sizeof(path)/sizeof(path[0]); i++){
		if (path[i] != 10)
			printf("%s\n", myRooms[path[i]]);
	}
	
}

void changeDirectory(){

	//initialize variables to use when finding the most recently modified directory
	//that has a prefix of "hotchkch.rooms"
	//most of this code was found on the canvas class website
	int newestDirTime  = -1;
	char targetDirPrefix[32] = "hotchkch.rooms.";
	char newestDirName[256];
	memset(newestDirName, '\0', sizeof(newestDirName));

	//initalize structs to store information about directories
	DIR* dirToCheck;
	struct dirent *fileInDir;
	struct stat dirAttributes;
	
	dirToCheck = opendir(".");
	

	if (dirToCheck > 0){
		//while there are more directories to check
		while ((fileInDir = readdir(dirToCheck)) != NULL){
			//if the target prefix is in the name of the directory
			if (strstr(fileInDir->d_name, targetDirPrefix) != NULL){
				stat(fileInDir->d_name, &dirAttributes);
				//if the latest timestamp is greater than current latest timestamp then we found our most recent dir
				if ((int)dirAttributes.st_mtime > newestDirTime){
					newestDirTime = (int)dirAttributes.st_mtime;
					memset(newestDirName, '\0', sizeof(newestDirName));
					strcpy(newestDirName, fileInDir->d_name);
					
				}
			}
		}
	}
	closedir(dirToCheck);

	//change directory to directory with files in it
	chdir(newestDirName);
}
int main(){
	//lock the mutex at start of program
	pthread_mutex_lock(&timeMutex);

	//call function to find a change working directory
	changeDirectory();
	
	//call playGame2() function to set up and play game
	playGame2();

	//destroy mutex at very end of program
	pthread_mutex_destroy(&timeMutex);

	return 0;
	
}
