//////////////////////////////////////////////////////////
//	Name:			Jacob Brown	        //
//	Student #:		100 762 690             //
//	Course:			COMP3203                //	
//	EDITED:  		03 Oct 2014	        //
//	Title:			Authentication System   //        
//                                                      //
//	INCLUDED FILES:                                 //
//		- authentication_server.c	(UDP)   //
//		- pi_client.c                           //
//		- authentication.h                      //
//		- authentication.c                      //
//      - README.txt                                    //
//                                                      //
//  TOTAL:         5 files                              //
//  LINES OF CODE: 1,333                                //
//////////////////////////////////////////////////////////

INFORMAITION:

	The included is an example of an authentication server
	implementing an authentication protocol developed for
	learning purposes. The code can be improved to be used
	in real world applications. The server implements some
	very basic user commands as well as basic authentication
	similar to that of the Kerberos protocol. For more info
	rmation on the functions used in the protocol implemented
	in this server please review the header file found in 
	this directory, authentication.h.
	
	The use of this code is at the users discretion. I am
	not responsible for the loss of any information, nor 
	for any loss of data or infiltrations to machines
	implementing this code. Use at your own risk. Recom-
	mended for learning purposes only.

	Please read below on compiling of the code. All testing
	of the provided code has been ran on a Macbook Pro, 
	running OSX Yosemite 10.10.1. The machine contains
	16gb of ram, operating with a intel i7 quad-core
	processor. 
	
	If building on this project for your own use please
	include the following at the top of file headers to 
	give credit.
	
	/* JAKE BROWN 2014 - AUTHENTICATION - SERVER - CLIENT 
	   code developed and produced by Jake Brown of Ottawa
	   Canada part of the YellowBud company - Software dev
	   and more! http://www.yellowbudinc.com */
	
COMPILE SERVER:

gcc -o server authentication_server.c authentication.c

COMPILE CLIENT:

gcc -o client pi_client.c authentication.c

RUN SERVER EXAMPLE:

./server 70000

RUN CLIENT EXAMPLE:

./client 127.0.0.1 70000

