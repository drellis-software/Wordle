/*
Author:        Dominic Rando
Major:         Computer Science
Creation Date: November 11, 2022
Due Date:      November 29, 2022
Course:        CSC 328
Professor:     Dr. Frye
Assignment:    Wordle Project
Filename:      socketUtilities.c
Purpose:       This file has the implementations from socketUtilities.h.
               Each function is meant to facilitate proper connections between
               client and server by incorporating error checking in this library
*/

// FUNCTION DOCUMENTATION IS IN socketUtilities.h

#include <stdlib.h>      // exit()
#include <stdio.h>       // perror()
#include <unistd.h>      // read()
#include <string.h>      // strchr(), strcpy()
#include <sys/socket.h>  // send()
#include <ctype.h>       // toupper()

int indexOfTerminator(char *str){
  char* foo;  // temp char pointer to store results from strchr function
  int index;  // Index where # appears

  // strchr returns string until #, gives needed message
  if(foo = strchr(str, '#')) {
  
    // subtracts string that has everything up to # from the whole string
    // gives index where message should be copied
    index = (int) (foo - str);
  }
  
  // Terminating character cannot be found
  else { index = -1; }
  
  return index;
}

/*
need to check if something happened in the send. 
If the socket missed something from the send, then we needf to ask the server to resend the message
*/
// check for lost packets
void readFromSocket(int sockfd, char buffer[], int maxBytes){
  
  // Stores final message
  char finalMsg[256];  
  int n = -1;
  
  // While terminator cannot be found (Didn't read full message)
  while(n < 0){   
    // Read from socket
    if (read(sockfd, buffer, maxBytes) < 0){
      perror("Failed to receive ");
      exit(-1);
    }
    // Save index of message without the terminator
    n = indexOfTerminator(buffer);      
  }
  
  // Copy message from the buffer without the terminator
  strncpy(finalMsg, buffer, n);
  
  // strncpy does not add a null terminator if source > destination
  // Gets rid of junk at end of string
  finalMsg[n] = '\0';

  // Copy message back to the buffer
  strcpy(buffer, finalMsg);
}

void writeToSocket(int sockfd, char* message, int maxBytes){
  // Stores message with the # on end
  char newMsg[maxBytes];
  
  // turn char pointer into char array
  strcpy(newMsg,  message);
  
  // Adds # onto end of message
  strcat(newMsg, "#");
  
  // Length of the word to send
  int msgLen = strlen(newMsg);
  
  int i = 0; // Holds how much information was sent
  
  // Loops until the entire message has been sent
  while(i < msgLen){
    i = send(sockfd, newMsg, msgLen, 0);
	if ( i < 0){
      perror("Failed to Send");
      exit(-1);
    }
  }
}

void upWord(char word[]){
  // store every char as uppercase
  for(int i = 0; i < strlen(word); i++){
    word[i] = toupper(word[i]);
  }         
}