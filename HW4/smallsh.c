/*Filename: smallsh.c
 * Author: Christien Hotchkiss
 * Due: 26 February 2020
 * This assignment waas very hard and I have never spent more hours on a school assignment. However, I think I have finally got it all and it is very satisfying. I have handled cd, status, and exit on my own and have let all other functionality of a shell be handled by execvp. I learned a ton doing this assignment and feel my coding skills got better just with this assignment
 */

//Ask about: ^Z clearing input and  and $$

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>



#define MAXIMUM_ARGUMENTS	512
#define	MAX_COMMAND_LINE	2048

//add array of background PIDs and index of current background process
int inForeground = 1;
int curbg = 0;
pid_t bgProcess[100];

//print working directory - I dont think i use this function
void printWorkingDir(){
	char cwd[512];
	//if it is not null, print to screen working directory
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		printf("%s\n", cwd);
	else
		perror("getcwd() error");

}

//handle cd to change to home directory
void changeToHomeDir(){
	char * homedir = getenv("HOME");	
	//change if not NULL
	if (homedir != NULL)
		chdir(homedir);	
}

//handle cd with arguments
void changeDirArgument(char command[]){
	int ind = 0;
	char result[256];
	int i;
	//initialize result directory
	for (i = 0; i< 256; i++)
		result[i] = '\0';
	int cInd = 0;
	int flag = 0;
	char id[5];
	int indStart = 0;
	//if the command is cd followed by a space
	if (command[0] == 'c' && command[1] == 'd' && command[2] == ' '){
		while (command[cInd] != '\0'){
			
			//if we can add to reslut, set result[ind] to command
			if (flag == 1){
				result[ind] = command[cInd];
				ind++;	
			}
			if (command[cInd] == ' ' && (command[cInd+1] != ' ' && command[cInd+1] != '\0'))
				flag = 1;
			cInd++;

		}
		//work
		//check if the $$ expansion is in a cd call
		for (i = 0; i<255; i++){
			if (result[i] != '\0'){
				//if two consecutive characters are $
				if (result[i] == '$' && result[i+1] == '$'){
					//get the pid and store it in variable id
					pid_t pid = getpid();
					sprintf(id, "%d", pid);
					indStart = i;
					//loop over the length of id string and store each character in corresponding spot in result array
					for (ind = 0; ind < strlen(id); ind++){
						result[indStart] = id[ind];
						indStart++;

					}		
				}
			
			}
		}
	//	printf("directory to change to: %s\n", result);
		//end work
		//change the directory
		if(chdir(result) == -1)
		//	printf("Directory doesn't exist\n"); dont print this
		fflush(stdout);
	}
	//reset the result character array
	for (i = 0; i<256; i++)
		result[i] = '\0';
	
}

//get the number of arguments in an input string
int getNumArguments(char in[]){
	int num = 0;
	int cur = 0;
	
	//while input strings is not null
	while (in[cur] != '\0'){
		//count the number of spaces to determine number of arguments
		if (in[cur] == ' ' && (in[cur+1] != '\0' && in[cur+1] != ' '))
			num++;
		cur++;	
	}
	
	return num;
}

//catch ^Z signal. if we are in foreground, enter foreground only mode
void catchZ(int a){
	
	//act differently if we are in foreground or background
	if (inForeground == 1){
		char* out = "\nEntering foreground-only mode (& is now ignored)\n";
		write(1, out, 50);
	//	printf("Exiting foreground-only mode\n");
		fflush(stdout);
	}
	
	//if we aren't in foreground then exit forground only mode
	if (inForeground == 0){
		//printf("Entering foreground-only mode (& is now ignored)\n");
		char* out = "\nExiting foreground-only mode\n";
		write(1, out, 30);
		inForeground = 1;
		fflush(stdout);
	//	main();
		return;
	}
	inForeground = 0;
	//return main();
	
}

