//////////////////////////////////////////////////////////
//	Name:			Jacob Brown							//
//	Student #:		100 762 690							//
//	Course:			COMP3203							//	
//	Assignment:  	Number 1 client/Server	03 Oct 2014	//
//////////////////////////////////////////////////////////

#include <stdio.h>      						// for printf() and fprintf() 
#include <sys/socket.h> 						// for socket() and bind() 
#include <arpa/inet.h>  						// for sockaddr_in and inet_ntoa() 
#include <stdlib.h>     						// for atoi() and exit() 
#include <string.h>     						// for memset() 
#include <unistd.h>     						// for close() 
#include <sys/types.h>							//
#include <netinet/in.h>							// 
#include <time.h>								// for time variables
#include <dirent.h>								// for files
#include <sys/stat.h>							// for specs of files
#include <string.h>								// for string manipulation
#include "authentication.h"						// for authentication protocols

#define MAX 2048     							// Longest string to echo 
#define FILE_MAX_SIZE 4096						// Largest file size

int main (int argc, char *argv[])
{
	int 			sock;								// The Socket
  	struct 			sockaddr_in  thisAddr, clientAddr;	// Socket struct
  	struct			dirent *myFile;						// Struct used for when displaying files in directory
  	struct 			stat myStat;						// Struct for using stats for files
  	unsigned int 	size;								// Stores size of received messages
  	fd_set 			readfds;					
  	unsigned short 	thisPort;     						// Server port
    char 			buffer[MAX];        				// Buffer for echo string
    char			fileBuff[FILE_MAX_SIZE];			// File Buffer
    char			strBuff[MAX];						// String buffer
    int 			recvMsgSize;                		// Size of received message
    DIR 			*thisDir;							// Working Directory for server
    //char			cwd[MAX];							// Stores the Current working Directory
    char 			fl_name[255];						// File name is stored in this buffer
    char 			cmd[5];								// Small buffer for commands from client
    int 			authenticated = 0;						// set to 1 when true
    
    
    if (argc != 2)         										// We verify the number of arguments inputed when starting the server
    {
        printf("Must include argument: <UDP SERVER PORT>");		// Error message if incorrect amount of arguments
        exit(1);
    }
    
    thisPort = atoi(argv[1]); 									// Assigning the first argument to the port
    
    
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);			// Creation of the socket
    if(sock < 0) {												// Verify it was created correctly
    	printf("Failed to open the server socket\n");			// Error message displayed if not created properly
    	exit(-1);
    }
    
   
    memset(&thisAddr, 0, sizeof(thisAddr));   					// Here the address is set up and memory is allocated
    thisAddr.sin_family = AF_INET;                
    thisAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    thisAddr.sin_port = htons(thisPort);  						// Use of htons for network bit order
    
    if (bind(sock, (struct sockaddr *) &thisAddr, sizeof(thisAddr)) < 0){ // Bind the sockets and check for success
    	printf("Failed to bind socket\n");						// Error message displayed of fails to Bind the socket
    	exit(-1);												// Program ends
    }
    
    printf("waiting for interaction from client...\n");			// Once socket created and bound we wait for a client to connect
    
    /*  First loop to handle authorizing a user  */
    while(!authenticated){
    	memset(buffer, 0, sizeof(buffer));						// Clear buffer
    	authenticated = 0;
    	char output[33];
    	output[32] = 0x00;
    	char aKey[33];
		aKey[32] = 0x00;
		int aSalt;
		int i,ctr;
		char *time;
    	char *salt_str;
    	int found = 0;
    	size = sizeof(thisAddr); 								//The size of the clients address is stored here
    	
    	recvMsgSize = recvfrom(sock, buffer, MAX, 0, (struct sockaddr *) &thisAddr, &size); //wait till we receive a rqst msg from client
    	if ((recvMsgSize) < 0){
    		printf("recvfrom() failed\n");						// If receiving fails, error message is displayed
		}
		
		printf("Handling client request from address %s\n", inet_ntoa(thisAddr.sin_addr)); //Display address of current requesting client
		printf("Request client : %s\n", buffer); 		    	// Here we should see the printed request message ie/ a username
		
		/*  We must now search the file of usernames for the password
		    Once the password is found we will run our authentication
			protocol and send the result back to the client.           */
		FILE *passwords = fopen("users.txt", "r");			// We open the file containing user information to search for passwords
		
		if(passwords == NULL){									// Check to see if file is valid
			printf("ERROR: Code 7 - no file found\n");			// If not shut down server as cannot authenticate any users without file
			exit(1);
		} else{													// Here we actually check the file for the user
			 char * line = NULL;
			 size_t len = 0;
			 
			/*  We must search for the username now and obtain the correct password  */
			while(getline(&line, &len, passwords) != -1){
				// we check for a match
				// If found we then read the next line which will be the corresponding password
				if(line[strlen(line) - 1] == '\n'){
					line[strlen(line) - 1] = '\0';
				}
				//printf("User : %s -- %s", line, buffer);
				if((strcmp(line,buffer) == 0)){
					found = 1;
					getline(&line, &len, passwords);
					/*  &line now holds the appropriate password to 
						authenticate with and hash to send to the 
						client. Output will contain the hashed value
						that will be sent to the client along with
						key and salt  */ 
					printf("The password is: %s\n", line);
					int result = hash_value(line, output, aKey, &aSalt); 
					if(result){									// If result is equal to 1 then the hash worked successfully 
						/*  Since the hash worked correctly we 
							then send the info along to the 
							client  */
							printf("Hashed Value: ");
							int ctr = 0;
							for(i = 0; i < 32; i ++){
								printf("%c", output[i]);
								if(ctr == 7 && i != 32 - 1){
									printf("-");
									ctr = 0;
								}else{
									ctr++;
								}
							}
							printf("\n");
							sprintf(salt_str, "%d", aSalt);									// We convert salt value to string
			
							sendto( sock, aKey, sizeof(char)*32, 0,(struct sockaddr *)  	// key value is sent
									&thisAddr, sizeof(clientAddr)); 
							printf("Sent key value: %s\n", aKey);
							sendto( sock, salt_str, strlen(salt_str), 0,(struct sockaddr *) // salt value is sent
									&thisAddr, sizeof(clientAddr)); 
							printf("Sent salt value: %s\n", salt_str);
							sendto( sock, "finished",9, 0,(struct sockaddr *)  				// finished signal is sent
									&thisAddr, sizeof(clientAddr));
									
							// Now we want to wait to see if we receive the same hashed value
							memset(buffer, 0, sizeof(buffer));						// Clear buffer
							recvMsgSize = recvfrom(sock, buffer, MAX, 0, (struct sockaddr *) &thisAddr, &size); //wait till we receive some msg from client
    						if ((recvMsgSize) < 0){
    							printf("recvfrom() failed\n");						// If receiving fails, error message is displayed
							}
							printf("Hashed Value received: ");
							ctr = 0;
							for(i = 0; i < 32; i ++){
								printf("%c", buffer[i]);
								if(ctr == 7 && i != 32 - 1){
									printf("-");
									ctr = 0;
								}else{
									ctr++;
								}
							}
							printf("\n");
							// Buffer should contain the hashed value to compare to the servers
							if(compare_hash_values(output,buffer)){					// If the result is 1 then they match
								authenticated = 1;									// Assign 1 to authentication Boolean
								printf("Confirmed both hash values match.\n");
								printf("Authenticating User...\n");
								sendto( sock, "1",2, 0,(struct sockaddr *)  				// send 1 for authenticated bit
									&thisAddr, sizeof(clientAddr));
								printf("User has been authenticated.\n");
							}else {
								sendto( sock, "0",2, 0,(struct sockaddr *)  				// send 0 for failed authenticated bit
									&thisAddr, sizeof(clientAddr));
							}
							
					} else{
						printf("Error: Code 5 - problem hashing\n");
						exit(1);
						// Do something to handle error
					}
				} 
			}
			if(found == 0){
				printf("\nNo user found\n");
			}
			break;
		}
		
    	
    } // End of the authentication loop. If authentication != 1 then we try again
    
    
    
    /* Here we must check if a user is authenticated before allowing them in */
    while(authenticated){										// The infinite loop to run the server begins
    	memset(buffer, 0, sizeof(buffer));						// Clear buffer
    	memset(fileBuff, 0, sizeof(fileBuff));  				// Clear fileBuff
    	memset(strBuff, 0, sizeof(strBuff));   					// Clear strBuff
    	memset(cmd, 0, sizeof(cmd));   							// Clear cmd
    	char			cwd[MAX];							// Stores the Current working Directory
    	
    	size = sizeof(thisAddr); 								//The size of the clients address is stored here
    	
    	recvMsgSize = recvfrom(sock, buffer, MAX, 0, (struct sockaddr *) &thisAddr, &size); //wait till we receive some msg from client
    	if ((recvMsgSize) < 0){
    		printf("recvfrom() failed\n");						// If receiving fails, error message is displayed
		}
		
		printf("Handling client request from address %s\n", inet_ntoa(thisAddr.sin_addr)); //Display address of current requesting client
		printf("Request from client : %s\n", buffer); 			// The request from client is printed to server
		
		char *token;											// A token is created to go through arguments in clients request
		token = strtok(buffer, " ");							// First token taken from request as a prime
		int ctr = 0;											// Initialize a counter to zero before going through arguments
		
		while(token != NULL){									// Here we separate arguments by spaces sent to server from the client
						printf("Token name : %s\n%d\n", token,ctr); // We display the token on the server 
						if(ctr == 0){
							strcpy(cmd, token);					// The first token is always the command itself so we store that in cmd
						}
						if(ctr){								// If we are at the second argument we enter the if statement
							strcpy(fl_name, token);				// The second argument is the file name(in this case) or second parameter to the command
							printf("filename is: %s\n", fl_name); // The file name is displayed to server
						}
						token = strtok(NULL, " ");				// Increment to null 
						ctr ++;									// Increment counter till no more tokens
					}
					
		if(strcmp(cmd, "ls")==0){								// If we receive the ls command we display contents of current directory
			printf("display contents\n");
			if (getcwd(cwd, sizeof(cwd)) != NULL){
    			thisDir = opendir(cwd);							// Open the current working directory and store it in thisDir
    		}else {
    			printf("getcwd() error\n");
    		}
			while((myFile = readdir(thisDir)) != NULL){			// Read the list of current files/directories in working directory
    			sprintf(strBuff + strlen(strBuff),"%s/%s\n", cwd, myFile->d_name); // Add the list to the buffer to be sent to client
    			printf("%s/%s\n", cwd, myFile->d_name);
			}
			sendto(sock, strBuff, sizeof(strBuff), 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); // Buffer of listed items sent to Client
		}else if(strcmp(cmd, "get")==0){						// If we receive the get command we retrieve the file for the client
			printf("Retrieve file\n");
			sendto(sock, "file", 5, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); // Send a notification that file is to be sent to client
			FILE *fl = fopen(fl_name,"r");						// Open the file to be read and sent to client
			if(fl == NULL){										// Verify the file to be correct
				printf("File could not be opened...\n");
				sendto(sock, "error", 6, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); // If an error we notify client, operation could not be completed
			}else{ 												// No errors occurred
				int file_block_size;
				while((file_block_size = fread(fileBuff, sizeof(char), FILE_MAX_SIZE, fl) > 0)) { // Verify and send file to Client
					sendto(sock, fl_name, sizeof(fl_name), 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); // File name is sent to Client
					int result = sendto(sock, fileBuff, sizeof(fileBuff), 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); // File itself is then sent to be saved on client with previous the previous lines filename
					if(result < 0){
						printf("Error: Could not send file to client...\n");	// If an error occurs it is displayed here
					}
				}
				printf("Success: Sent file to client!\n");		// When file send successfully , a success is displayed
				sendto(sock, "succeeded", 10, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); // Client is notified that all went well
			}
			
			
		}else if(strcmp(cmd,"quit")==0 || strcmp(cmd, "exit") == 0){	// If client disconnects from server we wait for a new client to connect
			printf("Waiting for new client to connect...\n");
		}
		
    }
    
    
    close(sock);												// Close the socket before terminating the program
    return(0);
        
}
//////////////////////////////////////////////////////////
//	Name:			Jacob Brown							//
//	Student #:		100 762 690							//
//	Course:			COMP3203							//	
//	Assignment:  	Number 1 client/Server	03 Oct 2014	//
//////////////////////////////////////////////////////////

