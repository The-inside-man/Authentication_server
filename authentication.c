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

#include "authentication.h"		// The header File including the prototypes		
#include <string.h>				// Used for strcmp(), strlen()
#include <time.h>				// Used for time() initializing srand()
#include <stdio.h>				// Used for standard in/out functions
#include <stdlib.h>				// Used for all standard Library calls
#include <unistd.h>

#define HASH_LENGTH_256 32		// Constant used for lengths of 256-bit hashing
#define HASH_RANGE		177		// Constant used for modulus to remain inside ascii values
#define HASH_CHAR_RANGE 36		// number of possible characters for hash

/* 	Generates a random one time use 
	salt value. Takes no arguments. */
int generate_salt(){
	srand(time(NULL));
	unsigned long r;
	int i;	
		for(i = 0; i < 64; i ++){
			r += rand() % 100;
		}
	return abs(r);
}

/*	Computes the hash value of some input
	and returns the output hash value. 
	Returns 0 if failed to hash and 1 if
	function runs successful.  */
int hash_value(char *input, char *output, char *key_value, int *salt_value){
	/*  Array of characters for the hash function  */
	char values[HASH_CHAR_RANGE] = {'A','B','C','D','E','F',
									'G','H','I','J','K','L',
									'M','N','O','P','Q','R',
									'S','T','U','V','W','X',
									'Y','Z','1','2','3','4',
									'5','6','7','8','9','0'};
									
	/*  Variable to be used for function  */
	int len = strlen(input);
	int total[HASH_LENGTH_256];
	int i,j;
	int salt;
	char key[HASH_LENGTH_256+1];				// The empty key array for function
	
	key[HASH_LENGTH_256] = 0x00;
	//printf("salt value is: %d \n", *salt_value);
	
	if((strlen(key_value) == 32)){			// This means a key has been provided
		strncpy(key, key_value, 32);		// Key value is assigned var for function to use
		//printf("key is provided: %s\n", key);
		if(salt_value >= 0){					// Confirms that a salt value was provided as well
			salt = *salt_value;				// Assigns the provided salt value to variable
			//printf("Salt used: %d\n", salt);
		} else {
			printf("Error: No salt value provided with key\n");  // Error message for missing salt
			exit(1);
		}
	} else{	
		salt = generate_salt();					// One time Salt value is obtained (to be stored for client)
		*salt_value = salt;						// Salt value assigned to var for client use						
		generate_key(key);						// Key value is generated
		strncpy(key_value, key, 32);			// Key value is assigned to var for client use
		//printf("RTN Key: %s\n", key_value);
	}
	if(input[strlen(input) - 1] == '\n'){		// Remove newline character from string if present
		input[strlen(input) - 1] = '\0';
	}
	//printf("input is: %s\n", input);
	// generate the hash combined with salt and message
	if(strlen(key) > 0){				// Confirms output was loaded with key
	
		/* 	Here we do a number of rounds to complete the Hash */
		//printf("key is: %s\n", key);
		/* 	First round we add the ascii value to the salt value
			modulus the HASH_CHAR RANGE_RANGE, which is 36 and add
			this value to running totals total[i] */
		for(i = 0; i < HASH_LENGTH_256; i ++){
			total[i]  = ((int)key[i] + salt) % HASH_CHAR_RANGE;		    			
//			printf("sizeof key = %lu\n", sizeof(key));
		// ROUND 1
		}
		//printf("key is: %s\n", key);
		/* 	Second round */
		//printf("second time we handle input\n");
		for(i = 0; i < HASH_LENGTH_256; i ++){
			total[i]  = (total[i] + ((int)input[i % len] + salt) 
							% HASH_CHAR_RANGE) % HASH_CHAR_RANGE;	// ROUND 2
		}
		//printf("key is: %s\n", key);
		/* 	Third round */
		//printf("third round of manipulation\n");
		for(i = 0; i < HASH_LENGTH_256; i ++){
			total[i]  = (total[HASH_LENGTH_256 - i] + ((int)input[i % len] + salt) 
							% HASH_CHAR_RANGE) % HASH_CHAR_RANGE;	// ROUND 3
		}
		//printf("key is: %s\n", key);
		/* 	Assign the character equivalent of each total to the 
			output array that will be our HASH VALUE  */
		//printf("fourth round to assign chars\n");
		for(i = 0; i < HASH_LENGTH_256; i ++){
			output[i] = values[total[i]];	// The Final saved HASH VALUE
		}
		//printf("Finished hashing\n");
	} else{									// If output failed to load key return 0
		return 0;
	} 
	return 1;
}

/*	Determines if two hash values are the
	same. Function returns 0 if false, and
	one if true. Takes two arguments. First
	is the first hash value as type const 
	unsigned char and second is the second
	value of the same type */
int compare_hash_values(char *one, char *two){
	int result = 1;
	int i = 0;
	for(i = 0; i < HASH_LENGTH_256; i++){
		if(one[i] == two[i]){
			result = result & 1;				// Bitwise AND used to maintain 1 when valid
		} else{
			result = 0;							// Bitwise AND against 0 results in invalid
		}
	}
	return result;								// Return the result 0 invalid 1 valid
}

/* 	sets a char pointer to the current
	time based on the host system  */
void get_current_time(char *tme){
	time_t rawtime;
  	struct tm * timeinfo;

  	time(&rawtime );
  	timeinfo = localtime ( &rawtime );
  	printf ( "|  Time stamp\t: %s", asctime (timeinfo) );

}

/* 	Takes a char by reference and
	assigns a generated random key
	to be used inside hash_value */
int generate_key(char *key){
	srand(time(NULL));
	
	/* 	An array of chars storing the alphabetical
		characters for the english language is 
		used to generate a key to be used for the 
		hash function. Called within the hash 
		function and assigned to the third variable
		passed to hash_value() */
	char letters[26] = {'a','b','c','d','e','f',
						'g','h','i','j','k','l',
						'm','n','o','p','q','r',
						's','t','u','v','w','x',
						'y','z'};
	int i;
	for(i = 0; i < HASH_LENGTH_256; i++){
		int r = rand();
		key[i] = letters[r % 26];				// Random values from a to z generate the key
	}
	//printf("key generated: %s\n", key);
	return 1;
}

/*int main(){
	char output[33];
	char *input = "HElLo my name is billy!";
	char aKey[33];
	int aSalt = 0x4321;
	
	aKey[32] = 0x00;
	output[32] = 0x00;
	printf("salt in main() is: %x \n", aSalt);

	
	int result = hash_value(input, output, aKey, &aSalt);
	printf("-------------------------------------------\n");
	printf("%s\n|with size| %lu\n", aKey, strlen(aKey));	 // Returns aKey concatenated with output...
	printf("%s\n|with size| %lu\n", output, strlen(output));  // Returns lager than it should
	printf("-------------------------------------------\n");
	printf("Each should be of size 32\n");
}*/