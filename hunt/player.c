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

# define xPastPlays "GED.... SGA.... HRO.... MGR.... DST.V.. GNS.... SCN.... HZU.... MCA.... DPAT... GNS.... SBS.... HZU.... MMA.... DCFT... GNS.... SCN.... HZU.... MSR.... DHIT... GNS.... SVR.... HZU.... MLS.... DGET... GNS.... SVR.... HLI.... MSN.... DZUT... GNS.... SCN.... HNU.... MSR.... DMIT.V. GNS.... SVR.... HST.... MTO.... DVET.M. GNS.... SCN.... HPA.... MTO.... DAS..M. GNS.... SVR.... HMR.... MTO.... DD2T.M. GNS.... SCN.... HGET... MTO.... DMUT... GNS.... SVR.... HMR.... MTO.... DSTT.M. GNS.... SCN.... HLE.... MTO.... DZUT.M. GNS.... SVR.... HSR.... MTO.... DMI.VM. GNS.... SCN.... HBO.... MTO.... DHIT... GNS.... SVR.... HCF.... MTO.... DVET.M. GNS.... SCN.... HGE.... MTO.... DFLT.M. GNS.... SVR.... HZUT... MTO.... DROT.M. GNS.... SCN.... HZU.... MTO.... DNPT... GNS.... SVR.... HZU.... MTO.... DTS..V. GNS.... SCN.... HZU.... MTO.... DD1..M. GNS.... SVR.... HLI.... MTO.... DGOT.M. GNS.... SCN.... HNU.... MTO.... DVET.M. GNS.... SVR.... HST.... MTO.... DFLT.M. GNS.... SCN.... HPA.... MTO.... DHIT.M. GNS.... SVR.... HMR.... MTO.... DROT... GNS.... SCN.... HGE.... MTO.... DNP.V.. GNS.... SVR.... HMR.... MTO.... DTS..M. GNS.... SCN.... HLE.... MTO.... DD1..M. GNS.... SVR.... HSR.... MTO.... DGOT.M. GNS.... SCN.... HBO.... MTO.... DVET.M. GNS.... SVR.... HCF.... MTO.... DFLT.M. GNS.... SCN.... HGE.... MTO.... DHIT.V. GNS.... SVR.... HZU.... MTO.... DROT... GNS.... SCN.... HZU.... MTO.... DNPT... GNS.... SVR.... HZU.... MTO.... DTS..M. GNS.... SCN.... HZU.... MTO.... DD1..M. GNS.... SVR.... HLI.... MTO.... DGOT.M. GNS.... SCN.... HNU.... MTO.... DVET.M. GNS.... SVR.... HST.... MTO.... DFL.VM. GNS.... SCN.... HPA.... MTO.... DHIT.M. GNS.... SVR.... HMR.... MTO.... DROT... GNS.... SCN.... HGE.... MTO.... DNPT... GNS.... SVR.... HMR.... MTO.... DTS..M. GNS.... SCN.... HMR.... MMR.... DCGT.M. GEC.... SGA.... HMR.... MGO.... DMS..V. GLO.... SBD.... HGO.... MMI.... DALT.M. GEC.... SKL.... HMI.... MROT... DGRT... GLO.... SBC.... HRO.... MFL.... DCAT.M. GLO.... SBD.... HFL.... MRO.... DAO.... GEC.... SBD.... HRO.... MFL.... DBB..M. GLO.... SKL.... HFL.... MGO.... DBOT... GLO.... SBC.... HGO.... MST.... DCF.VM. GLO.... SBD.... HST.... MCO.... DGET.M. GEC.... SBD.... HCO.... MBU.... DMRT.M. GLO.... SKL.... HBU.... MBOT... DGOT... GLO.... SBC.... HBO.... MBB.... DFLT... GLO.... SBD.... HBB.... MBB.... DD1T... GEC.... SBD.... HBB.... MBB.... DROT.V. GLO.... SKL.... HBB.... MBB.... DBIT.M. GLO.... SBC.... HBB.... MBB.... DAS..M. GLO.... SBD.... HBB.... MBB.... DIO..M. GEC.... SBD.... HBB.... MBB.... DATT.M. GLO.... SKL.... HBB.... MBB.... DVAT.M. GLO.... SBC.... HBB.... MBB.... DSAT.M. GLO.... SBD.... HBB.... MBB.... DSO.VM. GEC.... SBD.... HBB.... MBB.... DBET... GLO.... SKL.... HBB.... MBB.... DBCT... GLO.... SBCTD.. HBB.... MBB.... DCNT.M. GLO.... SBD.... HBB.... MBB...."
# define xMsgs { "", "", "", "" }

#else

typedef HunterView View;

# define ViewNew HvNew
# define decideMove decideHunterMove
# define ViewFree HvFree

// # define xPastPlays "GZA.... SED.... HZU...."
// Test strings
# define xPastPlays "GED.... SGA.... HRO.... MGR.... DC?.V.. GNS.... SSZ.... HGO.... MCA.... DNUT... GEC.... SVE.... HMI.... MGR.... DC?T... GLO.... SMI.... HMU.... MCA.... DC?T... GMN.... SMU.... HNUT... MAO.... DC?T... GED.... SNU...."
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
