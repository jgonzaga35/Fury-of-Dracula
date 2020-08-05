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

# define xPastPlays "GED.... SGA.... HRO.... MGR.... DST.V.. GED.... SBC.... HRO.... MAL.... DBUT... GLO.... SSO.... HNP.... MAL.... DAMT... GMN.... SSO.... HBI.... MAL.... DCOT... GED.... SSO.... HAS.... MMS.... DFRT... GED.... SBE.... HAS.... MMR.... DLIT... GED.... SBE.... HAS.... MMR.... DBRT.V. GSW.... SKL.... HIO.... MMR.... DHAT.M. GLO.... SBD.... HBS.... MCF.... DNS..M. GMN.... SVE.... HIO.... MGE.... DAMT.M. GLO.... SMU.... HBS.... MMR.... DBUT.M. GLV.... SZA.... HIO.... MMR.... DCOT.M. GMN.... SSJ.... HBS.... MCF.... DFRT.M. GLO.... SZA.... HIO.... MGE.... DLI.VM. GLV.... SSJ.... HBS.... MMR.... DBRT... GSW.... SZA.... HIO.... MMR.... DHAT.M. GLO.... SSJ.... HBS.... MCF.... DNS..M. GMN.... SZA.... HIO.... MGE.... DAMT.M. GLO.... SSJ.... HBS.... MMR.... DBUT.M. GLV.... SZA.... HIO.... MMR.... DCOT.V. GMN.... SSJ.... HBS.... MCF.... DFRT.M. GLO.... SZA.... HIO.... MGE.... DLIT.M. GLV.... SSJ.... HBS.... MMR.... DBRT... GSW.... SZA.... HIO.... MGO.... DPRT.M. GLO.... SVI.... HTS.... MST.... DVIT.M. GEC.... SVITD.. HGO.... MMU.... DBDT.M. GLE.... SVI.... HVE.... MVI.... DSZ.VM. GBU.... SVI.... HVI.... MVI.... DKLT.M. GST.... SVI.... HVI.... MVI.... DCDT.M. GFR.... SVI.... HVI.... MVI.... DD1T.M. GBR.... SVI.... HVI.... MVI.... DHIT... GBDT... SVI.... HVI.... MVI.... DGAT... GKLT... SVI.... HVI.... MVI.... DCNT.V. GCDTTT. SVI.... HVI.... MVI.... DBCT... GBE.... SVI.... HVI.... MVI.... DBET... GBETD.. SVI.... HVI.... MVI.... DD1T... GBETD.. SVI.... HVI.... MVI.... DSOT... GJM.... SVI.... HVI.... MVI.... DVAT.M. GJM.... SVI.... HVI.... MVI.... DSAT.M. GJM.... SVI.... HVI.... MVI.... DHI.VM. GJM.... SVI.... HVI.... MVI.... DIO.... GJM.... SVI.... HVI.... MVI.... DATT... GJM.... SVI.... HVI.... MVI.... DD2..M. GJM.... SVI.... HVI.... MVI.... DBS..M. GJM.... SVI.... HVI.... MVI.... DVRT.M. GJM.... SVI.... HVI.... MVI.... DSOT.V. GJM.... SVI.... HVI.... MVI.... DBCT... GJM.... SVI.... HVI.... MVI.... DCNT.M. GJM.... SVI.... HVI.... MVI.... DGAT... GJM.... SVI.... HVI.... MVI.... DCDT... GJM.... SVI.... HVI.... MVI.... DD1T.M. GJM.... SVI.... HVI.... MVI.... DHIT.M. GJM.... SVI.... HVI.... MVI.... DKL.VM. GJM.... SVI.... HVI.... MVI.... DBCT.M. GJM.... SVI.... HVI.... MVI.... DCNT.M. GJM.... SVI.... HVI.... MVI.... DGAT.M. GJM.... SVI.... HVI.... MVI.... DCDT.M. GJM.... SVI.... HVI.... MVI.... DD1T.M. GJM.... SVI.... HVI.... MVI.... DHIT.V. GJM.... SVI.... HVI.... MVI.... DKLT.M. GJM.... SVI.... HVI.... MVI.... DBCT.M. GJM.... SVI.... HVI.... MVI.... DCNT.M. GJM.... SVI.... HVI.... MVI.... DGAT.M. GJM.... SVI.... HVI.... MVI.... DCDT.M. GJM.... SVI.... HVI.... MVI.... DD1T.M. GJM.... SVI.... HVI.... MVI.... DHI.VM. GJM.... SVI.... HVI.... MVI.... DKLT.M. GJM.... SVI.... HVI.... MVI.... DBCT.M. GJM.... SVI.... HVI.... MVI.... DCNT.M. GJM.... SVI.... HVI.... MVI.... DGAT.M. GJM.... SVI.... HVI.... MVI.... DCDT.M. GJM.... SVI.... HVI.... MVI.... DD1T.V. GJM.... SVI.... HVI.... MVI.... DHIT.M. GJM.... SVI.... HVI.... MVI.... DKLT.M. GJM.... SVI.... HVI.... MVI.... DBCT.M. GJM.... SVI.... HVI.... MVI.... DCNT.M. GJM.... SVI.... HVI.... MVI.... DGAT.M. GJM.... SVI.... HVI.... MVI.... DCDT.M. GJM.... SVI.... HVI.... MVI.... DD1.VM. GJM.... SVI.... HVI.... MVI.... DHIT.M. GJM.... SVI.... HVI.... MVI.... DKLT.M. GJM.... SVI.... HVI.... MVI.... DBCT.M. GJM.... SVI.... HVI.... MVI.... DCNT.M. GJM.... SVI.... HVI.... MVI.... DGAT.M. GJM.... SVI.... HVI.... MVI.... DCDT.V. GJM.... SVI.... HVI.... MVI.... DD1T.M. GJM.... SVI.... HVI.... MVI.... DHIT.M. GJM.... SVI.... HVI.... MVI.... DKLT.M. GJM.... SVI.... HVI.... MVI.... DBCT.M. GJM.... SVI.... HVI.... MVI.... DCNT.M. GJM.... SVI.... HVI.... MVI.... DGAT.M. GJM.... SVI.... HVI.... MVI.... DCD.VM. GJM.... SVI.... HVI.... MVI.... DD1T.M. GJM.... SVI.... HVI.... MVI.... DHIT.M. GJM.... SVI.... HVI.... MVI.... DKLT.M. GJM.... SVI.... HVI.... MVI.... DBCT.M. GJM.... SVI.... HVI.... MVI.... DCNT.M. GJM.... SVI.... HVI.... MVI.... DGAT.V. GJM.... SVI.... HVI.... MVI.... DCDT.M. GJM.... SVI.... HVI.... MVI.... DD1T.M. GJM.... SVI.... HVI.... MVI.... DHIT.M. GJM.... SVI.... HVI.... MVI.... DKLT.M. GJM.... SVI.... HVI.... MVI.... DBCT.M. GJM.... SVI.... HVI.... MVI.... DCNT.M. GJM.... SVI.... HVI.... MVI.... DGA.VM. GJM.... SVI.... HVI.... MVI.... DCDT.M. GJM.... SVI.... HVI.... MVI.... DD1T.M. GJM.... SVI.... HVI.... MVI.... DHIT.M. GJM.... SVI.... HVI.... MVI.... DKLT.M. GJM.... SVI.... HVI.... MVI.... DBCT.M. GJM.... SVI.... HVI.... MVI.... DCNT.V. GJM.... SVI.... HVI.... MVI.... DGAT.M. GJM.... SVI.... HVI.... MVI.... DCDT.M. GJM.... SVI.... HVI.... MVI.... DD1T.M. GJM.... SVI.... HVI.... MVI.... DHIT.M. GJM.... SVI.... HVI.... MVI.... DKLT.M. GJM.... SVI.... HVI.... MVI.... DBCT.M. GJM.... SVI.... HVI.... MVI.... DBE.VM. GJM.... SVI.... HVI.... MVI.... DSOT.M. GJM.... SVI.... HVI.... MVI.... DSAT.M. GJM.... SVI.... HVI.... MVI.... DVAT.M. GJM.... SVI.... HVI.... MVI.... DATT.M. GJM.... SVI.... HVI.... MVI.... DHIT.M. GJM.... SVI.... HVI.... MVI.... DD3T.V. GJM.... SVI.... HVI.... MVI.... DSOT.M. GJM.... SVI.... HVI.... MVI.... DBET.M. GJM.... SVI.... HVI.... MVI.... DKLT.M. GJM.... SVI.... HVI.... MVI.... DCDT.M. GJM.... SVI.... HVI.... MVI.... DHIT.M. GJM.... SVI.... HVI.... MVI.... DD2T.M. GJM.... SVI.... HVI.... MVI.... DGA.VM. GJM.... SVI.... HVI.... MVI.... DCNT.M. GJM.... SVI.... HVI.... MVI.... DBS..M. GJM.... SVI.... HVI.... MVI.... DVRT.M. GJM.... SVI.... HVI.... MVI.... DSOT.M. GJM.... SVI.... HVI.... MVI.... DVAT.M. GJM.... SVI.... HVI.... MVI.... DSAT.V. GJM.... SVI.... HVI.... MVI.... DIO..M. GJM.... SVI.... HVI.... MVI.... DATT... GJM.... SVI.... HVI.... MVI.... DHIT.M. GJM.... SVI.... HVI.... MVI...."
# define xMsgs { "", "", "", "" }

#else

typedef HunterView View;

# define ViewNew HvNew
# define decideMove decideHunterMove
# define ViewFree HvFree

// # define xPastPlays "GZA.... SED.... HZU...."
// Test strings
# define xPastPlays "GED.... SGA.... HRO.... MGR.... DST.V.. GMN.... SCD.... HMI.... MMA.... DC?T... GED.... SGA.... HMR.... MAL.... DC?T... GLV.... SBC.... HMS.... MSR.... DC?T... GSW.... SBE.... HAL.... MMR.... DPAT... GLO.... SSJ.... HLS.... MZU.... DC?T... GLO.... SSJ.... HLS.... MZU.... DC?T.V. GED.... SBE.... HAO.... MMR.... DC?T.M. GMN.... SSJ.... HGW.... MPAT... DC?T.M. GLO.... SZA.... HAO...."
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
