//////////////////////////////////////////////////////////
//	Name:			Jacob Brown							//
//	Student #:		100 762 690							//
//	Course:			COMP3203							//	
//	Title:		  	pi_client.c							//
//////////////////////////////////////////////////////////

#include <stdio.h>      											// for printf() and fprintf() 
#include <sys/socket.h> 											// for socket() and bind() 
#include <arpa/inet.h>  											// for sockaddr_in and inet_ntoa() 
#include <stdlib.h>     											// for atoi() and exit() 
#include <string.h>     											// for memset() 
#include <unistd.h>     											// for close() 
#include <sys/types.h>
#include <netinet/in.h>
#include "authentication.h"

#define MAX 1024     												// Longest string to echo 
#define ANSI_COLOR_RED 		"\x1b[31m"								// Red color for text
#define ANSI_COLOR_BLUE    	"\x1b[34m"								// Blue Color for text
#define ANSI_COLOR_GREEN   	"\x1b[32m"								// Green color for text
#define ANSI_COLOR_CYAN    	"\x1b[36m"								// Cyan Color for text
#define ANSI_COLOR_MAGENTA  "\x1b[35m"								// Magenta color for text
#define ANSI_COLOR_NORMAL   "\x1b[0m"								// Normal color for text
#define DOTS_10 			"................................................................................"
#define SPAC_10 			"                                                                                "