//main function that paress input and calls execvp
void executive(char in[], struct sigaction contC, int* cExitStatus){
	
	//intialize variables
	int backGround = 0;
	
	char inputFile[256];
	char outputFile[256];
	int hasArgs = 0;
	int hasInput = 0;
	int hasOutput = 0;
	int hasAmp = 0;
	int i;
	int i2 = 0;
	int argNum = 1;
	int j = 0;
	//create argv to send to execvp
	char **argv = calloc(512, sizeof(char *));
	for (i = 0; i<512; i++){
		argv[i] = calloc(256, sizeof(char));
	}
	//initialize input/output files
	for(i = 0; i<256; i++){
		inputFile[i] = '\0';
		outputFile[i] = '\0';
	}
		
	i = 0;
	//find command
	while (1){
		//break loop once we encounter first space
		if (in[i] == ' ' || in[i] == '\0')
			break;
		//store command in argv[0]
		argv[0][i] = in[i];
		i++;
	}
	argv[0][i] = '\0';
	
	i = 0;
	int length = strlen(in);
	//get pid of current process
	pid_t pid = getpid();
//dont	char * temp;
	//populate argv
	while(1){
		//if we can store arguments and in[i] doesn't equal space
		if (hasArgs == 1 && in[i] != ' '){
			//store into argv and increment
			argv[argNum][i2] = in[i];
		//	if (strcmp(in[i], "$") == 0 && strcmp(in[i-1], "$" == 0)){
//dont				pid_t pid= getpid();
//dont				sprintf(temp, "%d", pid);
//dont				strncat(argv[argNum], temp, strlen(temp));
//dont			}
		//	printf("There is a $$\n");
		//	}
			i2++;
			
		}
		//if we have encountered the next argument
		if (in[i] == ' ' && hasArgs == 1){
			//if current argument = $$
			if (strcmp(argv[argNum], "$$") == 0){
				//get the pid and store it in argv
				pid_t pid = getpid();
				sprintf(argv[argNum], "%d", pid);
			}
			//reset and increment counter variables
			argNum++;
			i2 = 0;
		}
		//find break case
		if ((in[i] == ' ' && (in[i+1] == '<' || in[i+1] == '>' || in[i+1] == '&')) || in[i] == '\0')
			break;
		//once we encounter a space we can now start reading arguments
		if (in[i] == ' ')
			hasArgs = 1;
		i++;
		if (i == length)
			break;
					
	}
	//store into argv[argNum] the pid if it equals $$
	if (strcmp(argv[argNum], "$$") == 0){
		pid_t pid = getpid();
		sprintf(argv[argNum] , "%d", pid);
	}

	
	if (hasArgs == 0)
		argv[1] = '\0';
	
	//reset variables
	i = 0;
	i2 = 0;
	
	//find input file
	while(1){
		//break case
		if ((in[i] == ' ' && hasInput == 1) || in[i] == '\0')
			break;
		//once we find < we can read in input File
		if (in[i] == ' ' && in[i-1] == '<')
			hasInput = 1;
		else if (hasInput == 1){
			inputFile[i2] = in[i];
			i2++;
		}
		i++;
	}
	//reset variables
	i = 0;
	i2 = 0;
	//find output file
	while (1){
		//break case
		if ((in[i] == ' ' && hasOutput == 1) || in[i] == '\0')
			break;
		//once we find > we can read in output file
		if (in[i] == ' ' && in[i-1] == '>')
			hasOutput = 1;
		else if (hasOutput == 1){
			outputFile[i2] = in[i];
			i2++;
		}
		i++;
	}

	//reset variables
	i = 0;
	while (1){
		//look for & in code
		if (in[i] == '\0' && in[i-1] == '&' && in[i-2] == ' ')
			hasAmp = 1;
		//if last character is not & then we dont have &
		if (in[i] == '\0')
			break;
		i++;
	}

	//now I have got all input. correctly fork 
	//if the command on the input isn't cd, exit, or status, fork off a child
	pid_t spawnpid = -5;
	int ten = 10;

	//create file descriptors
	int input = 0;
	int output = 0;
	int fid;
//	int j;
	//working here
	//variables to check for $$ in middle of argument string
	char res[5];
	int len = 0;
	int k = 0;
	int indexLoop = 0;
	//initialize reslutant string
	for (i = 0; i<5; i++){
		res[i] = '\0';
	}
	//loop over all in argv
	for (i = 0; i<512; i++){
		if (argv[i] != '\0'){
			for (j=0; j<255; j++){
				//if they values aren't null and argv[i][j+1] and argv[i][j] both equl $
				if (argv[i][j] != '\0' && argv[i][j+1] != '\0'){
					if (argv[i][j] == '$' && argv[i][j+1] == '$'){
						//get process id and one by one store it in argv
						pid_t pid = getpid();
						sprintf(res, "%d", pid);
						indexLoop = j;
						//loop over contents of string containing pid and store them into argv
						for (k = 0; k<strlen(res); k++){
							argv[i][j] = res[k];
							j++;
						}
					}
				}
			}
		}

	}
	//printf("this is res: %s", res);
	//ending here
	//fork the process
	spawnpid = fork();
	switch (spawnpid){
		//failed to fork
		case -1:
			perror("Hull Breach!");
			exit(1);
			break;
		//child process
		case 0:
			//set control C to act as default
			contC.sa_handler = SIG_DFL;
			sigaction(SIGINT, &contC, NULL);
		
			//open input/output files - from thinkage.ca	
			if (inputFile[0] != '\0'){
				//input read only
				input = open(inputFile, O_RDONLY);
		
				//print error if we fail
				if (input == -1){
					printf("cannot open %s for input\n", inputFile);
					//perror("cannot open %s for input\n", inputFile);
					exit(1);
					fflush(stdout);
				}
				
				//using dup2 to dupliate open file descriptor - from mkssoftware.com		
				fid = dup2(input, 0);
				
				//if dup2 failed, print error message
				if (fid == - 1){
					perror("unable to assign input file\n");
					exit(2);
				}

			}

			//if there is an output file
			if (outputFile[0] != '\0'){
				//output file is write only or created if it doesn't already exist
				output = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
				
				//print error if we fail to open it
				if (output == -1){
					perror("Unable to open output file: \n");
					exit(1);
				}
				
				//use dup2 to redirect output
				fid = dup2(output, 1);
				//print error if dup2 fails
				if (fid == -1){
					perror("Unable to assign output file\n");
					exit(2);
				}
				
			}
			//decrement number of legit arguments if argv has &
			if (hasAmp == 1)
				argNum--;
			//set all strings in argv to null if they aren't arguments
			for (i = 0; i<512; i++){
				if (i > argNum )//|| (strcmp(argv[i], '&') == 0))
					argv[i] = '\0';
			}
			//call the execvp function
			//execvp has two parameters: first is name of file to be executed. second is pointer to array of character strings
			//it returns a - value if executation fails
			
			//use execvp to execute command specified in argv
			if (execvp(*argv, argv) < 0){
				//print error and exit 2 if no such file/executable
				printf("%s: no such file or directory\n", argv[0]);
			
				fflush(stdout);
				exit(2);
			}
			
			//check to see if program has exited normally. If it has print exit value
			if (WIFEXITED(*cExitStatus) != 0){
				printf("exit value %d\n", WEXITSTATUS(*cExitStatus));
				fflush(stdout);
			}
			//check to see if program exicted by signal. if it has print termination signal
			if (WIFSIGNALED(*cExitStatus) != 0){
				printf("terminated by a signal %d\n", WTERMSIG(*cExitStatus));
				fflush(stdout);
			}

			break;

		//parent process
		default:
			//online method
			//while (wait(&exitStatus) != spawnpid)
			//	;
			
			// if we are in foreground and there is &, then add this process to the array of background processes
			if (inForeground == 1 && hasAmp == 1){
				printf("background pid is %d\n", spawnpid);
				fflush(stdout);
				bgProcess[curbg] = spawnpid;
				curbg++;
			}
			//if it isn't in background, check for termination
			else{
				//wait for process to complete
				waitpid(spawnpid, cExitStatus, 0);
				//print if exited by status
				if (WIFSIGNALED(*cExitStatus)){
					printf("terminated by signal %d\n", WTERMSIG(*cExitStatus));
					fflush(stdout);
				}
				pid_t inFG = waitpid(spawnpid, cExitStatus, 0);
			}

			break;
			
			//if exited normally print the exit value
			if (WIFEXITED(*cExitStatus) != 0){
				printf("exit value %d\n", WEXITSTATUS(*cExitStatus));
				fflush(stdout);
			}
			//if terminated by a signal, print out the termination signal
			if (WIFSIGNALED(*cExitStatus) != 0){
				printf("terminated by a signal %d\n", WTERMSIG(*cExitStatus));
				fflush(stdout);
		//	}
			//child exited by signal - from geeksforgeeks.org
			//if (WIFSIGNALED(exitStatus)){
				//online
				//psignal(WTERMSIG(exitStatus), "Exit signal");
				//first metohd
		}
	}


}

