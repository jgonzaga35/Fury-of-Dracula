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

# define xPastPlays "GMN.... SGA.... HFR.... MMA.... DKL.V.. GED.... SCD.... HBR.... MAL.... DCDT... GNS.... SCDTD.. HPR.... MMS.... DGAT... GHA.... SKLV... HVI.... MTS.... DCNT... GBR.... SCD.... HSZ.... MTS.... DBS.... GPR.... SCD.... HVR.... MGO.... DIO.... GVI.... SCD.... HCNT... MTS.... DTS.... GSZ.... SGAT... HVR.... MTS.... DMS.... GBE.... SCN.... HSA.... MTS.... DALT... GBC.... SSZ.... HVR.... MIO.... DSRT... GGA.... SVI.... HCN.... MBS.... DHIT... GSZ.... SBD.... HBC.... MBS.... DD2T... GSZ.... SBD.... HBC.... MBS.... DTOT... GBE.... SPR.... HGA.... MIO.... DCF.V.. GVR.... SVI.... HCN.... MTS.... DPAT.M. GVR.... SVI.... HCN.... MTS.... DGET.M. GBS.... SMU.... HSZ.... MTS.... DHIT.M. GIO.... SNU.... HVI.... MGO.... DD4T.M. GIO.... SST.... HMU.... MMR.... DTOT.M. GIO.... SGETT.. HMI.... MLE.... DBAT.V. GTS.... SCFT... HZU.... MBU.... DSRT.M. GGO.... SCF.... HLI.... MPA.... DMAT... GZU.... SCF.... HCO.... MBO.... DGRT... GRO.... SGE.... HBU.... MSRT... DHIT... GFL.... SCF.... HCO.... MLS.... DD2T.M. GGO.... SCF.... HST.... MAO.... DALT.M. GMI.... SCF.... HGE.... MMS.... DSR.V.. GFR.... SBO.... HCF.... MMS.... DMAT.M. GCO.... SSRV... HBO.... MBA.... DGRT.M. GBU.... SLS.... HPA.... MTO.... DHIT.M. GFR.... SSN.... HCF.... MBA.... DD2T.M. GZU.... SMAT... HBO.... MBO.... DCAT.M. GMR.... SCATD.. HMA.... MLS.... DAO.... GMS.... SGRT... HGRTT.. MAO.... DNS.... GMS.... SJM.... HCA.... MCA.... DHAT... GMS.... SSJ.... HLS.... MCA.... DHIT... GAL.... SVA.... HSN.... MCA.... DD2T... GMS.... SVA.... HBO.... MLS.... DLIT... GMS.... SVA.... HSR.... MCA.... DNUT... GMS.... SIO.... HAL.... MCA.... DPR.V.. GBA.... SBS.... HMS.... MLS.... DVIT.M. GSR.... SCN.... HBA.... MMA.... DMUT.M. GSR.... SCN.... HBA.... MMA.... DZAT.M. GPA.... SBC.... HMS.... MLS.... DSJT.M. GST.... SSZ.... HMS.... MBO.... DD2T.M. GST.... SSZ.... HMS.... MBO.... DBDT.V. GZU.... SVIT... HMR.... MCF.... DKLT... GST.... SMUT... HGO.... MCF.... DCDT... GMU.... SVE.... HVE.... MMR.... DGAT.M. GVE.... SBDT... HBD.... MGO.... DCNT.M. GPR.... SSO.... HKLT... MVE.... DVRT.M. GNU.... SBE.... HGAT... MVI.... DBS.... GMU.... SBC.... HCNT... MPR.... DIO.... GVI.... SBE.... HGA.... MVI.... DTS..M. GBR.... SVRT... HBC.... MBD.... DCGT... GST.... SBS.... HSO.... MVI.... DHIT... GMU.... SCN.... HSA.... MPR.... DD2T... GVE.... SBC.... HVR.... MVI.... DMS.... GPR.... SBE.... HCN.... MBD.... DALT... GVE.... SKL.... HGA.... MKL.... DSRT... GVE.... SKL.... HGA.... MKL.... DSNT.M. GGO.... SKL.... HCN.... MBE.... DHIT.M. GZU.... SKL.... HBS.... MSJ.... DD2T.M. GST.... SBE.... HIO.... MSJ.... DLST... GGE.... SSJ.... HIO.... MSJ.... DCAT.M. GMR.... SBE.... HBS.... MBE.... DMA.VM. GMR.... SBE.... HBS.... MBE.... DSNT.M. GPA.... SKL.... HIO.... MVR.... DHIT.M. GCF.... SBD.... HIO.... MSO.... DD2T.M. GBO.... SVI.... HIO.... MSA.... DLST.M. GPA.... SSZ.... HTS.... MIO.... DCAT.M. GBO.... SJM.... HGO.... MAT.... DGRT.V. GBO.... SJM.... HGO.... MAT.... DMAT.M. GSR.... SJM.... HZU.... MIO.... DSNT.M. GPA.... SJM.... HGE.... MTS...."
# define xMsgs { "", "", "", "" }

#else

typedef HunterView View;

# define ViewNew HvNew
# define decideMove decideHunterMove
# define ViewFree HvFree

// # define xPastPlays "GZA.... SED.... HZU...."
// Test strings
# define xPastPlays "GMN.... SGA.... HFR.... MMA.... DCD.V.. GED.... SCDVD.. HMU.... MAL.... DD1T... GNS.... SCDTD.. HZA.... MMS.... DHIT... GHA.... SBE.... HSZ.... MTS.... DGAT... GBR.... SKL.... HGATD.. MIO.... DCNT... GPR.... SBE.... HCDT... MBS.... DBS.... GLI...."
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