int main (int argc, char *argv[])
{
	int 			sock;											// Socket to be created
  	struct 			sockaddr_in  thisAddr, clientAddr;				// Struct for socket
  	unsigned int 	clientAddrSize;									// Integer to store size of address
  	unsigned short 	thisPort;     									// Server port
  	char			msgStr[MAX];									// Msg string for Commands
    char 			cBuff[MAX];        								// Buffer for strings passed back and fourth
    char 			rcvBuff[MAX];									// Buffer for receiving from server
    int 			msgSize;                						// Size of received message
    int				rcvMsgSize;										// Int to store the size of the msg received
    char 			*serverIp;                  					// IP address of server 
    int 			authenticated 	= 0;
    char 			fl_name[255];
    int				receivedTwo	 	= 0;
    
    
    if (argc != 3)         											// Here we verify the number of arguments to be used with running client
    {
        printf("Must include argument: <Server IP>[<Echo Port>]\n");
        exit(1);
    }
    
    serverIp = argv[1];           									//assigning ip of server to variable
    thisPort = atoi(argv[2]); 										//assigning the first argument to the port
    
	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	if(sock < 0){													// Verify socket was created successfully 
		printf("Failed to create socket..\n");
		exit(-1);
	}
	
    memset(&thisAddr, 0, sizeof(thisAddr));    
    thisAddr.sin_family = AF_INET;
    thisAddr.sin_addr.s_addr = inet_addr(serverIp);  
    thisAddr.sin_port = htons(thisPort);     						// htons used for sending to account for network bit order

	while(1){
		int 	salt;
		char 	key[32];
		char 	output[33];
				output[32] 	= 0x00;
		char 	pss[MAX];
		char 	password[32];
		char 	aKey[33];
				aKey[32]	= 0x00;
		int 	aSalt;	
		char 	username[MAX];
		int 	i 			= 0, j = 0;
		int 	failed 		= 0;
			
		
      	while(!authenticated){
      		memset(rcvBuff,0,sizeof(rcvBuff));							// reset the buffers each time loop is ran for new request to be sent
      		receivedTwo = 0;
      		system("clear");											// Clear the screan
      		printf(ANSI_COLOR_MAGENTA"********************************************************************************"ANSI_COLOR_NORMAL"\n");
      		printf(ANSI_COLOR_MAGENTA"*"ANSI_COLOR_CYAN"             AUTHENTICATION CLIENT BY JAKE BROWN - NOVEMBER 2014              "ANSI_COLOR_MAGENTA"*\n"); 
			printf(ANSI_COLOR_MAGENTA"********************************************************************************"ANSI_COLOR_NORMAL"\n");
			printf(ANSI_COLOR_MAGENTA"*"ANSI_COLOR_CYAN"             Please Log in... Case Sensitive                                  "ANSI_COLOR_MAGENTA"*\n");
			printf(ANSI_COLOR_MAGENTA"********************************************************************************"ANSI_COLOR_NORMAL"\n");
			printf(ANSI_COLOR_MAGENTA"*"ANSI_COLOR_RED"             NOTE: If not a user please contact an existing member            "ANSI_COLOR_MAGENTA"*\n");
			printf(ANSI_COLOR_MAGENTA"*"ANSI_COLOR_RED"                   to register you to the server.                             "ANSI_COLOR_MAGENTA"*\n");
			printf(ANSI_COLOR_MAGENTA"********************************************************************************"ANSI_COLOR_NORMAL"\n");
			printf(ANSI_COLOR_BLUE"<CLIENT>--Username:"ANSI_COLOR_NORMAL"\t"); // Ask for username
			fgets(msgStr, sizeof(msgStr), stdin);
    		msgStr[strlen(msgStr)-1] = 0;		
      		strcpy(cBuff, msgStr);										// msgStr is then loaded into a buffer
      		strcpy(username, msgStr);
      		
      		printf(ANSI_COLOR_BLUE"<CLIENT>--Password:"ANSI_COLOR_NORMAL"\t"); // Ask for password
      		fgets(pss, sizeof(pss), stdin);								
      		strcpy(password, pss);										// password is then loaded into a var named password
      		char *newPass = password;
      		
      		sendto(sock, cBuff, strlen(cBuff), 0,						// the buffer is then send across the socket to the server to be processed
      				(struct sockaddr *) &thisAddr, sizeof(thisAddr));
      		int ctr = 0;
      		while(!receivedTwo){
      			memset(rcvBuff,0,sizeof(rcvBuff));							// reset the buffers each time loop is ran for new request to be sent
      			if ((rcvMsgSize = recvfrom(sock, rcvBuff, MAX, 0,			// here we wait for any incoming data from the server
            		(struct sockaddr *) &thisAddr, &clientAddrSize)) < 0)
            		printf("recvfrom() failed\n");							// Handle is any errors occur during the receive
            	if((strcmp(rcvBuff, "fail") == 0)){
        			printf("received fail message...\n");
        			receivedTwo = 1;
        			failed = 1;												// Failed to find user
        			sleep(1);
        		}
        		if((strcmp(rcvBuff, "locked") == 0)){						// Account locked
        			printf("THIS USER ACCOUNT HAS BEEN LOCKED. CONTACT ADMIN USER. FLAG RAISED!\n");
        			printf("|  ||||||||||||||||||||||||"ANSI_COLOR_RED"||||||||||||"ANSI_COLOR_NORMAL"|||||||||||||||||||||||||\n");
      			    printf("|  ||||||||||||||||||"ANSI_COLOR_RED"|||||||||||||||||||||||||"ANSI_COLOR_NORMAL"||||||||||||||||||\n");
      				printf("|  ||||||||||||||||||"ANSI_COLOR_RED"|||||||||||||||||||||||||"ANSI_COLOR_NORMAL"||||||||||||||||||\n");
                    printf("|  ||||||||||||||||||"ANSI_COLOR_RED"||||   |||||||||||   ||||"ANSI_COLOR_NORMAL"||||||||||||||||||\n");
                    printf("|  ||||||||||||||||||"ANSI_COLOR_RED"||||   |||||||||||   ||||"ANSI_COLOR_NORMAL"||||||||||||||||||\n");
                    printf("|  ||||||||||||||||||"ANSI_COLOR_RED"|||||||||||||||||||||||||"ANSI_COLOR_NORMAL"||||||||||||||||||\n");
                    printf("|  ||||||||||||||||||||"ANSI_COLOR_RED"||||||||| || ||||||||||"ANSI_COLOR_NORMAL"||||||||||||||||||\n");
                    printf("|  |||||||||||||||||||||"ANSI_COLOR_RED"||||||| _||_ ||||||||"ANSI_COLOR_NORMAL"|||||||||||||||||||\n"); 
                    printf("|  ||||||||||||||||||||||||"ANSI_COLOR_RED"|||||||||||||"ANSI_COLOR_NORMAL"||||||||||||||||||||||||\n");
                    printf("|  ||||||||||||||||||||||||"ANSI_COLOR_RED"|||||||||||||"ANSI_COLOR_NORMAL"||||||||||||||||||||||||\n");
                    printf("|  ||||||||||||||||||||||||"ANSI_COLOR_RED"| | | | | | |"ANSI_COLOR_NORMAL"||||||||||||||||||||||||\n");
                    printf("|  ||||||||||||||||||||||||"ANSI_COLOR_RED"!LOCKED--OUT!"ANSI_COLOR_NORMAL"||||||||||||||||||||||||\n");
                    printf("|  ||||||||||||||||||||||||"ANSI_COLOR_RED"|||||||||||||"ANSI_COLOR_NORMAL"||||||||||||||||||||||||\n");
                    printf("|  |\n");
                    printf("|  |\n");
                    printf("|  |\n");
                    printf("|  |\n");
                    printf("|  |\n");
                    printf("|  |\n");
                    printf("|__|\n");
                    
        			receivedTwo = 1;
        			failed = 1;												
        			sleep(5);
        		}
        		if(ctr == 0){												// key is received
        			strncpy(aKey, rcvBuff, sizeof(char)*32);
        		}else if(ctr == 1){											// salt is received
        			sscanf(rcvBuff, "%d", &aSalt);
        		}else if((strcmp(rcvBuff, "finished") == 0)){
        			receivedTwo = 1;										// we have received all we need to for hash function
        			failed = 0;
        		}
        		ctr++;
        		//printf("reply from server:\n%s\n", rcvBuff);				// The reply from the server is display for clients convenience
        	}
        	if(newPass[strlen(newPass) - 1] == '\n'){
					newPass[strlen(newPass) - 1] = '\0';
			}
			
			for(i = 1; i < 80; i++){
				for(j = 0; j < 10000000; j++);
				printf(ANSI_COLOR_BLUE"%1.*s", i, DOTS_10);
        		printf("%1.*s\n\r", 80-i, SPAC_10);  
			}
			if(!failed){
        		int result = hash_value(newPass,output,aKey,&aSalt);			// Here we has the provided values with our password
        		if(result){
        			sendto(sock, output, sizeof(char)*32, 0,						// The hashed result is then send across the socket to the server to be processed
      					(struct sockaddr *) &thisAddr, sizeof(thisAddr));
      				// after the result is sent we wait for a reply form the server saying whether they match or dont
      				// If they match we authenticate, If not we dont
      				memset(rcvBuff,0,sizeof(rcvBuff));							// reset the buffers each time loop is ran for new request to be sent
      				if ((rcvMsgSize = recvfrom(sock, rcvBuff, MAX, 0,			// here we wait for any incoming result from the server
            			(struct sockaddr *) &thisAddr, &clientAddrSize)) < 0)
            			printf("recvfrom() failed\n");							// Handle is any errors occur during the receive
            		if(strcmp(rcvBuff, "1")==0){								// If true we have been authenticated
            			authenticated = 1;
            			system("clear");
            			printf(ANSI_COLOR_GREEN"\n******************************************************************************" ANSI_COLOR_NORMAL "\n");
            		  	printf(ANSI_COLOR_GREEN"*" ANSI_COLOR_BLUE"           **   **   ** ****  **     ****    **    **     **  ****          "ANSI_COLOR_GREEN"*"   ANSI_COLOR_NORMAL "\n");
            		  	printf(ANSI_COLOR_GREEN"*" ANSI_COLOR_BLUE"            **  **  **  **    **    **     **  **  **** ****  **            "ANSI_COLOR_GREEN"*"   ANSI_COLOR_NORMAL "\n");
            		  	printf(ANSI_COLOR_GREEN"*" ANSI_COLOR_BLUE"             ***  ***   ****  **   **     **    ** ** *** **  ****          "ANSI_COLOR_GREEN"*"   ANSI_COLOR_NORMAL"\n");
            		  	printf(ANSI_COLOR_GREEN"*" ANSI_COLOR_BLUE"              **  **    **    **    **     **  **  **  *  **  **            "ANSI_COLOR_GREEN"*"   ANSI_COLOR_NORMAL"\n");
            		  	printf(ANSI_COLOR_GREEN"*" ANSI_COLOR_BLUE"               *  *     ****  ****   ****    **    **     **  ****          "ANSI_COLOR_GREEN"*"   ANSI_COLOR_NORMAL"\n");
            		  	printf(ANSI_COLOR_GREEN"******************************************************************************"   ANSI_COLOR_NORMAL"\n");
            		  	printf(ANSI_COLOR_GREEN"*" ANSI_COLOR_BLUE"              Welcome to a server by Jake Brown 2014 - 2015                 "ANSI_COLOR_GREEN"*"   ANSI_COLOR_NORMAL"\n");
            		  	printf(ANSI_COLOR_GREEN"*" ANSI_COLOR_BLUE"              Version: 002                                                  "ANSI_COLOR_GREEN"*"   ANSI_COLOR_NORMAL"\n");
            		  	printf(ANSI_COLOR_GREEN"*" ANSI_COLOR_BLUE"              Authentication Protocol: Like Kerberos                        "ANSI_COLOR_GREEN"*"   ANSI_COLOR_NORMAL"\n");
            		  	printf(ANSI_COLOR_GREEN"******************************************************************************"   ANSI_COLOR_NORMAL"\n");
            		} else{
            			authenticated = 0;
            		}
      			
        		}
        	}
        }
        
        memset(rcvBuff,0,sizeof(rcvBuff));							// reset the buffers each time loop is ran for new request to be sent
		memset(cBuff, 0, sizeof(cBuff));							// reset the buffers each time loop is ran for new request to be sent
		
		printf("%s:->", username);									// User is prompted to enter a command to be sent to server
		fgets(msgStr, sizeof(msgStr), stdin);
    	msgStr[strlen(msgStr)-1] = 0;								// after using fgets to retrieve input, it is stored in msgStr to be used
    	
    	strcpy(cBuff, msgStr);										// msgStr is then loaded into a buffer
    	sendto(sock, cBuff, strlen(cBuff), 0,						// the buffer is then send across the socket to the server to be processed
      	(struct sockaddr *) &thisAddr, sizeof(thisAddr));
      	
    	if (strcmp(msgStr, "quit") == 0 || strcmp(msgStr, "exit") == 0){	// if the command quit is typed client closes connections and stops running
      		authenticated = 0;
      		for(i = 1; i < 80; i++){
				for(j = 0; j < 10000000; j++);
				printf(ANSI_COLOR_RED"%1.*s", i, DOTS_10);
        		printf("%1.*s\n\r", 80-i, SPAC_10);  
			}
      	}
		if(authenticated){
    		if ((rcvMsgSize = recvfrom(sock, rcvBuff, MAX, 0,			// here we wait for any incoming data from the server
            	(struct sockaddr *) &thisAddr, &clientAddrSize)) < 0)
            	printf("recvfrom() failed\n");							// Handle is any errors occur during the receive
        
        	printf("reply from server:\n"ANSI_COLOR_MAGENTA"%s"ANSI_COLOR_NORMAL"\n", rcvBuff);				// The reply from the server is display for clients convenience
        }
        if(strcmp(rcvBuff, "file")==0){								// Here we check to see if the reply is letting us know a file is to be recieved
        	printf("Waiting for file from server...\n");
        	while(1){
        		if ((rcvMsgSize = recvfrom(sock, rcvBuff, MAX, 0, (struct sockaddr *) &thisAddr, &clientAddrSize)) < 0) // Data received from server comes here
            		printf("recvfrom() failed\n");
            	printf("Notes form server: %s\n", rcvBuff);			// Any notes before server sends the file are received and displayed to client
            	if(strcmp(rcvBuff, "error")==0){					// Check to see if an error was produced
            		printf("Error: Host could not find file...\n");	// If an error, this message is displayed to the client
            		break;
            	}else if(strcmp(rcvBuff, "succeeded")==0){ 			// After a file has been received successfully we enter this if statement to complete transaction
            		printf("Success: received file!\n");
            		break;											// we break out of the loop and client is asked again for farther commands
            	}else {												// If no errors and fille has not yet been processed we then receive the file and process it
            		strcpy(fl_name, rcvBuff);
            		FILE *fl = fopen(fl_name,"a");					// Open a file to be written to for the receiving file
            		int block_size;
            		block_size = recvfrom(sock, rcvBuff, MAX, 0, (struct sockaddr *) &thisAddr, &clientAddrSize); // File is received
            		if(block_size > 0){								// Once again check size to makesure correct 
            			printf("File received\n%s",rcvBuff);		// File is printed to screen for client to view
            			int write_size = fwrite(rcvBuff, sizeof(char), block_size, fl); // File is also saved to file we opened earlier
            			if(write_size < block_size){				// If an error occurred durring the writing process we catch that here
            				printf("Error: Failed to write file...\n");
            			}
            		}else{											// The error displayed when the size of the file is less than 0 meaning incorrect from server
            			printf("error\n");
            		}
            		fclose(fl);										// File transfer process complete and we close the file
            	}		
        	}
        }
    	if(strcmp(rcvBuff, "exit")==0){								// Client has been logged out by the server
    		authenticated = 0;
      		for(i = 1; i < 80; i++){
				for(j = 0; j < 10000000; j++);
				printf(ANSI_COLOR_RED"%1.*s", i, DOTS_10);
        		printf("%1.*s\n\r", 80-i, SPAC_10);  
			}
    	}	
      
	}

	close(sock);													// Close socket and end program
	return(0);
}