#include <stdio.h>      						// for printf() and fprintf() 
#include <sys/socket.h> 						// for socket() and bind() 
#include <arpa/inet.h>  						// for sockaddr_in and inet_ntoa() 
#include <stdlib.h>     						// for atoi() and exit() 
#include <string.h>     						// for memset() 
#include <unistd.h>     						// for close() 
#include <sys/types.h>							//
#include <netinet/in.h>							// 
#include <time.h>								// for time variables
#include <dirent.h>								// for files
#include <sys/stat.h>							// for specs of files
#include <string.h>								// for string manipulation
#include "authentication.h"						// for authentication protocols

#define MAX 2048     							// Longest string to echo 
#define FILE_MAX_SIZE 4096						// Larn", inet_ntoa(thisAddr.sin_addr)); //Display address of current requesting client
		printf("Request client : %s\n", buffer); 		    	// Here we should see the printed request message ie/ a username
		
		/*  We must now search the file of usernames for the password
		    Once the password is found we will run our authentication
			protocol and send the result back to the client.           */
		FILE *passwords = fopen("users.txt", "r");			// We open the file containing user information to search for passwords
		
		if(passwords == NULL){									// Check to see if file is valid
			printf("ERROR: Code 7 - no file found\n");			// If not shut down server as cannot authenticate any users without file
			exit(1);
		} else{													// Here we actually check the file for the user
			 char * line = NULL;
			 size_t len = 0;
			 
			/*  We must search for the username now and obtain the correct password  */
			while(getline(&line, &len, passwords) != -1){
				// we check for a match
				// If found we then read the ne
						exit(1);
						// Do something to handle error
					}
				} 
			}
			if(found == 0){
				printf("\nNo user found\n");
			}
			break;
		}
		
    	
    } // End of the authentication loop. If authentication != 1 then we try again
    
    
    
    /* Here we must check if a user is authenticated before allowing them in */
    while(authenticated){										// The infinite loop to run the server begins
    	memset(buffer, 0, sizeof(buffer));						// Clear buffer
    	memset(fileBuff, 0, sizeof(fileBuff));  				// Clear fileBuff
    	memset(strBuff, 0, sizeof(strBuff));   					// Clear strBuff
    	memset(cmd, 0, sizeof(cmd));   							// Clear cmd
    	char			cwd[MAX];							// Stores the Current working Directory
    	
    	size = sizeof(thisAddr); 								//The size of the clients address is stored here
    	
    	recvMsgSize = recvfrom(sock, buffer, MAX, 0, (struct sockaddr *) &thisAddr, &size); //wait till we receive some msg from client
    	if ((recvMsgSize) < 0){
    		printf("recvfrom() failed\n"fileBuff), 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); // File itself is then sent to be saved on client with previous the previous lines filename
					if(result < 0){
						printf("Error: Could not send file to client...\n");	// If an error occurs it is displayed here
					}
				}
				printf("Success: Sent file to client!\n");		// When file send successfully , a success is displayed
				sendto(sock, "succeeded", 10, 0,(struct sockaddr *) &thisAddr, sizeof(clientAddr)); // Client is notified that all went well
			}
			
			
		}else if(strcmp(cmd,"quit")==0 || strcmp(cmd, "exit") == 0){	// If client disconnects from server we wait for a new client to connect
			printf("Waiting for new client to connect...\n");
		}
		
    }
    
    
    close(sock);												// Close the socket before terminating the program
    return(0);
        
}
sock, buffer, MAX, 0, (struct sockaddr *) &thisAddr, &size); //wait till we receive some msg from client
    	if ((recvMsgSize) < 0){
    		printf("recvfrom() failed\n"