/*
Author:        Gabriel Leffew
Major:         Computer Science
Creation Date: November 15th, 2022
Due Date:      November 29th, 2022
Course:        CSC 328
Professor:     Dr. Frye
Assignment:    Wordle
Filename:      server.c
Purpose:       This file has the server application for Wordle Server.
			   Using TCP connection, The client will start and wait for 
               client connections, upion receiving a message (request) for
               'WORD' the server will send a random word to the client from
               word_list. When the server receives a 'QUIT' from the client
               it closes connection with the client. 
*/
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include "socketUtilities.h"

#define DEFAULT_PORT 4000
#define SIZE sizeof(struct sockaddr)


char word_list[85][10] = {"GRAVY", "HOUSE", "SERVE", "SORRY", "GREEK", "FUDGE", "CARRY", "CRATE", "ABUSE", "ADULT", "AGENT", "ANGER", "APPLE", "AWARD", "BEACH", "BIRTH", "BLOCK", "BLOOD", "BOARD", "BRAIN", "BREAD", "BREAK", "BROWN", "BUYER", "CAUSE", "CHAIN", "CHAIR", "CHEST", "CHIEF", "CHILD", "CHINA", "TITLE", "THING", "TOTAL", "TOUCH", "TRACK", "TREND", "TRIAL", "UNCLE", "VALUE", "VIDEO", "VOICE", "WASTE", "WHOLE", "YOUTH", "WOMAN", "SOUND", "SPEED", "STAFF", "SPITE", "STAGE", "SPOIL", "SIMON", "SHOCK", "SHAPE", "SCALE", "ROUTE", "RIVER", "REPLY", "PATIO", "RANGE", "QUEEN", "PROOF", "PRIZE", "POWER" , "POUND", "PLATE", "PITCH", "PILOT", "PAPER", "ORDER", "OFFER", "OWNER", "NURSE", "NOVEL", "NORTH", "NOISE", "MONEY", "MONTH", "KNIFE", "LAYER", "INPUT", "INDEX", "DEVIN", "GABRI"};
static void sig_chld(int signo);
/*
Function Name:	talk_to_client
Description:	send random words/quit the application
Parameters:     sockfd - socket to send and recieve from client
Return Value:	void
*/
void talk_to_client(int sockfd) {
    char message[1024] = "HELLO";
	char message2[1024];
	char recieved[1024];
	ssize_t	n;
	//client can send WORD to receive a word or QUIT to quit the game.
	writeToSocket(sockfd, message, sizeof(message));
	while (1) {
        readFromSocket(sockfd, recieved, sizeof(recieved));
		if (strcmp(recieved, "WORD") == 0) {
			//send initial word
			//printf("SENDING WORD");
			//message2 = word_list[r];
			int r = rand() % 85;
			writeToSocket(sockfd, word_list[r], sizeof(word_list[r]));
		}
		if (strcmp(recieved, "QUIT") == 0) {
			//close connection
			break;
		}
    }
	close(sockfd);
    return;
}
/*
Function Name:	main
Description:	create socket to communicate with client
                connections.
Parameters:     argc - number of command line arguments
                argv[1] - PORT number (optional)
Return Value:	0
*/
int main(int argc, char **argv) {
	srand(time(NULL));  
    int sockfd, newsockfd, valread, destlen;
	int status = 0;
    pid_t childpid, wpid;
    char buffer[1024] = { 0 };
    void sig_chld(int);
    struct sockaddr_in server, dest;
    socklen_t size;

    if (argc > 2) {
        printf("\nusage: %s <port_number>\n\n", argv[0]);
        exit(1);
    }

    //GOT THIS FROM Dr. Fryes /sum/server.c
    /************** SIGNAL HANDLING CODE   ********************/
	// set-up to catch the signal when a child process dies                                                                                                                      
	struct sigaction sigact;   // signal action structure                                                                                             

	// set values for signal action structure   
	memset((void *)&sigact, 0, sizeof(sigact));
	// setting sa_handler on Linux didn't work; use sa_sigaction with SA_SIGINFO
	sigact.sa_sigaction = (void *)&sig_chld;   // signal handler if SA_SIGINFO flag set                                                             
	// mask of signals to block during execution of signal handler --> set to empty set                                                               
	sigemptyset(&sigact.sa_mask);
	// allow the signal handler to be restarted and 
	// don't call signal handler if child process was stopped
	sigact.sa_flags = SA_SIGINFO | SA_RESTART | SA_NOCLDSTOP;;

	// set signal handler                                                                                                                             
	if (sigaction(SIGCHLD, &sigact, NULL)) {
		perror("Error with sigaction: ");
		return -1;
	}  // end if sigaction
	/************** END SIGNAL HANDLING CODE   *******************/

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket call failed");
        exit(-1);
    }  
	memset(&server, 0, sizeof(server));
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if (argc == 2) {
        server.sin_port = htons(atoi(argv[1]));
        //printf("Server running on port %d", argv[1]);
    }
    else if (argc == 1) {
        server.sin_port = htons(4321);
        //printf("Server running on defualt port %d", DEFAULT_PORT);
    }

    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("bind call failed");
        exit(-1);
    } 
    if (listen(sockfd, 1024) == -1) {
        perror("listen call failed");
        exit(-1);
    } 
	//client is only receiving one message from the server 
    destlen = sizeof(dest);
	//******Documentation for the while loop******
    //While loop accepts numerous clients 
    //calls fork to handle differ
    while (1) {
        if ((newsockfd = accept(sockfd, (struct sockaddr *)&dest, &destlen)) == -1) {
            if ((errno == EINTR) || (errno == ECONNABORTED)) {
	            continue;		/* back to while() */
            }
            else {
                perror("Accept call failed");
                exit(-1);
            }
        }
        //call fork to handle client connection
        //child
        if ((childpid = fork()) == 0) {
            close(sockfd);
            talk_to_client(newsockfd);
            close(newsockfd); 
            exit(0);
        }
        //parent
        else if (childpid > 0)  {
            close(newsockfd);
        }
        else { // fork call failed
            perror("Error creating a process");
            exit(-1);
        }
    }
    close(sockfd);

    return 0;
}
/*
Function Name:	sig_child
Description:	Handle SIGCHILD signal
Parameters:     signo - the signal number received
Return Value:	none
Owner:          Dr. Frye
                I got this from Dr. Frye's '/csc328/sockets/sum/server.c'
*/
static void sig_chld(int signo) {
	pid_t	pid;
	int		stat;
	int     saved_errno;   
	
	// save errno and restore it at end in case a server system call had set it
	// waitpid() may change the value of errno
	saved_errno = errno;
	
	// check for any child processes that have ended
	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("Wordle client connected at PID %d terminated\n", pid);
	
	// restore errno
	errno = saved_errno;
	
	return;
}  // end function sig_chld