/*
Author:        Devin Ellis
Major:         Computer Science
Creation Date: November 11, 2022
Due Date:      November 29, 2022
Course:        CSC 328
Professor:     Dr. Frye
Assignment:    Wordle Project
Filename:      client.c
Purpose:       This file has the client application for Wordle Server.
			   Using TCP connection, The client will receive a word	
			   from the server and go through a Wordle gameplay. 
			   Has play again or quit feature for user.
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#include "socketUtilities.h"

#define SIZE sizeof(struct sockaddr_in)
#define def 600
#define WORD_LEN 5
#define ALPHA 26

char* word[WORD_LEN] = {0};
char output[WORD_LEN+ 1];
int numEltsGuess = 0;
int numEltsCorrect = 0;

void matching(const char* answer, char guess[], char* output, char correct[], char guessed[]);
void gameplay(const char* word);

/*
Function Name:	connect2user
Description:	Receives host and port number and connects to server through TCP
Parameters:     host: Host name - input
                port: Specified port number - input
Return Value:	void - none
*/
void connect2user(char *host, int port) {
    // Declaration of socket and rv
	int sockfd, rv;
	// Struct pointer for servaddr
	struct sockaddr_in *servaddr;
	
	// Init of msg for receiving quote from server
	char srv[1024];
	char srv1[1024];
	char choice[1024];
    char cli[1024] = "WORD\0"; //message to retrieve word
	char cli1[1024] = "QUIT\0"; //message to quit
	
	// init of struct addrinfo for server and local
	struct addrinfo *hintsinfo = (struct addrinfo *)malloc(sizeof(struct addrinfo));
	struct addrinfo *servaddrinfo = (struct addrinfo *)malloc(sizeof(struct addrinfo));
	
	// IP addr struct init
	struct in_addr *IPaddr = (struct in_addr *)malloc(sizeof(struct in_addr));

	// Set up for local addr info
	hintsinfo->ai_family = AF_INET;
	hintsinfo->ai_flags = 0;
	hintsinfo->ai_protocol = 0;
	hintsinfo->ai_socktype = SOCK_STREAM; // TCP
	
	// System call to get addr info of inputted host
	rv = getaddrinfo(host, NULL, hintsinfo, &servaddrinfo);
	if (rv < 0){
		printf("Error in getaddrinfo call: %s\n", gai_strerror(rv));
		exit(-1);
	}
	
	// deckares servaddr equal to servaddrinfo->ai_adder
	servaddr = (struct sockaddr_in *)servaddrinfo->ai_addr;
	
	// Sets servaddr port to 17
	servaddr->sin_port = htons(port);
	
	// Creates socket of TCP
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1){
		perror("Error creating client socket");
		exit(-1);
	}
	
	// Connects socket to server
		rv = connect(sockfd, (struct sockaddr *) servaddr, sizeof(struct sockaddr));
		if (rv == -1){
			perror("Error connecting to the server");
			exit(-1);
		}		
		
		readFromSocket(sockfd, srv, sizeof(srv));
		if (strcmp(srv, "HELLO") == 0) {
			printf("------------------------------------------------\n");
			printf("Welcome to WORDLE!\n");
			printf("Options:\n");
			printf("         ':play' (starts a new wordle game)\n");
			printf("         ':quit' (quits the application)\n\n");
			printf("------------------------------------------------\n");
			
			while(1) {
				printf("------------------------------------------------\n");
				printf("          <> ");
				scanf("%s", choice);
				if(strcmp(choice, ":quit") == 0) {
					writeToSocket(sockfd, cli1, sizeof(srv1)+1);
					close(sockfd);
					printf("[-]Disconnected from server.\n");
					exit(1);
				}
				if (strcmp(choice, ":play") == 0) {
					writeToSocket(sockfd, cli, sizeof(srv1)+1);
					readFromSocket(sockfd, srv, sizeof(srv));
					//printf("Starting Game word received: %s\n", srv);
					gameplay(srv);
				}
			}
		}
		close(sockfd);
	
}


/*
Function Name:	make_upper
Description:	Receives user guess and makes characters uppercase
Parameters:     s: char array to make uppercase - input/output
Return Value:	void - none
*/
void make_upper(char s[]){
	int len = strlen(s);
	for(int k = 0; k < len; k++){
			s[k] = toupper(s[k]);
	}
}

