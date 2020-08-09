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

# define xPastPlays "GMN.... SGA.... HFR.... MMA.... DKL.V.. GED.... SBC.... HBR.... MAL.... DCDT... GNS.... SGA.... HPR.... MMS.... DGAT... GHA.... SGATD.. HVI.... MTS.... DCNT... GBR.... SKLV... HBD.... MIO.... DBS.... GPR.... SSZ.... HVE.... MBS.... DIO.... GVI.... SBD.... HMU.... MCNT... DTS.... GVE.... SKL.... HVI.... MBC.... DMS..M. GAS.... SBE.... HPR.... MGA.... DALT... GIO.... SVR.... HVE.... MBC.... DSRT... GVA.... SSA.... HGO.... MSZ.... DMAT... GSA.... SIO.... HMR.... MCN.... DSNT... GSA.... SIO.... HMR.... MCN.... DLST... GIO.... SVA.... HLE.... MGA.... DCA.V.. GVA.... SSA.... HNA.... MKL.... DGRT.M. GVA.... SSA.... HNA.... MKL.... DALT.M. GSA.... SIO.... HCF.... MSZ.... DSRT.M. GIO.... SVA.... HTO.... MKL.... DMAT.M. GIO.... SVA.... HTO.... MKL.... DSNT.M. GVA.... SSA.... HCF.... MSZ.... DLST.V. GSA.... SIO.... HTO.... MSO.... DCAT.M. GSA.... SIO.... HTO.... MSO.... DGRT.M. GSO.... SVA.... HCF.... MVR.... DALT.M. GVR.... SSA.... HTO.... MCN.... DSRT.M. GVR.... SSA.... HTO.... MCN.... DMAT.M. GBS.... SSO.... HCF.... MGA.... DSNT.M. GIO.... SVR.... HTO.... MKL.... DLS.VM. GIO.... SVR.... HTO.... MKL.... DCAT.M. GVA.... SBS.... HCF.... MSZ.... DGRT.M. GSA.... SIO.... HTO.... MKL.... DALT.M. GSA.... SIO.... HTO.... MKL.... DSRT.M. GIO.... SAT.... HCF.... MBC.... DMAT.M. GAT.... SVA.... HTO.... MBD.... DSNT.V. GAT.... SVA.... HTO.... MBD.... DLST.M. GVA.... SSA.... HCF.... MVI.... DCAT.M. GSA.... SSO.... HTO.... MMU.... DGRT.M. GSA.... SSO.... HTO.... MMU.... DALT.M. GSO.... SVA.... HCF.... MZA.... DSRT.M. GVA.... SSA.... HTO.... MMU.... DMAT.M. GVA.... SSA.... HTO.... MMU.... DSN.VM. GSA.... SSO.... HCF.... MMI.... DLST.M. GSO.... SVA.... HTO.... MVE.... DCAT.M. GSO.... SVA.... HTO.... MVE.... DGRT.M. GBD.... SSA.... HCF.... MFL.... DALT.M. GVI.... SSO.... HTO.... MVE.... DSRT.M. GVI.... SSO.... HTO.... MVE.... DMAT.V. GMU.... SBD.... HCF.... MFL.... DSNT.M. GMI.... SVI.... HTO.... MVE.... DLST.M. GMI.... SVI.... HTO.... MVE.... DCAT.M. GVE.... SMU.... HCF.... MGO.... DGRT.M. GFL.... SMI.... HTO.... MMR.... DALT.M. GFL.... SMI.... HTO.... MMR.... DMS..M. GRO.... SVE.... HCF.... MMS.... DAO..M. GBI.... SFL.... HTO.... MAO.... DNS..M. GBI.... SFL.... HTO.... MAO.... DHAT.M. GNP.... SRO.... HCF.... MCA.... DBRT.M. GRO.... SBI.... HTO.... MLS.... DPRT.M. GRO.... SBI.... HTO.... MLS.... DVIT... GMI.... SNP.... HCF.... MMA.... DBDT... GGE.... SRO.... HTO.... MBO.... DKLT... GGE.... SRO.... HTO.... MBO.... DCDT.M. GMI.... SMI.... HMR.... MCF.... DD1T.M. GVE.... SMU.... HGO.... MGE.... DGAT.M. GBDT... SZA.... HVE.... MMI.... DCNT.M. GKLT... SSZ.... HBD.... MGE.... DBS.... GGAT... SBD.... HBC.... MMR.... DIO.... GBC.... SBC.... HGA.... MGO.... DTS..M. GSZ.... SGA.... HBC.... MMI.... DMS..M. GKL.... SCD.... HBD.... MGE...."
# define xMsgs { "", "", "", "" }

#else

typedef HunterView View;

# define ViewNew HvNew
# define decideMove decideHunterMove
# define ViewFree HvFree

// # define xPastPlays "GZA.... SED.... HZU...."
// Test strings
# define xPastPlays "GMN.... SGA.... HFR.... MMA.... DCD.V.. GED.... SCDVD.. HMU.... MAL.... DD1T... GNS.... SCDTD.. HZA.... MMS.... DHIT... GHA.... SBE.... HSZ.... MTS.... DGAT... GBR.... SBC.... HGATD.. MIO.... DCNT... GPR.... SGA.... HSZ.... MBS.... DBS.... GVI.... SBC.... HKL.... MCNT..."
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
