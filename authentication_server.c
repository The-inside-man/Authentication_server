//////////////////////////////////////////////////////////
//Name:			Jacob Brown			//
//Student #:		100 762 690			//
//Course:		COMP3203			//	
//EDITED:  		03 Oct 2014			//
//Title:		Authentication Server.c	(UDP)	//
//////////////////////////////////////////////////////////

#include <stdio.h>      					// for printf() and fprintf() 
#include <sys/socket.h> 					// for socket() and bind() 
#include <arpa/inet.h>  					// for sockaddr_in and inet_ntoa() 
#include <stdlib.h>     					// for atoi() and exit() 
#include <string.h>     					// for memset() 
#include <unistd.h>     					// for close() 
#include <sys/types.h>						//
#include <netinet/in.h>						// 
#include <time.h>						// for time variables
#include <dirent.h>						// for files
#include <sys/stat.h>						// for specs of files
#include <string.h>						// for string manipulation
#include "authentication.h"					// for authentication protocols

#define MAX 2048     						// Longest string to echo 
#define FILE_MAX_SIZE 4096					// Largest file size
#define ANSI_COLOR_RED 		"\x1b[31m"			// Red color for text
#define ANSI_COLOR_BLUE    	"\x1b[34m"			// Blue Color for text
#define ANSI_COLOR_GREEN   	"\x1b[32m"			// Green color for text
#define ANSI_COLOR_CYAN    	"\x1b[36m"			// Cyan Color for text
#define ANSI_COLOR_MAGENTA  	"\x1b[35m"			// Magenta color for text
#define ANSI_COLOR_GREY		"\x1b[33m"			// Yellow for logging text
#define ANSI_COLOR_NORMAL   	"\x1b[0m"			// Normal color for text