/*
Function Name:	gameplay
Description:	receives word from server and controls gameplay of wordle
				including getting guess and attempts
Parameters:     word: the word to guess from server - input
Return Value:	void - none
*/
void gameplay(const char* word){
	
	// Variable Declaration
	char guess[WORD_LEN + 1];
	char correct[21];
	char guessed[21];
	int MAX_ATTEMPTS = 6;
	int flag = 0;
	int cnt = 0;
	memset(guessed, '-', 21);
	memset(correct, '-', 21);
	
	// loop for guess attempts
	for(int j = 0; j < MAX_ATTEMPTS; j++){
		printf("Guess Remaining [%d]\n", MAX_ATTEMPTS-j);
		printf("Guess word here: ");
		scanf("%s", guess);
		printf("\n");
		
		for(int i = 0; i < strlen(guess); i++){
			if(isalpha(guess[i]) == 0){
				flag = 1;
				break;
			}
			else{
				continue;
			}
		}
		
		
		// Checks for user guess length
		if(strlen(guess) != 5){
			printf("\nInvalid word guess: (Guesses must be of length 5)\n");
			if(j == 0){
				j = 0;
			}
			else{
				j--;
			}
		}
		
		else if(flag == 1){
			printf("\nInvalid word guess: (Guesses must be Alpabetical Letters)\n");
			if(j == 0){
				j = 0;
			}
			else{
				j--;
				flag = 0;
			}
		}
		
		else{
			// Function call to make guess uppercase
			make_upper(guess);
			
			// Correctly guessed word, end gameplay
			if(strcmp(guess, word) == 0){
				printf("!!!!!!!!!!!! GOOD JOB !!!!!!!!!!!!\n\n");
				printf("   You correctly guessed %s\n\n", word);
				printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
				break;
			}
			
			else if(strcmp(guess, word) != 0){
				// Function call to matching
				matching(word, guess, output, correct, guessed);
			} 
		}
		cnt ++;
	}
	if (cnt == 6) {
		printf("!!!!!!!!!!!! YOU LOST !!!!!!!!!!!!\n\n");
		printf("       Attempt limit hit\n\n");
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
	}
	printf("  Options: \n");
	printf("          ':play' (play with a new word)\n");
	printf("          ':quit' (quit the game)\n");

}

/*
Function Name:	matching
Description:	Receives answer and guess and determines guess correctness and correct characters
Parameters:     answer: word from server - input
                guess: user guess to compare to answer - input/output 
                output: char array to display correct spots - input
				correct: char array for correct letters in word - input/output
				guessed: char array for characters guessed not in array - input/ouput
Return Value:	void - none
*/
void matching(const char* answer, char guess[], char* output, char correct[], char guessed[]) {
	
	// Declaration of copy
	char* copy = (char*)calloc(WORD_LEN, sizeof(char));

	// Copy answer into copy
	strcpy(copy, answer);
	
	// loop over guess and set copy flags
	for(int i = 0; i < WORD_LEN; i++){
		// Character in correct spot of word
		if(guess[i] == answer[i]){
			copy[i] = '*';
		}
		else{
			for(int j = 0; j < WORD_LEN; j++){
				// Character in word but not right spot
				if(strchr(answer, guess[i])){
					copy[i] = '#';
				}
				// Char is not in word
				else{
					copy[i] = '?';
				}
			}
		}
	}
	
	// Loop over copy length to set values
	for(int i = 0; i < strlen(copy); i++){
		// Character is in right spot
		if(copy[i] == '*'){
			output[i] = answer[i];
			correct[i] = ' ';
		}
		else if(copy[i] == '#'){
			// If charcter in answer not in right spot and not in correct array
			if(strchr(answer, guess[i])){
				if(strchr(correct, guess[i]) == NULL){
					correct[numEltsCorrect] = guess[i];
					output[i] = '_';
					numEltsCorrect++;
				}
				else{
					output[i] = '_';
				}

			}
		}
		// Character not in word (answer)
		else{
			if(strchr(guessed, guess[i]) == NULL){
				guessed[numEltsGuess] = guess[i];
				output[i] = '_';
				numEltsGuess++;
			}
			else{
				output[i] = '_';
			}
		}
	}
	
	printf("   Guessed Letters:  ");
	for(int i = 0; i < numEltsGuess; i++){
		printf("%c", guessed[i]);
	}
	
	printf("\n");
	
	printf("\n   Correct Letters:  ");
	for(int i = 0; i < numEltsCorrect; i++){
		printf("%c", correct[i]);
	}
	
	// Output 
	printf("\n");
	printf("\n   Correct Spots:    %s\n\n", output);

}

int main(int argc, char **argv){

    if(argc == 3){
		char *a = argv[2];
		int port = atoi(a);
        connect2user(argv[1], port);
    }
	else if (argc == 2) {
		int port = 4321;
		connect2user(argv[1], port);
	}
	else {
      printf("\nusage: %s <hostname> <port_number>\n\n", argv[0]);
      exit(1);
    }
}