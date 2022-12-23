/*
Author:        Dominic Rando
Major:         Computer Science
Creation Date: November 12, 2022
Due Date:      November 29, 2022
Course:        CSC 328
Professor:     Dr. Frye
Assignment:    Wordle Project
Filename:      socketUtilities.h
Purpose:       This file defines the headers to the library socketUtilities.c
               This file is to be #included in the client and server programs
               so that there won't be any warning messages thrown upon compiling
*/

/*
Function Name:	indexOfTerminator
Description:	Will find the index of the terminating character (#) from messages 
Parameters:     str: the string to find the terminating character - input
Return Value:	int - index of the character, -1 if not found
*/
int indexOfTerminator(char *str);

/*
Function Name:	readFromSocket
Description:	Receives a message from socket and stores it in the buffer
Parameters:     sockfd: the socket to be read from - input
                buffer: the buffer being read to and saved to - input/output 
                maxBytes: maximum bytes to read from the buffer - input
Return Value:	void - none
*/
void readFromSocket(int sockfd, char buffer[], int maxBytes);

/*
Function Name:	writeToSocket
Description:	Writes a message to a given socket
Parameters:     sockfd: the socket to be written to - input/output
                message: message to be sent to socket - input
                maxBytes: Bytes to send in the buffer - input
Return Value:	void - none
*/
void writeToSocket(int sockfd, char* message, int maxBytes);

/*
Function Name:	upWord
Description:	Sets every letter in char array to uppercase
Parameters:     array: The array to be uppercased - input/output
Return Value:	void - none
                array is referenced like a pointer, no need to return
*/
void upWord(char word[]);
