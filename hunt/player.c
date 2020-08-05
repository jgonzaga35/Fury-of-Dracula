////////////////////////////////////////////////////////////////////////
// Runs a player's game turn ...
//
// Can  produce  either a hunter player or a Dracula player depending on
// the setting of the I_AM_DRACULA #define
//
// This  is  a  dummy  version of the real player.c used when you submit
// your AIs. It is provided so that you can test whether  your  code  is
// likely to compile ...
//
// Note that this is used to drive both hunter and Dracula AIs. It first
// creates an appropriate view, and then invokes the relevant decideMove
// function,  which  should  use the registerBestPlay() function to send
// the move back.
//
// The real player.c applies a timeout, and will halt your  AI  after  a
// fixed  amount of time if it doesn 't finish first. The last move that
// your AI registers (using the registerBestPlay() function) will be the
// one used by the game engine. This version of player.c won't stop your
// decideMove function if it goes into an infinite loop. Sort  that  out
// before you submit.
//
// Based on the program by David Collien, written in 2012
//
// 2017-12-04	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v1.2	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v1.3	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Game.h"
#ifdef I_AM_DRACULA
# include "dracula.h"
# include "DraculaView.h"
#else
# include "hunter.h"
# include "HunterView.h"
#endif

// Moves given by registerBestPlay are this long (including terminator)
#define MOVE_SIZE 3

// The minimum static globals I can get away with
static char latestPlay[MOVE_SIZE] = "";
static char latestMessage[MESSAGE_SIZE] = "";

// A pseudo-generic interface, which defines
// - a type `View',
// - functions `ViewNew', `decideMove', `ViewFree',
// - a trail `xtrail', and a message buffer `xmsgs'.
#ifdef I_AM_DRACULA

typedef DraculaView View;

# define ViewNew DvNew
# define decideMove decideDraculaMove
# define ViewFree DvFree

