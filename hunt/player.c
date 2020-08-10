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

# define xPastPlays "GMN.... SGA.... HFR.... MMA.... DCD.V.. GED.... SCDVD.. HMU.... MAL.... DD1T... GNS.... SCDTD.. HZA.... MMS.... DHIT... GHA.... SBE.... HSZ.... MTS.... DGAT... GBR.... SKL.... HGATD.. MIO.... DCNT... GPR.... SBC.... HKL.... MBS.... DBS.... GBD.... SCNT... HKL.... MCN.... DIO.... GCN.... SBS.... HBC.... MGA.... DTS.... GBC.... SBS.... HBD.... MCN.... DMS..M. GSZ.... SBS.... HPR.... MBS.... DALT... GVI.... SBS.... HPR.... MIO.... DSRT... GHA.... SBS.... HPR.... MTS.... DMAT... GNS.... SBS.... HPR.... MMS.... DSNT... GED.... SBS.... HPR.... MALT... DLS.V.. GED.... SBS.... HPR.... MMAT... DCAT... GED.... SBS.... HPR.... MMA.... DAO.... GED.... SBS.... HPR.... MMA.... DNS..M. GED.... SBS.... HPR.... MMA.... DEC.... GED.... SBS.... HPR.... MMA.... DPLT.M. GED.... SBS.... HPR.... MMA.... DLOT.V. GED.... SBS.... HPR.... MMA.... DMNT.M. GED.... SBS.... HPR.... MMA.... DLVT... GED.... SBS.... HPR.... MMA.... DIR.... GED.... SBS.... HPR.... MMA.... DDUT... GED.... SBS.... HPR.... MMA.... DGWT.M. GED.... SBS.... HPR.... MMA.... DAO..M. GED.... SBS.... HPR.... MMA.... DNS..M. GED.... SBS.... HPR.... MMA.... DAMT.M. GED.... SBS.... HPR.... MMA.... DBUT... GED.... SBS.... HPR.... MMA.... DHIT.M. GED.... SBS.... HPR.... MMA.... DD2T.M. GED.... SBS.... HPR.... MMA.... DSTT... GED.... SBS.... HPR.... MMA.... DZUT... GED.... SBS.... HPR.... MMA.... DMIT.M. GED.... SBS.... HPR.... MMA.... DVET.M. GED.... SBS.... HPR.... MMA.... DFLT.M. GED.... SBS.... HPR.... MMA.... DROT.M. GED.... SBS.... HPR.... MMA.... DD2T.M. GED.... SBS.... HPR.... MMA.... DHIT.M. GED.... SBS.... HPR.... MMA.... DGO.VM. GED.... SBS.... HPR.... MMA.... DMRT.M. GED.... SBS.... HPR.... MMA.... DZUT.M. GED.... SBS.... HPR.... MMA.... DMIT.M. GED.... SBS.... HPR.... MMA.... DD2T.M. GED.... SBS.... HPR.... MMA.... DHIT.M. GED.... SBS.... HPR.... MMA.... DGET.V. GED.... SBS.... HPR.... MMA.... DMRT.M. GED.... SBS.... HPR.... MMA.... DZUT.M. GED.... SBS.... HPR.... MMA.... DMIT.M. GED.... SBS.... HPR.... MMA.... DD5T.M. GED.... SBS.... HPR.... MMA.... DHIT.M. GED.... SBS.... HPR.... MMA.... DGET.M. GED.... SBS.... HPR.... MMA.... DPA.VM. GED.... SBS.... HPR.... MMA.... DCFT.M. GNS.... SIO.... HVI.... MSR.... DMRT.M. GNS.... STS.... HVI.... MSR.... DD5T.M. GNS.... SMS.... HVI.... MSR.... DSTT.M. GNS.... SAO.... HVI.... MSR.... DGET.M. GNS.... SGW.... HVI.... MSR.... DZUT.V. GNS.... SGW.... HVI.... MSR.... DHIT.M. GNS.... SGW.... HVI.... MSR.... DMRT.M. GNS.... SGW.... HVI.... MSR.... DD4T.M. GNS.... SGW.... HVI.... MSR.... DSTT.M. GNS.... SGW.... HVI.... MSR.... DGET.M. GNS.... SGW.... HVI.... MSR.... DZUT.M. GNS.... SGW.... HVI.... MSR.... DHI.VM. GNS.... SGW.... HVI.... MSR.... DMRT.M. GNS.... SGW.... HVI.... MSR.... DD4T.M. GNS.... SGW.... HVI.... MSR.... DSTT.M. GNS.... SGW.... HVI.... MSR.... DGET.M. GNS.... SGW.... HVI.... MSR.... DZUT.M. GNS.... SGW.... HVI.... MSR.... DHIT.V. GNS.... SGW.... HVI.... MSR.... DMRT.M. GNS.... SGW.... HVI.... MSR.... DD4T.M. GNS.... SGW.... HVI.... MSR.... DSTT.M. GNS.... SGW.... HVI.... MSR.... DGET.M. GNS.... SGW.... HVI.... MSR.... DZUT.M. GNS.... SGW.... HVI.... MSR.... DHIT.M. GNS.... SGW.... HVI.... MSR.... DMR.VM. GNS.... SGW.... HVI.... MSR.... DD3T.M. GNS.... SGW.... HVI.... MSR.... DSTT.M. GNS.... SGW.... HVI.... MSR.... DGET.M. GNS.... SGW.... HVI.... MSR.... DZUT.M. GNS.... SGW.... HVI.... MSR.... DHIT.M. GNS.... SGW.... HVI.... MSR.... DMRT.V. GNS.... SGW.... HVI.... MSR.... DD4T.M. GNS.... SGW.... HVI.... MSR.... DSTT.M. GNS.... SGW.... HVI.... MSR.... DGET.M. GNS.... SGW.... HVI.... MSR.... DZUT.M. GNS.... SGW.... HVI.... MSR.... DHIT.M. GNS.... SGW.... HVI.... MSR.... DMRT.M. GNS.... SGW.... HVI.... MSR.... DD3.VM. GNS.... SGW.... HVI.... MSR.... DSTT.M. GNS.... SGW.... HVI.... MSR.... DGET.M. GNS.... SGW.... HVI.... MSR.... DZUT.M. GNS.... SGW.... HVI.... MSR.... DHIT.M. GNS.... SGW.... HVI.... MSR.... DMRT.M. GNS.... SGW.... HVI.... MSR.... DD4T.V. GNS.... SGW.... HVI.... MSR.... DSTT.M. GNS.... SGW.... HVI.... MSR.... DGET.M. GNS.... SGW.... HVI.... MSR.... DZUT.M. GNS.... SGW.... HVI.... MSR.... DHIT.M. GNS.... SGW.... HVI.... MSR.... DMRT.M. GNS.... SGW.... HVI.... MSR.... DD3T.M. GNS.... SGW.... HVI.... MSR.... DST.VM. GNS.... SGW.... HVI.... MSR.... DGET.M. GNS.... SGW.... HVI.... MSR.... DZUT.M. GNS.... SGW.... HVI.... MSR.... DHIT.M. GNS.... SGW.... HVI.... MSR.... DMRT.M. GNS.... SGW.... HVE.... MSR.... DD4T.M. GAO.... SGW.... HAS.... MSR.... DSTT.V. GGW.... SGW.... HAS.... MSR.... DNUT.M. GGW.... SGW.... HAS.... MSR.... DPRT.M. GGW.... SGW.... HAS.... MSR...."
# define xMsgs { "", "", "", "" }

#else

typedef HunterView View;

# define ViewNew HvNew
# define decideMove decideHunterMove
# define ViewFree HvFree

// # define xPastPlays "GZA.... SED.... HZU...."
// Test strings
# define xPastPlays "GMN.... SGA.... HFR.... MMA.... DCD.V.. GED.... SCDVD.. HMU.... MAL.... DD1T... GNS.... SCDTD.. HZA.... MMS.... DHIT... GHA.... SBE.... HSZ.... MTS.... DGAT... GBR.... SKL.... HGATD.. MIO.... DCNT... GPR.... SBC.... HKL.... MBS.... DBS.... GBD.... SCNT... HKL.... MCN.... DIO.... GCN.... SBS.... HBC.... MGA.... DTS.... GBC.... SIO.... HBE.... MBD.... DMS..M. GSZ.... SIO.... HSA.... MVI.... DALT... GVI.... SIO.... HIO.... MMU.... DSRT... GHA.... SIO.... HTS.... MMI.... DMAT... GNS.... SIO.... HNP.... MMR.... DSNT... GNS.... SIO.... HNP.... MTO.... DLS.V.."
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