int main (int argc, char *argv[])
{
	int 			sock;					// The Socket
  	struct 			sockaddr_in  thisAddr, clientAddr;	// Socket struct
  	struct			dirent *myFile;				// Struct used for when displaying files in directory
  	struct 			stat myStat;				// Struct for using stats for files
  	fd_set 			readfds;					
  	unsigned short 		thisPort;     				// Server port
    	char 			buffer[MAX];        			// Buffer for echo string
    	char			fileBuff[FILE_MAX_SIZE];		// File Buffer
    	char			strBuff[MAX];				// String buffer
    	int 			recvMsgSize;                		// Size of received message
    	DIR 			*thisDir;				// Working Directory for server
    	//char			cwd[MAX];				// Stores the Current working Directory
    	char 			fl_name[255];				// File name is stored in this buffer
    	char 			cmd[80];				// Small buffer for commands from client
    	int 			authenticated = 0;			// set to 1 when true
	time_t			rawtime;				// Time
	struct			tm * time_info;				// Time struct
	
	time( &rawtime );
    
    if (argc != 2)         						// We verify the number of arguments inputed when starting the server
    {
        printf("Must include argument: <UDP SERVER PORT>");		// Error message if incorrect amount of arguments
        exit(1);
    }
    
    thisPort = atoi(argv[1]); 						// Assigning the first argument to the port
    
    
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);			// Creation of the socket
    if(sock < 0) {							// Verify it was created correctly
    	printf("Failed to open the server socket\n");			// Error message displayed if not created properly
    	exit(-1);
    }
    
   
    memset(&thisAddr, 0, sizeof(thisAddr));   					// Here the address is set up and memory is allocated
    thisAddr.sin_family = AF_INET;                
    thisAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    thisAddr.sin_port = htons(thisPort);  					// Use of htons for network bit order
    
    if (bind(sock, (struct sockaddr *) &thisAddr, sizeof(thisAddr)) < 0){ 	// Bind the sockets and check for success
    	printf("Failed to bind socket\n");					// Error message displayed of fails to Bind the socket
    	exit(-1);								// Program ends
    }
    
    unsigned int	size = sizeof(thisAddr); 				//The size of the clients address is stored here
    
    //	Print the start up message with legend for color codes for operations
    
    system("clear");											// Clear the screan
    printf(ANSI_COLOR_MAGENTA"********************************************************************************"                	  	ANSI_COLOR_NORMAL"\n");
    printf(ANSI_COLOR_MAGENTA"*"ANSI_COLOR_CYAN"             AUTHENTICATION SERVER BY JAKE BROWN - NOVEMBER 2014              "	  	ANSI_COLOR_MAGENTA"*\n"); 
	printf(ANSI_COLOR_MAGENTA"********************************************************************************"			ANSI_COLOR_NORMAL"\n");
	printf(ANSI_COLOR_MAGENTA"*"ANSI_COLOR_CYAN"                                     LEGEND                                   "	ANSI_COLOR_MAGENTA"*\n");
	printf(ANSI_COLOR_MAGENTA"********************************************************************************"			ANSI_COLOR_NORMAL"\n");
	printf(ANSI_COLOR_MAGENTA"*"    ANSI_COLOR_RED"                             ERROR MESSAGES                                   "	ANSI_COLOR_MAGENTA"*\n");
	printf(ANSI_COLOR_MAGENTA"*"   ANSI_COLOR_BLUE"                             WORK DONE BY SERVER                              "	ANSI_COLOR_MAGENTA"*\n");
	printf(ANSI_COLOR_MAGENTA"*"  ANSI_COLOR_GREEN"                             SUCCESSFUL OPERATION                             "	ANSI_COLOR_MAGENTA"*\n");
	printf(ANSI_COLOR_MAGENTA"*"   ANSI_COLOR_CYAN"                             HASH INFORMATION                                 "	ANSI_COLOR_MAGENTA"*\n");
	printf(ANSI_COLOR_MAGENTA"*"ANSI_COLOR_MAGENTA"                             INFORMATION RECEIVED FROM CLIENT                 "	ANSI_COLOR_MAGENTA"*\n");
	printf(ANSI_COLOR_MAGENTA"*"   ANSI_COLOR_GREY"                             INFORMATION PRINTED TO LOG                       "	ANSI_COLOR_MAGENTA"*\n");
	printf(ANSI_COLOR_MAGENTA"********************************************************************************"			ANSI_COLOR_NORMAL"\n");
    
    
    
    
    
    
    printf(ANSI_COLOR_BLUE "<SERVER>--waiting for interaction from client..." ANSI_COLOR_NORMAL "\n");			// Once socket created and bound we wait for a client to connect
    
    while(1) {										// We start a main loop for the server
    	memset(buffer, 0, sizeof(buffer));						// Clear buffer
    	memset(fileBuff, 0, sizeof(fileBuff));  					// Clear fileBuff
    	memset(strBuff, 0, sizeof(strBuff));   						// Clear strBuff
    	memset(cmd, 0, sizeof(cmd));   							// Clear cmd
    	char			cwd[MAX];						// Stores the Current working Directory
    	char 			attempts[2];						// Stores attempts from files
    	char			username[80];						// Store Username
    	
    	while(!authenticated){								// We start the loop to authenticate users
    		/* 	Variables to be used with Authenticating  */
    		memset(buffer, 0, sizeof(buffer));					// Clear buffer
    		char 	*line 		= NULL;						// Char array to store lines from file
			size_t 	len 		= 0; 					// Stores length of line from readline of file
			char 	output[33];
    		char 	aKey[33];
    		
    		memset(aKey, 0, sizeof(aKey));
    		
    				output[32] 	= 0x00;							// Output buffer for hash function
    				aKey[32] 	= 0x00;							// Key buffer for hash function
			int 	aSalt 		= 0;							// Salt variable sent to hash function
			int 	found 		= 0;							// Assigned 1 when user is found
			int	result		= 0;							// Assigned 1 if hash is successful
			char 	*input;
			int 	locked		= 0;
    		
    		recvMsgSize = recvfrom(sock, buffer, MAX, 0, (struct sockaddr *) &thisAddr, &size); //wait till we receive a rqst msg from client
    		if ((recvMsgSize) < 0){
    			printf("recvfrom() failed\n");					// If receiving fails, error message is displayed
			}
			strcpy(username, buffer);
			
			/*  Check to see if user name is locked  */
			FILE * locked_users = fopen("bin/locked.txt", "r");
			
			if(locked_users == NULL){							// Check to see if file is valid
				printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error: Code 7 - no Locked users file found" ANSI_COLOR_NORMAL "\n");	// Check if file is valid
			}
			while(getline(&line, &len, locked_users) != -1){
				// we check for a match
				// If found we then read the next line which will be the corresponding password
				if(line[strlen(line) - 1] == '\n'){
					line[strlen(line) - 1] = '\0';
				}
				if((strcmp(line,buffer) == 0)){
					// Meaning username is locked
					locked = 1;
				}
			}
			fclose(locked_users);
			
			if(!locked){
			/*  End of checking for locked users */
			printf(ANSI_COLOR_BLUE "<SERVER>--Searching for user: %s" ANSI_COLOR_NORMAL "\n", buffer);	// Buffer is printed to server screen
			
			FILE *passwords = fopen("bin/users.txt", "r");			// We open the file containing user information to search for passwords
    		
    		if(passwords == NULL){									// Check to see if file is valid
				printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error: Code 7 - no file found" ANSI_COLOR_NORMAL "\n");	// If not shut down server as cannot authenticate any users without file
				exit(1);
			}
			
			/*  We must search for the username now and obtain the correct password  */
			while(getline(&line, &len, passwords) != -1){
				// we check for a match
				// If found we then read the next line which will be the corresponding password
				if(line[strlen(line) - 1] == '\n'){
					line[strlen(line) - 1] = '\0';
				}
				if((strcmp(line,buffer) == 0)){
					printf(ANSI_COLOR_GREEN "<SERVER>--Found user. Getting password..." ANSI_COLOR_NORMAL "\n");
					getline(&line, &len, passwords);
					/*  &line now holds the appropriate password to 
						authenticate with and hash to send to the 
						client. Output will contain the hashed value
						that will be sent to the client along with
						key and salt  */ 
					if(line[strlen(line) - 1] == '\n'){
						line[strlen(line) - 1] = '\0';
					}
					printf("<SERVER>--The password is:\t%s\n", line);
					found = 1;
					aSalt = 0;
					result = hash_value(line, output, aKey, &aSalt);
					break;
				}
			}
			fclose(passwords);
			if(found){																// If user is found we create a hash of password
				if(result){
					char 	salt_str[12];
					memset(salt_str, 0, sizeof(salt_str));
					printf(ANSI_COLOR_GREEN "<SERVER>--Key created\t :\t"  ANSI_COLOR_CYAN "%s" ANSI_COLOR_NORMAL "\n", aKey);	// Display the key that was produced
					printf(ANSI_COLOR_GREEN "<SERVER>--Hashed Value\t :\t" ANSI_COLOR_CYAN "%s" ANSI_COLOR_NORMAL "\n", output); // Display the generated Hash value
					printf(ANSI_COLOR_GREEN "<SERVER>--Salt created\t :\t" ANSI_COLOR_CYAN "%d" ANSI_COLOR_NORMAL "\n", aSalt);  // Display the salt value created
				
					sprintf(salt_str, "%d", aSalt);									// We convert salt value to string
				
					/*  Now the Key and Salt values must be sent to Client*/
					printf(ANSI_COLOR_BLUE "<SERVER>--Sending values to client for verification..." ANSI_COLOR_NORMAL "\n");
					sendto( sock, aKey, sizeof(char)*32, 0,(struct sockaddr *)  	// key value is sent
												&thisAddr, sizeof(clientAddr)); 
					sendto( sock, salt_str, strlen(salt_str), 0,(struct sockaddr *) // salt value is sent
												&thisAddr, sizeof(clientAddr)); 
					sendto( sock, "finished",9, 0,(struct sockaddr *)  			// finished signal is sent
												&thisAddr, sizeof(clientAddr));
					printf(ANSI_COLOR_GREEN "<SERVER>--Values have been sent to client." ANSI_COLOR_NORMAL "\n");
					
					/*  Now wait for reply from client saying items received  */
					memset(buffer, 0, sizeof(buffer));							// Clear buffer
					recvMsgSize = recvfrom(sock, buffer, MAX, 0, (struct sockaddr *) &thisAddr, &size); //wait till we receive some msg from client
    				if ((recvMsgSize) < 0){
    					printf("recvfrom() failed\n");							// If receiving fails, error message is displayed
					}
					printf(ANSI_COLOR_MAGENTA "<SERVER RECEIVED>--Received: %s" ANSI_COLOR_NORMAL "\n",buffer);
					
					/*  Now we must compare hash values in order to authenticate user*/
					if(compare_hash_values(output,buffer)){ 					// Will return 1 if both values are equal
						authenticated = 1;
						// Send succeeded message to client
						printf(ANSI_COLOR_GREEN "<SERVER>--AUTHENTICATED" ANSI_COLOR_NORMAL "\n");
						sendto( sock, "1",2, 0,(struct sockaddr *)  			// Authenticate signal is sent as value 1
									&thisAddr, sizeof(clientAddr));
						printf(ANSI_COLOR_GREEN "<SERVER>--User has access to the main server" ANSI_COLOR_NORMAL "\n");
						
						FILE * log = fopen("bin/log.txt", "a");
							
						if(log == NULL){										// Check to see if file is valid
							printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error 3: Could not open log file." ANSI_COLOR_NORMAL "\n");	// If not shut down server as cannot authenticate any users without file
						}
						time( &rawtime );
						time_info = localtime ( &rawtime ); 					// Get current time
						char t_info[40];
						strftime(t_info,40, "%D - %T", time_info);
						fprintf(log, "<SERVER NOTE>------User: %s Logged in -- %s\n", username, t_info);							// Write to the log
						printf(ANSI_COLOR_GREEN "<SERVER>--User: %s Logged in -- %s" ANSI_COLOR_NORMAL "\n", username, t_info);
						fclose(log);
						if(remove(username)){
						}
					}else{
						int	num_tries 		= 0;									// Track attempts to log in
						authenticated		= 0;
						char 	*line 		= NULL;									// Char array to store lines from file
						size_t 	len 		= 0; 									// Stores length of line from readline of file
						memset(attempts, 0, sizeof(attempts));
						
						// Send failure message to client
						/* 	If the user fails to log into the account we want to remember the number of attempts per user
							the way we do this is every failed attempt per unique user we will create a file. This file will
							hold a number representing the number of failed attempts. If the number reaches three then we know
							to add that user to the locked user list file. If the user successfully logs in before the three 
							attempts then we delete the file and the user will have three attempts again the next time they 
							have a failed attempt. */
						FILE * user_attempt_file = fopen(username, "a");
						fclose(user_attempt_file);	
						user_attempt_file = fopen(username, "r");					// We open the file, creating it if it doesn't exist
						while(getline(&line, &len, user_attempt_file) != -1){
							strcpy(attempts, line);
							//printf("\nAttempts %s\n", attempts);
						}
						fclose(user_attempt_file);
						
						printf(ANSI_COLOR_RED "<SERVER>--FAILED AUTHENTICATION" ANSI_COLOR_NORMAL "\n");
						sendto( sock, "0",2, 0,(struct sockaddr *)  				// Failure to authenticate signal of 0 is sent
									&thisAddr, sizeof(clientAddr));
						if(!(strcmp(attempts, "") == 0)){
							num_tries = atoi(attempts);
							//printf("FILE Number of attempts: %d", num_tries);
						}
						num_tries ++;
						//printf("\nINT Number of attempts: %d", num_tries);
						
						/*  Keep track of number of attempts with total tries being 3 */
						if(2 - num_tries == 0){
							printf(ANSI_COLOR_RED "<SERVER>--FAILED AUTHENTICATION - LAST ATTEMPT TO LOG IN" ANSI_COLOR_NORMAL "\n");
							FILE * user_attempt_file = fopen(username, "w");				// User file updated with number of tries
							fprintf(user_attempt_file, "%d", num_tries);
							fclose(user_attempt_file);
						}else if(2 - num_tries < 0){
							FILE * user_attempt_file = fopen(username, "w");				// User file updated with number of tries
							fprintf(user_attempt_file, "%d", num_tries);
							fclose(user_attempt_file);
							
							printf(ANSI_COLOR_RED "<SERVER>--FAILED AUTHENTICATION - ACCOUNT LOCKED" ANSI_COLOR_NORMAL "\n");
							printf(ANSI_COLOR_GREY "<SERVER LOGGED> -- Error: Code 2 - Account Locked" ANSI_COLOR_NORMAL "\n");
							FILE * log = fopen("bin/log.txt", "a");
							
							if(log == NULL){										// Check to see if file is valid
								printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error 3: Could not open log file." ANSI_COLOR_NORMAL "\n");	// If not shut down server as cannot authenticate any users without file
							}
							time( &rawtime );
							time_info = localtime ( &rawtime ); 					// Get current time
							char t_info[40];
							strftime(t_info,40, "%D - %T", time_info);
							fprintf(log, "<SERVER SECURITY>--3 failed attempts for USERNAME: %s -- %s\n", username, t_info);
							
							fclose(log);
							
							FILE * locked_users = fopen("bin/locked.txt", "a");
							if(locked_users == NULL){								// Check to see if file is valid
								printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error: Code 7 - File could not be opened" ANSI_COLOR_NORMAL "\n");	// If not shut down server as cannot authenticate any users without file
							}
							fprintf(locked_users, "%s\n", username);
							fclose(locked_users);
							num_tries = 0;
						}else{
							FILE * user_attempt_file = fopen(username, "w");				// User file updated with number of tries
							fprintf(user_attempt_file, "%d", num_tries);
							fclose(user_attempt_file);
							printf(ANSI_COLOR_RED "<SERVER>--FAILED AUTHENTICATION - %d ATTEMPTS LEFT" ANSI_COLOR_NORMAL "\n", (3 - num_tries));
						}
					}
				}else{
					printf(ANSI_COLOR_RED "<SERVER ERROR>--failed to hash values..." ANSI_COLOR_NORMAL "\n");
					printf(ANSI_COLOR_GREY "<SERVER LOGGED> -- Error: Code 5 - Failed to hash values" ANSI_COLOR_NORMAL "\n");
					FILE * log = fopen("bin/log.txt", "a");
							
					if(log == NULL){										// Check to see if file is valid
						printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error 3: Could not open log file." ANSI_COLOR_NORMAL "\n");	// If not shut down server as cannot authenticate any users without file
					}
					time( &rawtime );
					time_info = localtime ( &rawtime ); 					// Get current time
					char t_info[40];
					strftime(t_info,40, "%D - %T", time_info);
					fprintf(log, "<SERVER SECURITY>--Error: Code 5 - Failed to hash values: %s -- %s\n", username, t_info);
							
					fclose(log);
				}
			}else{
				printf(ANSI_COLOR_RED "<SERVER ERROR>--User could not be found..." ANSI_COLOR_NORMAL "\n");
				printf(ANSI_COLOR_GREY "<SERVER LOGGED> -- Error: Code 4 - User could not be found" ANSI_COLOR_NORMAL "\n");
				sendto( sock, "fail",5, 0,(struct sockaddr *)  				// Fail signal is sent
									&thisAddr, sizeof(clientAddr));
				FILE * log = fopen("bin/log.txt", "a");
							
				if(log == NULL){											// Check to see if file is valid
					printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error 3: Could not open log file." ANSI_COLOR_NORMAL "\n");	// If not shut down server as cannot authenticate any users without file
				}
				time( &rawtime );
				time_info = localtime ( &rawtime ); 						// Get current time
				char t_info[40];
				strftime(t_info,40, "%D - %T", time_info);
				fprintf(log, "<SERVER SECURITY>--Error: Code 4 - User could not be found: %s -- %s\n", username, t_info);
							
				fclose(log);
			}
    		}else{
    			printf(ANSI_COLOR_RED "<SERVER ERROR>--User is locked out of account..." ANSI_COLOR_NORMAL "\n");
    			printf(ANSI_COLOR_GREY "<SERVER LOGGED> -- Possible Threat: Attempt to access locked account" ANSI_COLOR_NORMAL "\n");
    			sendto( sock, "locked",8, 0,(struct sockaddr *)  						// locked signal is sent
									&thisAddr, sizeof(clientAddr));
				FILE * log = fopen("bin/log.txt", "a");
							
				if(log == NULL){										// Check to see if file is valid
					printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error 3: Could not open log file." ANSI_COLOR_NORMAL "\n");	// If not shut down server as cannot authenticate any users without file
				}
				time( &rawtime );
				time_info = localtime ( &rawtime ); 					// Get current time
				char t_info[40];
				strftime(t_info,40, "%D - %T", time_info);
				fprintf(log, "<SERVER SECURITY>--Possible Threat: Attempt to access locked account: %s -- %s\n", username, t_info);
							
				fclose(log);
    		} // End of checking blocked
    	}
    	/*  Now wait for reply from client saying items received  */
		memset(buffer, 0, sizeof(buffer));								// Clear buffer
		recvMsgSize = recvfrom(sock, buffer, MAX, 0, (struct sockaddr *) &thisAddr, &size); //wait till we receive some msg from client
    	if ((recvMsgSize) < 0){
    		printf("recvfrom() failed\n");								// If receiving fails, error message is displayed
		}
		
		printf(ANSI_COLOR_BLUE "<SERVER>--Handling client request from address %s" ANSI_COLOR_NORMAL "\n", inet_ntoa(thisAddr.sin_addr)); //Display address of current requesting client
    	printf(ANSI_COLOR_MAGENTA "<SERVER>--Request from client : %s" ANSI_COLOR_NORMAL "\n", buffer);  // The request from client is printed to server
    
    	char *token;													// A token is created to go through arguments in clients request
		token = strtok(buffer, " ");										// First token taken from request as a prime
		int ctr = 0;													// Initialize a counter to zero before going through arguments
		memset(fl_name, 0, sizeof(fl_name));
		while(token != NULL){											// Here we separate arguments by spaces sent to server from the client
						if(ctr == 0){
							strcpy(cmd, token);							// The first token is always the command itself so we store that in cmd
						}
						if(ctr){										// If we are at the second argument we enter the if statement
							strcpy(fl_name, token);						// The second argument is the file name(in this case) or second parameter to the command
						}
						token = strtok(NULL, " ");						// Increment to null 
						ctr ++;											// Increment counter till no more tokens
		}
		/*	If the ls command is called	
			the server returns a list of
			files in the current working
			directory to the client. Note
			any file in the current working 
			directory can be retrieved by 
			the client. If a file is to not
			be accessed by the client, these
			files should be placed in the 
			bin directory, or another dir
			of a given choice */
		if(strcmp(cmd, "ls")==0){										// If we receive the ls command we display contents of current directory
			printf("display contents\n");
			if (getcwd(cwd, sizeof(cwd)) != NULL){
    			thisDir = opendir(cwd);									// Open the current working directory and store it in thisDir
    		}else {
    			printf("getcwd() error\n");
    		}
			while((myFile = readdir(thisDir)) != NULL){					// Read the list of current files/directories in working directory
    			sprintf(strBuff + strlen(strBuff),"%s/%s\n", cwd, myFile->d_name); // Add the list to the buffer to be sent to client
    			printf("%s/%s\n", cwd, myFile->d_name);
			}
			sendto(sock, strBuff, sizeof(strBuff), 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); // Buffer of listed items sent to Client
		}
		/*	When the get command is called 
			the fl_name argument will be used
			to determine the filename to 
			retrieve. If no filename, or the
			incorrect filename is provided,
			then an error message will be 
			produced and returned */
		else if(strcmp(cmd, "get")==0){								// If we receive the get command we retrieve the file for the client
			printf("Retrieve file\n");
			sendto(sock, "file", 5, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); // Send a notification that file is to be sent to client
			FILE *fl = fopen(fl_name,"r");								// Open the file to be read and sent to client
			if(fl == NULL){												// Verify the file to be correct
				printf("File could not be opened...\n");
				sendto(sock, "error", 6, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); // If an error we notify client, operation could not be completed
			}else{ 														// No errors occurred
				int file_block_size;
				while((file_block_size = fread(fileBuff, sizeof(char), FILE_MAX_SIZE, fl) > 0)) { // Verify and send file to Client
					sendto(sock, fl_name, sizeof(fl_name), 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); // File name is sent to Client
					int result = sendto(sock, fileBuff, sizeof(fileBuff), 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); // File itself is then sent to be saved on client with previous the previous lines filename
					if(result < 0){
						printf("Error: Could not send file to client...\n");	// If an error occurs it is displayed here
					}
				}
				printf("Success: Sent file to client!\n");						// When file send successfully , a success is displayed
				sendto(sock, "succeeded", 10, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); // Client is notified that all went well
			}
		}
		/*	When the quit command is called
			a user is logged out of the 
			server and the logout time is 
			recorded to the server log */
		else if(strcmp(cmd,"quit")==0 || strcmp(cmd, "exit") == 0){	// If client disconnects from server we wait for a new client to connect
			FILE * log = fopen("bin/log.txt", "a");
							
			if(log == NULL){											// Check to see if file is valid
				printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error 3: Could not open log file." ANSI_COLOR_NORMAL "\n");	// If not shut down server as cannot authenticate any users without file
			}
			time( &rawtime );
			time_info = localtime ( &rawtime ); 						// Get current time
			char t_info[40];
			strftime(t_info,40, "%D - %T", time_info);
			fprintf(log, "<SERVER NOTE>------User: %s Logged out -- %s\n", username, t_info);
			printf(ANSI_COLOR_GREEN "<SERVER>--User: %s Logged out -- %s" ANSI_COLOR_NORMAL "\n", username, t_info);
			fclose(log);
			printf("Waiting for new client to connect...\n");			// We wait for another client to connect
			authenticated = 0;
		}
		/*	The info command simply returns
			the basic info of the server */
		else if(strcmp(cmd, "info")==0){
			char *info = "<SERVER INFO>--Created By Jake Brown 2014\n";
			printf(ANSI_COLOR_GREEN "<SERVER INFO>--Created By Jake Brown 2014" ANSI_COLOR_NORMAL "\n");
			sendto(sock, info, 80, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); 	// Client is sent an info string
		}
		/*	This help command is called when
			no arguments have been supplied 
			and a simple help is returned with
			all available commands */
		else if((strcmp(cmd, "help")==0) && (strcmp(fl_name, "")==0)){ 					// fl_name holds a second argument
			char * help = "<SERVER HELP>\nAuthentication Server: Version 2.2 - Release <x86_64-apple-Darwin14>\nThese shell commands are defined within this server:\n\t-ls\n\t-get <FILENAME>\n\t-info\n\t-version\n\t-joke\n\t-add\n\t-remove\n\t-unlock\n\t-users";	
			printf(ANSI_COLOR_GREEN "<SERVER HELP>\nAuthentication Server: Version 2.0 - Release <x86_64-apple-Darwin14>\nThese shell commands are defined within this server:\n\t-ls\n\t-get <FILENAME>\n\t-info\n\t-version\n\t-joke\n\t-add\n\t-remove\n\t-unlock\n\t-users" ANSI_COLOR_NORMAL "\n");
			sendto(sock, help, 300, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); 	// Client is sent the list of known commands
		}
		/*	The version command simply returns
			the version of the server */
		else if(strcmp(cmd, "version")==0){
			char * version = "<SERVER VERSION>--Version 2.2\n";	
			printf(ANSI_COLOR_GREEN "<SERVER VERSION>--Version 2.2" ANSI_COLOR_NORMAL "\n");
			sendto(sock, version, 80, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); // Client is sent version number
		}
		/*	The joke command supplies a joke
			stored on the server. Could be 
			used to return a random joke from
			an array of stored jokes. */
		else if(strcmp(cmd, "joke")==0){	
			char * joke = "<SERVER JOKE>--So a man walks into a pharmacy wrapped in plastic wrap...\n The cashier says I can clearly see your nuts...\n";
			printf(ANSI_COLOR_GREEN "<SERVER JOKE>--So a man walks into a pharmacy wrapped in plastic wrap...\n The cashier says I can clearly see your nuts..." ANSI_COLOR_NORMAL "\n");
			sendto(sock, joke, 300, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); 	// Client is sent a joke
		}
		/*	The add command adds a user to the
			server database. The client is 
			prompted to supply a user name 
			and password, and is then added
			to the server database */
		else if(strcmp(cmd, "add")==0){		// Add a new user when requested
			char 	*line 		= NULL;											// Char array to store lines from file
			size_t 	len 		= 0; 
			int success 		= 0;
			int found 			= 0;
			memset(buffer, 0, sizeof(buffer));
			printf(ANSI_COLOR_GREEN "<SERVER ADD USER>--Opening file of users" ANSI_COLOR_NORMAL "\n");
			
			char * msgBuf = "<SERVER REQUEST>--Please provide new Users Username: ";
			sendto(sock, msgBuf, 80, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); 		// Client is sent the username request
			
			recvMsgSize = recvfrom(sock, buffer, MAX, 0, (struct sockaddr *) &thisAddr, &size); //wait till we receive the new Username from client
    		if ((recvMsgSize) < 0){
    			printf("recvfrom() failed\n");													// If receiving fails, error message is displayed
			}
			printf(ANSI_COLOR_MAGENTA "<SERVER>--Reply from client : %s" ANSI_COLOR_NORMAL "\n", buffer);
			char added_user[80];
			strcpy(added_user, buffer);
			
			/*	Once we receive the username from the client we should do a check to make
				sure that the username does not already exist in the database. If it does 
				we should ask the user to try a new username */
				
			FILE *chk_usrs = fopen("bin/users.txt", "r");										// We open the file containing user information in read mode
			if(chk_usrs == NULL){																// Check to see if file is valid
				printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error: Code 7 - no file found" ANSI_COLOR_NORMAL "\n");	// If not shut down server as cannot authenticate any users without file
				exit(1);
			}
			
			while(getline(&line, &len, chk_usrs) != -1){										// Search through the file looking for a match
				// we check for a match
				if(line[strlen(line) - 1] == '\n'){
					line[strlen(line) - 1] = '\0';
				}
				if((strcmp(line,added_user) == 0)){
					found = 1;																	// If a match is found we assign one to the found variable
				}	
			}	
				
			if(!found){																			// If the user is not already in the server database we will then continue to add the new user
				FILE *users = fopen("bin/users.txt", "a");										// We open the file containing user information in append mode to add new user and password
    		
    			if(users == NULL){																// Check to see if file is valid
					printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error: Code 7 - no file found" ANSI_COLOR_NORMAL "\n");	// If not shut down server as cannot authenticate any users without file
					exit(1);
				}
			
				/*  Check if successfully written to file */
				if(fprintf(users, "%s\n", buffer)){
					memset(buffer, 0, sizeof(buffer));
					msgBuf = "<SERVER REQUEST>--Please provide new Users Password: ";					// Now request password
					sendto(sock, msgBuf, 80, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); 		// Client is sent the request for password
				
					recvMsgSize = recvfrom(sock, buffer, MAX, 0, (struct sockaddr *) &thisAddr, &size); //wait till we receive the new password from client
    				if ((recvMsgSize) < 0){
    					printf("recvfrom() failed\n");													// If receiving fails, error message is displayed
					}
					printf(ANSI_COLOR_MAGENTA "<SERVER>--Reply from client : %s" ANSI_COLOR_NORMAL "\n", buffer);
					if(fprintf(users, "%s\n", buffer)){
						memset(buffer, 0, sizeof(buffer));
						FILE * log = fopen("bin/log.txt", "a");
							
						if(log == NULL){											// Check to see if file is valid
							printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error 3: Could not open log file." ANSI_COLOR_NORMAL "\n");				// If not shut down server as cannot authenticate any users without file
						}
						time( &rawtime );
						time_info = localtime ( &rawtime ); 						// Get current time
						char t_info[40];
						strftime(t_info,40, "%D - %T", time_info);
						fprintf(log, "<SERVER NOTE>------Added User: %s by %s -- %s\n", added_user, username, t_info);
						printf(ANSI_COLOR_GREEN "<SERVER NOTE>--Added User: %s by %s -- %s" ANSI_COLOR_NORMAL "\n", added_user, username, t_info);
						fclose(log);
						success = 1;
					}else{
						sendto(sock, "<SERVER ERROR>--Failed to write to file", 80, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); 		// Client is sent fail message
					}
				}else{
					sendto(sock, "<SERVER ERROR>--Failed to write to file", 80, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); 			// Client is sent fail message
				}
				if(success){
					sendto(sock, "<SERVER MESSAGE>--Succeeded adding new user....", 80, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); 	// Client is sent Complete message		
				}else{
					sendto(sock, "<SERVER ERROR>--Failed to create new user.", 80, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); 		// Client is sent fail message
				}
				fclose(users);	
			}else{																	// Username already exists and we notify the client
				printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error 9: Username already exists in database." ANSI_COLOR_NORMAL "\n");
				sendto(sock, "<SERVER ERROR>--Username already exists in database.", 80, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr));
			}			
		}
		/*	If a user has been locked due to a 
			forgotten password, an authenticated
			user can call this command to unlock
			that user or any other user that has 
			been locked out of their account */
		else if(strcmp(cmd, "unlock")==0){	
			char 	*line 		= NULL;											// Char array to store lines from file
			size_t 	len 		= 0; 
			int 	found		= 0;											// Keeps track if user was found or not
			
			memset(buffer, 0, sizeof(buffer));									// Clear the buffer
			
			char * msgBuf = "<SERVER REQUEST>--Please provide locked Users Username: ";			// Request the locked users username
			sendto(sock, msgBuf, 80, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr));
			
			recvMsgSize = recvfrom(sock, buffer, MAX, 0, (struct sockaddr *) &thisAddr, &size); // Wait till we receive the new Username from client
    		if ((recvMsgSize) < 0){
    			printf("recvfrom() failed\n");									// If receiving fails, error message is displayed
			}
			char locked_user[80]; 												// Name of locked user stored
			strcpy(locked_user, buffer);

			/*  Check to see if user name is locked  */
			FILE * locked = fopen("bin/locked.txt", "r");
			
			if(locked == NULL){													// Check to see if file is valid
				printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error: Code 7 - no Locked users file found" ANSI_COLOR_NORMAL "\n");	// Check if file is valid
				exit(1);
			}
			
			FILE * replica = fopen("bin/rep.txt", "w");
			if(replica == NULL){												// Check to see if file is valid
				printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error: Code 7 - no Locked users file found" ANSI_COLOR_NORMAL "\n");	// Check if file is valid
			}
			printf("List of locked users:\n");
			while(getline(&line, &len, locked) != -1){				
				// we check for a match
				if(line[strlen(line) - 1] == '\n'){
					line[strlen(line) - 1] = '\0';
				}
				if(!(strcmp(line,locked_user) == 0)){
					fprintf(replica,"%s\n", line);								// If the username doesn't match the file then write too new file
					printf("username: %s\n", line);
				}else{
					found = 1;
				}
			}
			fclose(locked);
			// We now have to delete the old file and rename the new to locked.txt
			
			fclose(replica);
			remove("bin/locked.txt");
			rename("bin/rep.txt", "bin/locked.txt");
			if(found){
				char * completed = "<SERVER MESSAGE>--User is no longer locked.";
				sendto(sock, completed, 80, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); 	// We send verification to the client
				FILE * log = fopen("bin/log.txt", "a");
							
				if(log == NULL){																	// Check to see if file is valid
					printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error 3: Could not open log file." ANSI_COLOR_NORMAL "\n");	// If not shut down server as cannot authenticate any users without file
				}
				time( &rawtime );
				time_info = localtime ( &rawtime ); 												// Get current time
				char t_info[40];
				strftime(t_info,40, "%D - %T", time_info);
				fprintf(log, "<SERVER NOTE>------Unlocked User: %s by %s -- %s\n", locked_user, username, t_info);
				printf(ANSI_COLOR_GREY "<SERVER NOTE> -- Unlocked User has been logged" ANSI_COLOR_NORMAL "\n");		
				fclose(log);
			}else{
				char * completed = "<SERVER MESSAGE>--No user by that name found in server database.";
				sendto(sock, completed, 80, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); 	// We send verification to the client
				FILE * log = fopen("bin/log.txt", "a");
							
				if(log == NULL){																	// Check to see if file is valid
					printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error 3: Could not open log file." ANSI_COLOR_NORMAL "\n");	// If not shut down server as cannot authenticate any users without file
				}
				time( &rawtime );
				time_info = localtime ( &rawtime ); 												// Get current time
				char t_info[40];
				strftime(t_info,40, "%D - %T", time_info);
				fprintf(log, "<SERVER NOTE>------Attempt to unlock unknown user: %s by %s -- %s\n", locked_user, username, t_info);
				printf(ANSI_COLOR_GREY "<SERVER NOTE> -- Attempt to unlock unknown user has been logged" ANSI_COLOR_NORMAL "\n");		
				fclose(log);
			}
			
			
		}
		/*	The remove command searches the server
			database for a given username and if
			found, removes that user form the 
			database, otherwise notifies the 
			client that no user was found with 
			the supplied username */
		else if(strcmp(cmd, "remove")==0){										// Remove a user from server
			// REMOVE A USER FROM THE SERVER
			char 	*line 		= NULL;											// Char array to store lines from file
			size_t 	len 		= 0; 
			int success 		= 0;
			char	passcode[80];
			memset(buffer, 0, sizeof(buffer));
			
			char * msgBuf = "<SERVER REQUEST>--Please provide Users Username to be deleted: ";
			sendto(sock, msgBuf, 80, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); 		// Client is sent the username request
			
			recvMsgSize = recvfrom(sock, buffer, MAX, 0, (struct sockaddr *) &thisAddr, &size); //wait till we receive the new Username from client
    		if ((recvMsgSize) < 0){
    			printf("recvfrom() failed\n");													// If receiving fails, error message is displayed
			}
			printf(ANSI_COLOR_MAGENTA "<SERVER>--Reply from client : %s" ANSI_COLOR_NORMAL "\n", buffer);
			char del_user[80];
			strcpy(del_user, buffer);
			
			printf(ANSI_COLOR_BLUE "<SERVER REMOVE USER>--Opening file of users" ANSI_COLOR_NORMAL "\n");
			
			FILE *users = fopen("bin/users.txt", "r");							// We open the file containing user information in append mode to add new user and password
    		
    		// Retrieve the password by searching the username
    		    		
    		if(users == NULL){													// Check to see if file is valid
				printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error: Code 7 - no file found" ANSI_COLOR_NORMAL "\n");	// If not shut down server as cannot authenticate any users without file
				exit(1);
			}
			
			FILE *repl	= fopen("bin/temp.txt", "w");							// We create a temporary file to store the updated user information	
    		if(users == NULL || repl == NULL){									// Check to see if file is valid
				printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error: Code 7 - no file found" ANSI_COLOR_NORMAL "\n");	// If not shut down server as cannot authenticate any users without file
				exit(1);
			}
			
			/*  We must search for the username now and obtain the correct password  */
			while(getline(&line, &len, users) != -1){
				// we check for a match
				// If found we then read the next line which will be the corresponding password
				if(line[strlen(line) - 1] == '\n'){
					line[strlen(line) - 1] = '\0';
				}
				if((strcmp(line,del_user) == 0)){
					printf(ANSI_COLOR_GREEN "<SERVER>--Found user. Getting password..." ANSI_COLOR_NORMAL "\n");
					getline(&line, &len, users);
					if(line[strlen(line) - 1] == '\n'){
						line[strlen(line) - 1] = '\0';
					}
				}else{ // We print the line to the new file
					fprintf(repl,"%s\n", line);									// If the username doesn't match the file then write too new file
				}
			}
			fclose(users);			
			fclose(repl);
			remove("bin/users.txt");
			rename("bin/temp.txt", "bin/users.txt");
			
			FILE * log = fopen("bin/log.txt", "a");
							
			if(log == NULL){													// Check to see if file is valid
				printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error 3: Could not open log file." ANSI_COLOR_NORMAL "\n");	// If not shut down server as cannot authenticate any users without file
			}
			time( &rawtime );
			time_info = localtime ( &rawtime ); 								// Get current time
			char t_info[40];
			strftime(t_info,40, "%D - %T", time_info);
			fprintf(log, "<SERVER NOTE>------Removed User: %s by %s -- %s\n", del_user, username, t_info);
			printf(ANSI_COLOR_GREY "<SERVER NOTE> --User has been Removed by %s" ANSI_COLOR_NORMAL "\n", username);		
			fclose(log);
			if(strcmp(del_user,username) == 0){
				sendto(sock, "exit", 5, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); // We send verification to the client
				FILE * log = fopen("bin/log.txt", "a");
							
				if(log == NULL){												// Check to see if file is valid
					printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error 3: Could not open log file." ANSI_COLOR_NORMAL "\n");	// If not shut down server as cannot authenticate any users without file
				}
				time( &rawtime );
				time_info = localtime ( &rawtime ); 							// Get current time
				char t_info[40];
				strftime(t_info,40, "%D - %T", time_info);
				fprintf(log, "<SERVER NOTE>------User Account: %s deleted and logged out -- %s\n", username, t_info);
				printf(ANSI_COLOR_GREEN "<SERVER>--User Account: %s deleted and logged out -- %s" ANSI_COLOR_NORMAL "\n", username, t_info);
				fclose(log);
				printf("Waiting for new client to connect...\n");				// We wait for another client to connect
				authenticated = 0;
			}else{
				char * completed = "<SERVER MESSAGE>--User has been removed from server.";
				sendto(sock, completed, 80, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); // We send verification to the client
			}
		}
		/*	The help command which takes an argument
			and returns the info on that command 
			that supplied as the argument. The arg
			is stored in a variable called fl_name */
		else if(strcmp(cmd, "help")==0 && !(strcmp(fl_name, "")==0)){			// fl_name will hold the second argument name
			// DO A SWITCH STATEMENT HERE
			char ret_string[300];
			if(strcmp(fl_name, "ls")==0){
				char * help = "<SERVER HELP>\nAuthentication Server: Version 2.2 - Release <x86_64-apple-Darwin14>\nThe 'ls' command is used to display a list of files in the working directory of the server.";
				strcpy(ret_string, help);
			}else if(strcmp(fl_name, "get")==0){
				char * help = "<SERVER HELP>\nAuthentication Server: Version 2.2 - Release <x86_64-apple-Darwin14>\nThe 'get' command is used to retrieve a file form the server.\nUse with form get <FILENAME>.";
				strcpy(ret_string, help);
			}else if(strcmp(fl_name, "info")==0){
				char * help = "<SERVER HELP>\nAuthentication Server: Version 2.2 - Release <x86_64-apple-Darwin14>\nThe 'info' command simply returns the info of the authentication server.";
				strcpy(ret_string, help);	
			}else if(strcmp(fl_name, "version")==0){
				char * help = "<SERVER HELP>\nAuthentication Server: Version 2.2 - Release <x86_64-apple-Darwin14>\nThe 'version' command simply returns the current version of the authentication server.";
				strcpy(ret_string, help);			
			}else if(strcmp(fl_name, "joke")==0){
				char * help = "<SERVER HELP>\nAuthentication Server: Version 2.2 - Release <x86_64-apple-Darwin14>\nThe 'joke' command returns the a joke.";
				strcpy(ret_string, help);
			}else if(strcmp(fl_name, "add")==0){
				char * help = "<SERVER HELP>\nAuthentication Server: Version 2.2 - Release <x86_64-apple-Darwin14>\nThe 'add' command takes the name and password of a new user to add to the server database.";
				strcpy(ret_string, help);
			}else if(strcmp(fl_name, "remove")==0){
				char * help = "<SERVER HELP>\nAuthentication Server: Version 2.2 - Release <x86_64-apple-Darwin14>\nThe 'remove' command removes a user from the server database.\nIf client wishes to remove themself they may do so but will be \nlogged out immediately";
				strcpy(ret_string, help);
			}else if(strcmp(fl_name, "unlock")==0){
				char * help = "<SERVER HELP>\nAuthentication Server: Version 2.2 - Release <x86_64-apple-Darwin14>\nThe 'unlock' command unlocks a user that has been previously locked\non the server.\nThis will grant them access to the server once again.";
				strcpy(ret_string, help);
			}else if(strcmp(fl_name, "users")==0){
				char * help = "<SERVER HELP>\nAuthentication Server: Version 2.2 - Release <x86_64-apple-Darwin14>\nThe 'user' command prints a list of all current users in the server database.";
				strcpy(ret_string, help);
			}else{
				char * help = "<SERVER HELP>\nAuthentication Server: Version 2.2 - Release <x86_64-apple-Darwin14>\nThese shell commands are defined within this server:\n\t-ls\n\t-get <FILENAME>\n\t-info\n\t-version\n\t-joke\n\t-add\n\t-remove\n\t-unlock\n\t-users";	
				printf(ANSI_COLOR_GREEN "<SERVER HELP>\nAuthentication Server: Version 2.0 - Release <x86_64-apple-Darwin14>\nThese shell commands are defined within this server:\n\t-ls\n\t-get <FILENAME>\n\t-info\n\t-version\n\t-joke\n\t-add\n\t-remove\n\t-unlock\n\t-users" ANSI_COLOR_NORMAL "\n");
				strcpy(ret_string, help);
			}
			sendto(sock, ret_string, sizeof(ret_string), 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); // Client is sent the list of known commands
			
		}
		
		/*	List all Users in the server database 
			uses files 'users.txt' located in the
			bin diretory */
		else if(strcmp(cmd, "users")==0){										// Display all users on the server
			char 	*line 		= NULL;											// Char array to store lines from file
			size_t 	len 		= 0; 
			FILE *users = fopen("bin/users.txt", "r");					
    		// Retrieve the password by searching the username
    		    		
    		if(users == NULL){													// Check to see if file is valid
				printf(ANSI_COLOR_RED "<SERVER ERROR> -- Error: Code 7 - no file found" ANSI_COLOR_NORMAL "\n");	// If not shut down server as cannot authenticate any users without file
				exit(1);
			}
			
			char user_list[1024];												// Buffer to hold all the user
			int ctr = 0;
			while(getline(&line, &len, users) != -1){							// Instead of sending file we just send a list of users
				if(ctr == 0){
					strcpy(user_list, line);									// If the first user we initialize the buffer
					getline(&line, &len, users);								// We do another getline to skip the users password
				}else{
					strcat(user_list, line);									// Every other user is concatenated to the buffer forming a list
					getline(&line, &len, users);								// We do another getline to skip the users password
				}
				ctr++;
			}
			strcat(user_list, "\nTotal Users: ");								// Add a note for total users
			char number_buff[10];
			sprintf(number_buff, "%d", ctr);									// Copy the number into a buffer so it can be added to the string
			strcat(user_list, number_buff);										// Append on the value for the total number of users
			
			sendto(sock, user_list, sizeof(user_list), 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); // We send list to the client	
			
		}else{
			char * cmd_not_found = "<SERVER ERROR>\nAuthentication Server: Version 2.2 - Release <x86_64-apple-Darwin14>\n--Command not found...\n";
			printf(ANSI_COLOR_RED  "<SERVER ERROR>\nAuthentication Server: Version 2.2 - Release <x86_64-apple-Darwin14>\n--Command not found..." ANSI_COLOR_NORMAL "\n");
			sendto(sock, cmd_not_found, 80, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); // Client is notification that the cmd was not found
		}
    }

}