int main(){
	//initialize variables
	int i;
	//set bgProcces to a default of -1
	for (i = 0; i<100; i++){
		bgProcess[i] = -1;
	}
	//ignore control C. Create a signal handler for ^C
	struct sigaction contC = {0};
	contC.sa_handler = SIG_IGN;
	sigfillset(&contC.sa_mask);
	contC.sa_flags = 0;
	sigaction(SIGINT, &contC, NULL);


	//handle control Z. Use funciton catchZ to handle ^Z.
	struct sigaction contZ = {0};
	contZ.sa_handler = catchZ;
	sigfillset(&contZ.sa_mask);
	contZ.sa_flags = 0;
	sigaction(SIGTSTP, &contZ, NULL);

	//variables necessary to read user input
	int loop = 1;
	char input[MAX_COMMAND_LINE];
	pid_t pid = getpid();
	char inputNew[MAX_COMMAND_LINE];
	//keep track of exit status of proccess
	int exitStatus = 0;
	i = 0;
	int j;
	//read user input while they do not type exit
	do {
		//loop through all background proccess array
		for (j = 0; j< 100; j++){
			if (bgProcess[j] != -1){
				
				//if there is a real id then check to see if it has exited
				pid_t temp = bgProcess[curbg - 1];
				//let program continue, but check for termination
				if (waitpid(bgProcess[j], &exitStatus, WNOHANG) > 0){
					//if the process has exited normally, print its exit value and print the PID of the process that exited
					if (WIFEXITED(exitStatus)){
						printf("background pid %d is done: exit value %d\n", bgProcess[j], WEXITSTATUS(exitStatus));
					//if the process has exited from signal, print pid and the termination signal
					}
					if (WIFSIGNALED(exitStatus)){
						printf("background pid %d is done: ", bgProcess[j]);
						printf("terminated by signal %d\n", WTERMSIG(exitStatus));
					}
					//update background process array to correctly store only the active pids
					bgProcess[j] = temp;
					bgProcess[curbg-1] = -1;
					curbg--;
					
				}
			}
				
		}
		printf(": ");
		
		//read user input
		fgets(input, MAX_COMMAND_LINE, stdin);
		strcpy(inputNew, input);
		input[strlen(input) - 1] = 0;
		//change the directory if input == "cd" with no arguments	
		if (strcmp(input, "cd") == 0){
			changeToHomeDir();
		}
		
		//change to directory specfieid by the user if there are arguments following the cd command
		else if (input[0] == 'c' && input[1] == 'd' && input[2] == ' ' && getNumArguments(input) == 1){
			changeDirArgument(input);
		}
		
		//set loop variable to 0 to exit loop
		else if (strcmp(input, "exit") == 0){
			loop = 0;
		}
		else if (input[0] == '#' || input[0] == '\0'){ // do nothing when a line is comment
		
		}

		//handle status input
		else if (strcmp(input, "status") == 0){
			//child exited normally - from eeksforgeeks.org
			if (WIFEXITED(exitStatus)){
				printf("exit value %d\n", WEXITSTATUS(exitStatus));
				fflush(stdout);
			}
			//if child exited from a signal: print termination signal
			else
				printf("terminated by signal %d\n", WTERMSIG(exitStatus));
				fflush(stdout);
		}
		//if user requests status of last exit value. simply do the same thing
		else if (strcmp(input, "status &") == 0){
			
			//if exited normally, print exit sttus
			if (WIFEXITED(exitStatus)){
				printf("exit value %d\n", WEXITSTATUS(exitStatus));
				fflush(stdout);
			}
			//if terminated by signal, print the termination signal number
			else
				printf("terminated by signal %d\n", WTERMSIG(exitStatus));
				fflush(stdout);
		}
		
		//call executive function to read input and call execvp
		else {
			executive(input,contC, &exitStatus);
			input[0] = '\0';
		}
	
	//look until exit is typed to command screen. setting loop to 1
	}while(loop);

	return 0;
}
