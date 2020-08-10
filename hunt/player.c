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

# define xPastPlays "GST.... SSR.... HBC.... MCD.... DST.V.. GFR.... SPA.... HVI.... MKL.... DZUT... GSTV... SST.... HMU.... MBD.... DMRT... GCO.... SMU.... HST.... MPR.... DTOT... GST.... SST.... HGE.... MFR.... DSRT... GMU.... SPA.... HST.... MST.... DBAT... GST.... SST.... HFR.... MZUT... DMS.... GZU.... SZU.... HST.... MMU.... DAO.... GZU.... SZU.... HST.... MMU.... DNS..M. GZU.... SMR.... HGE.... MMI.... DHAT.M. GZU.... SMR.... HGE.... MMI.... DBRT.M. GZU.... SMS.... HCF.... MMR.... DPRT.M. GZU.... SMS.... HCF.... MMR.... DVIT... GZU.... SAO.... HBO.... MMS.... DBD.V.. GZU.... SAO.... HBO.... MMS.... DKLT... GZU.... SNS.... HPA.... MAO.... DCDT.M. GMI.... SHA.... HST.... MMS.... DD1T.M. GVE.... SBR.... HMU.... MMR.... DGAT.M. GBDV... SBD.... HVIT... MGO.... DCNT... GGAT... SKLT... HBD.... MVE.... DBS.... GCDTT.. SGA.... HBE.... MBD.... DIO.... GGA.... SBC.... HGA.... MKL.... DTS.... GKL.... SGA.... HCNT... MBC.... DMS.... GBC.... SCN.... HGA.... MCN.... DALT... GCN.... SGA.... HCN.... MVR.... DSRT... GCN.... SGA.... HCN.... MVR.... DMAT... GCN.... SCN.... HBS.... MBS.... DLS.V.. GCN.... SCN.... HBS.... MBS.... DCAT... GCN.... SBS.... HIO.... MIO.... DGRT... GCN.... SBS.... HIO.... MIO.... DALT.M. GCN.... SIO.... HTS.... MTS.... DSRT.M. GCN.... SIO.... HTS.... MTS.... DMAT.M. GCN.... STS.... HMS.... MMS.... DLST.V. GCN.... STS.... HMS.... MMS.... DAO..M. GCN.... SMS.... HALT... MAL.... DMS..M. GBS.... SAL.... HMS.... MGR.... DALT... GIO.... SMAT... HALTD.. MALD... DSRT.M. GTS.... SAL.... HMS.... MSRTD.. DTOT... GMS.... SSR.... HAL.... MZA.... DMRT.M. GAL.... SBO.... HSR.... MMU.... DD2.V.. GSR.... SSR.... HMA.... MMI.... DCFT... GBO.... SMA.... HSR.... MMRT... DGET... GMR.... SAL.... HTOT... MTOV... DZUT... GTO.... SSR.... HZA.... MMR.... DSTT... GSR.... STO.... HMU.... MTO.... DBUT... GTO.... SSR.... HMI.... MBO.... DAMT... GTO.... SSR.... HMI.... MBO.... DNS..M. GTO.... SBO.... HGET... MCF.... DHAT... GTO.... SBO.... HGE.... MCF.... DLIT.M. GTO.... SBUT... HPA.... MPA.... DBRT.M. GBO.... SPA.... HBU.... MBU.... DPRT... GBO.... SPA.... HBU.... MBU.... DHIT.M. GBO.... SBU.... HAM.... MAM.... DD2.V.. GBO.... SBU.... HAM.... MAM.... DVIT.M. GBO.... SLIT... HCO.... MCO.... DZAT... GBO.... SLI.... HCO.... MCO.... DSZT.M. GBO.... SBR.... HFR.... MBR.... DKLT.M. GPA.... SPRTV.. HPR.... MPR.... DGAT... GCO.... SNU.... HNU.... MBR.... DCDT... GBR.... SMU.... HPR.... MPR.... DHIT.M. GPR.... SVE.... HBD.... MBD.... DD2T.M. GVI.... SBD.... HGAT... MKLT... DTP..M. GBD.... SGA.... HCDTTTD MCDD... DKLT... GGA.... SCD.... HBE.... MGA.... DBDT... GCD.... SKLT... HKL.... MKL.... DVIT... GKL.... SBE.... HSZ.... MCD.... DPR.V.. GBC.... SKL.... HKL.... MKL.... DNUT... GKL.... SGA.... HSZ.... MSZ.... DHIT... GBC.... SKL.... HKL.... MKL.... DD2T... GBC.... SKL.... HKL.... MKL.... DSTT.M. GBC.... SBD.... HBD.... MBD.... DZUT.M. GBC.... SBD.... HBD.... MBD.... DGET.V. GBC.... SVI.... HPR.... MPR.... DPAT.M. GBE.... SBR.... HNUTT.. MNU.... DCFT... GBD.... SNU.... HFR.... MFR.... DHIT... GBD.... SNU.... HFR.... MFR.... DTOT.M. GBD.... SFR.... HZUT... MZU.... DSRT... GBD.... SFR.... HZU.... MZU.... DMAT.M. GBD.... SPAT... HGE.... MGE.... DSN.V.. GBD.... SPA.... HGE.... MGE.... DBB..M. GBD.... SCFT... HCF.... MCF.... DAO.... GBD.... SCF.... HCF.... MCF.... DNS..M. GBD.... SBO.... HBO.... MBO.... DHAT.M. GBD.... SBO.... HBO.... MBO.... DBRT.M. GBD.... SBB.... HBA.... MSNV... DHIT... GBD.... SBB.... HBA.... MSN.... DD3T... GBD.... SAO.... HMS.... MBB.... DCOT... GBD.... SAO.... HMS.... MBB.... DSTT... GBD.... SNS.... HAO.... MAO.... DZUT.M. GVI.... SHAT... HNS.... MNS.... DMRT.M. GBRT... SCOT... HAM.... MAM.... DHIT... GCO.... SLI.... HCO.... MCO.... DMS.... GHA.... SCO.... HAM.... MBU.... DALT... GHA.... SCO.... HAM.... MBU.... DSRT.M. GHA.... SZUT... HBU.... MST.... DMAT... GHA.... SZU.... HBU.... MST.... DSNT.M. GHA.... SMRT... HMR.... MGE.... DBB.... GHA.... SMR.... HMR.... MGE.... DAO.... GHA.... SMS.... HMS.... MCF.... DNS..M. GHA.... SMS.... HMS.... MCF.... DEC..M. GHA.... SAL.... HAL.... MBO.... DPLT.M. GHA.... SAL.... HAL.... MBO.... DHIT.M. GHA.... SBO.... HMA.... MBB.... DD2T... GHA.... SBO.... HMA.... MBB.... DLOT... GHA.... SBB.... HCA.... MAO.... DSW.V.. GHA.... SBB.... HCA.... MAO.... DLVT... GHA.... SAO.... HAO.... MEC.... DMNT.M. GNS.... SEC.... HEC.... MPLTT.. DEDT... GEC.... SPL.... HPL.... MLOT... DHIT... GLO.... SLO.... HLO.... MMNT... DNS.... GMN.... SMN.... HMN.... MMN.... DHAT.V. GEDTT.. SED.... HED.... MED.... DD1T.M. GMN.... SNS.... HNS.... MMN...."
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
