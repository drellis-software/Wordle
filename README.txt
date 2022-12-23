---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Welcome to our WORDLE application.
Developed by Devin Ellis & Gabriel Leffew

To compile all of the files enter the directory they're in
and type:
           make

Once the files are compiled you'll see four new files in your
directory:
           client
           server
           socketUtilities.o
           libsocketUtilities.a

From here you can start the program!

To start the server: (<optional>)
    './server <PORT>'

    'nohup ./server <PORT> &'  <-- This will run the server in the background

    If a port number is not specified PORT 4321 will be used.

To start the client and play the game: (<optional>)

    './client acad.kutztown.edu <PORT>'

    If a port number is not specified PORT 4321 will be used.
    *NOTE: The port numbers MUST be the same if specified.

    From here youll be prompted with a start screen... 
    ------------------------------------------------
    Welcome to WORDLE!
    Options:
            ':play' (starts a new wordle game)
            ':quit' (quits the application)

    ------------------------------------------------
    To get a word and start the game type        (:play)
    To quit the applicationa and disconnect type (:quit)

    The same commands apply when you have either guessed 6 times
    or got the word correct....
    
    Have fun playing!!!

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Program: client.c
Purpose: This file has the client application for Wordle Server. Using TCP connection, The client will receive a word	
from the server and go through a Wordle gameplay. Has play again or quit feature for user.

USAGE:
<Functions>
connect2user(char *host, int port) 
	- Receives host and port number and connects to server through TCP

make_upper(char s[])
	- Receives user guess and makes characters uppercase

gameplay(const char* word)
	- receives word from server and controls gameplay of wordle, including getting guess and attempts

matching(const char* answer, char guess[], char* output, char correct[], char guessed[])
	- Receives answer and guess and determines guess correctness and correct characters
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Program: server.c 
Purpose: This file has the server application for Wordle Server. Using TCP connection, The client will start and wait for 
client connections, upion receiving a message (request) for 'WORD' the server will send a random word to the client from
word_list. When the server receives a 'QUIT' from the client it closes connection with the client. 

USAGE:
<Functions>
talk_to_client(int sockfd)
    - Sends random word to client or on 'QUIT' dsiconnects the client.
sig_chld(int signo)
    - Handle SIGCHILD signal
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ALL Library function info can be found in socketUtilities.c & socketUtilities.h 
