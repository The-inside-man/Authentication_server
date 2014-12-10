//////////////////////////////////////////////////////////
//	Name:			Jacob Brown							//
//	Student #:		100 762 690							//
//	Course:			COMP3203							//	
//	Title:			Authentication like KERBEROS Source //
//	Date:			31 October 2014						//
//////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
//															//
//	Description:	This is a KERBEROS like implementation 	//
//					developed by student, Jacob Brown, at 	//
//					Carleton University. KERBEROS was dev-	//
//					eloped by MIT. This implementation us-	//	
//					es sha256 for the hashing algorithm a-	//
//					nd BBC as the encryption for salt val-	//
//					ues and any information needed to be 	//
//					encrypted. Developed for COMP3203 		//
//					Fundamentals of computer networks.		//
//															//
//////////////////////////////////////////////////////////////

#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H


/* 	Generates a random one time use 
	salt value. Takes no arguments. */
int generate_salt();

/*	Computes the hash value of some input
	and returns the output hash value. 
	Returns 0 if failed to hash and 1 if
	function runs successful. Takes 4 
	arguments: First - The input string to
	be hashed (password), Second - The
	output hashed value, Third - The
	Value of the key will be assigned to 
	this variable, Fourth - The salt Value
	is assigned to this variable */
int hash_value(char*, char*, char*, int*);

/*	Determines if two hash values are the
	same. Function returns 0 if false, and
	one if true. Takes two arguments. First
	is the first hash value as type const 
	unsigned char and second is the second
	value of the same type */
int compare_hash_values(char *, char *);

/* 	sets a char pointer to the current
	time based on the host system  */
void get_current_time(char *);

/* 	Takes a char by reference and
	assigns a generated random key
	to be used inside hash_value */
int generate_key(char*);

#endif