// # define xPastPlays "GZA.... SED.... HZU.... MZU...."
//#define xPastPlays  "GGE.... SGE.... HGE.... MGE.... DCN.V..\nGGE.... SGE.... HGE.... MGE.... DD1T...\nGGE.... SGE.... HGE.... MGE.... DHIT...\nGGE.... SGE.... HGE.... MGE.... DGAT...\nGGE.... SGE.... HGE.... MGE.... DKLT...\nGGE.... SGE.... HGE.... MGE.... DBET...\nGGE.... SGE.... HGE.... MGE.... DBCT...\nGGE.... SGE.... HGE.... MGE...."        
#define xPastPlays "GED.... SGA.... HRO.... MGR.... DST.V.. GED.... SCD.... HBI.... MAL.... DMUT... GED.... SKL.... HBI.... MAL.... DZAT... GED.... SKL.... HBI.... MAL.... DSJT... GED.... SKL.... HBI.... MAL.... DSOT... GED.... SKL.... HBI.... MAL.... DVAT... GED.... SKL.... HBI.... MAL.... DATT.V. GED.... SKL.... HBI.... MAL.... DD2T.M. GED.... SKL.... HBI.... MAL.... DSAT.M. GED.... SKL.... HBI.... MAL.... DHIT.M. GED.... SKL.... HBI.... MAL.... DIO..M. GED.... SKL.... HBI.... MAL.... DTS..M. GED.... SKL.... HBI.... MAL.... DCGT.M. GED.... SKL.... HBI.... MAL.... DD1.VM. GED.... SKL.... HBI.... MAL.... DMS..M. GED.... SKL.... HBI.... MAL.... DBAT.M. GED.... SKL.... HBI.... MAL.... DTOT... GED.... SKL.... HBI.... MAL.... DBOT... GED.... SKL.... HBI.... MAL.... DNAT.M. GED.... SKL.... HBI.... MAL.... DD2T.V. GED.... SKL.... HBI.... MAL.... DCFT... GED.... SKL.... HBI.... MAL.... DMRT.M. GED.... SKL.... HBI.... MAL.... DGET.M. GED.... SKL.... HBI.... MAL.... DSTT.M. GED.... SKL.... HBI.... MAL.... DZUT.M. GED.... SKL.... HBI.... MAL.... DMIT.M. GED.... SKL.... HBI.... MAL.... DD2.VM. GED.... SKL.... HBI.... MAL.... DMUT.M. GED.... SKL.... HBI.... MAL.... DNUT.M. GED.... SKL.... HBI.... MAL.... DFRT.M. GED.... SKL.... HBI.... MAL.... DLIT.M. GED.... SKL.... HBI.... MAL.... DCOT.M. GED.... SKL.... HBI.... MAL.... DAMT.V. GED.... SKL.... HBI.... MAL.... DBUT.M. GED.... SKL.... HBI.... MAL.... DPAT.M. GED.... SKL.... HBI.... MAL.... DGET.M. GED.... SKL.... HBI.... MAL.... DCFT.M. GED.... SKL.... HBI.... MAL.... DBOT.M. GED.... SKL.... HBI.... MAL.... DNAT.M. GED.... SKL.... HBI.... MAL.... DLE.VM. GED.... SKL.... HBI.... MAL.... DBUT.M. GED.... SKL.... HBI.... MAL.... DAMT.M. GED.... SKL.... HBI.... MAL.... DCOT.M. GED.... SKL.... HBI.... MAL.... DFRT.M. GED.... SKL.... HBI.... MAL.... DNUT.M. GED.... SKL.... HBI.... MAL.... DLIT.V. GED.... SKL.... HBI.... MAL.... DBRT.M. GED.... SKL.... HBI.... MAL.... DHAT.M. GED.... SKL.... HBI.... MAL.... DD2T.M. GED.... SKL.... HBI.... MAL.... DPRT.M. GED.... SKL.... HBI.... MAL.... DVIT.M. GED.... SKL.... HBI.... MAL.... DMUT.M. GED.... SKL.... HBI.... MAL.... DZA.VM. GED.... SKL.... HBI.... MAL.... DSJT.M. GED.... SKL.... HBI.... MAL.... DSOT.M. GED.... SKL.... HBI.... MAL.... DVAT.M. GED.... SKL.... HBI.... MAL.... DATT.M. GED.... SKL.... HBI.... MAL.... DD2T.M. GED.... SKL.... HBI.... MAL.... DIO..V. GED.... SKL.... HBI.... MAL.... DSAT.M. GED.... SKL.... HBI.... MAL.... DSOT.M. GED.... SKL.... HBI.... MAL.... DSJT.M. GED.... SKL.... HBI.... MAL.... DVAT.M. GED.... SKL.... HBI.... MAL.... DATT.M. GED.... SKL.... HBI.... MAL.... DD2T... GED.... SKL.... HBI.... MAL.... DIO..M. GED.... SKL.... HBI.... MAL.... DSAT.M. GED.... SKL.... HBI.... MAL.... DSOT.M. GED.... SKL.... HBI.... MAL.... DSJT.M. GED.... SKL.... HBI.... MAL.... DVAT.M. GED.... SKL.... HBI.... MAL.... DATT.M. GED.... SKL.... HBI.... MAL...."
			
			
			
					
					
					
					
					

# define xMsgs { "", "", "", "" }

#else

typedef HunterView View;

# define ViewNew HvNew
# define decideMove decideHunterMove
# define ViewFree HvFree

# define xPastPlays "GZA.... SED.... HZU...."
# define xMsgs { "", "", "" }

#endif

int main(void)
{
	char *pastPlays = xPastPlays;
	Message msgs[] = xMsgs;

	View state = ViewNew(pastPlays, msgs);
	decideMove(state);
	ViewFree(state);

	printf("Move: %s, Message: %s\n", latestPlay, latestMessage);
	return EXIT_SUCCESS;
}

// Saves characters from play (and appends a terminator)
// and saves characters from message (and appends a terminator)
void registerBestPlay(char *play, Message message)
{
	strncpy(latestPlay, play, MOVE_SIZE - 1);
	latestPlay[MOVE_SIZE - 1] = '\0';

	strncpy(latestMessage, message, MESSAGE_SIZE - 1);
	latestMessage[MESSAGE_SIZE - 1] = '\0';
}
