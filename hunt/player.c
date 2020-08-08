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

# define xPastPlays "GMN.... SGA.... HNU.... MTO.... DMU.V.. GED.... SBC.... HBR.... MBA.... DZAT... GLV.... SBE.... HHA.... MAL.... DHIT... GLO.... SKL.... HCO.... MGR.... DVIT... GPL.... SBD.... HAM.... MCA.... DPRT... GEC.... SPRTD.. HCO.... MAO.... DNUT... GNS.... SBR.... HFR.... MNS.... DMUT.V. GHA.... SPR.... HCO.... MHA.... DD5T.M. GLI.... SVIT... HFR.... MNUT... DHIT.M. GCO.... SBR.... HMUT... MMU.... DSZT... GFR.... SBD.... HVE.... MNU.... DBET... GMU.... SVI.... HFL.... MST.... DSOT... GNU.... SZAT... HVE.... MMU.... DBCT... GPR.... SSJ.... HSZT... MVI.... DGA.V.. GBD.... SBET... HBE.... MZAT... DCDT... GGAV... SBCT... HKL.... MBD.... DKLT... GCDT... SBE.... HBC.... MGA.... DBDT... GKLT... SKL.... HBE.... MKL.... DZAT.M. GBDT... SBD.... HKL.... MBD.... DSJT... GVI.... SSZ.... HBD.... MKL.... DSOT... GZAT... SZA.... HVE.... MBE.... DVRT... GBD.... SMU.... HBD.... MSJT... DCNT... GBE.... SZA.... HZA.... MVA.... DBS.... GSOT... SSZ.... HBD.... MSA.... DIO.... GSJ.... SBE.... HBC.... MVA.... DTS.... GBE.... SVRT... HCNT... MSO.... DROT... GBC.... SCN.... HBS.... MBC.... DFL.V.. GCN.... SGA.... HVR.... MCN.... DVET... GBC.... SCN.... HBE.... MBS.... DMUT... GGA.... SBC.... HVR.... MCN.... DZUT... GBC.... SBE.... HBS.... MBS.... DGET... GBE.... SSJ.... HVR.... MVR.... DPAT.M. GBE.... SSJ.... HVR.... MVR.... DNAT.V. GBC.... SVA.... HBS.... MBS.... DBB..M. GBD.... SIO.... HIO.... MIO.... DSNT.M. GZA.... SSA.... HSA.... MSA.... DSRT.M. GZA.... SSA.... HSA.... MSA.... DTOT.M. GMU.... SIO.... HIO.... MIO.... DCFT.M. GMI.... STS.... HTS.... MTS.... DHIT.M. GFL.... SMS.... HMS.... MMS.... DPA.V.. GFL.... SMS.... HMS.... MMS.... DSTT.M. GGO.... SAL.... HAO.... MAL.... DD1T.M. GMR.... SLS.... HBB.... MMA.... DNUT.M. GPAV... SAO.... HBO.... MBA.... DPRT.M. GGE.... SBB.... HBU.... MBO.... DHIT.M. GMR.... SBO.... HMR.... MCF.... DVIT... GBU.... SBU.... HCF.... MGE.... DZAT.M. GBO.... SLE.... HBO.... MSTT... DD1T... GCF.... SBU.... HBU.... MMU.... DSZT.M. GGE.... SCO.... HFR.... MST.... DKLT.M. GCF.... SNU.... HCO.... MBU.... DCDT.M. GMR.... SMU.... HFR.... MFR.... DHIT.M. GGO.... SVE.... HBR.... MMU.... DGA.VM. GVE.... SBD.... HBD.... MZAT... DCNT... GBD.... SGAV... HKLT... MBD.... DBS..M. GGA.... SCNT... HBE.... MBE.... DIO.... GCN.... SBS.... HBC.... MGA.... DVAT.M. GVR.... SCN.... HCN.... MBC.... DSJT.M. GBE.... SBS.... HGA.... MBE.... DZAT... GCN.... SCN.... HCN.... MBC.... DMUT... GVR.... SGA.... HBS.... MGA.... DZUT... GCN.... SCN.... HCN.... MCN.... DGET... GGA.... SSZ.... HBC.... MBC.... DPAT.M. GKL.... SKL.... HGA.... MSO.... DNAT.M. GKL.... SKL.... HGA.... MSO.... DBB..M. GBD.... SBE.... HBD.... MSJ.... DSN.VM. GVE.... SVI.... HSZ.... MZA.... DSRT.M. GFL.... SMU.... HKL.... MJM.... DHIT.M. GFL.... SMU.... HKL.... MJM.... DTOT.M. GGO.... SLI.... HBD.... MZA.... DCFT.M. GGE.... SBU.... HVI.... MMU.... DNAT... GPA.... SCO.... HPR.... MVE.... DBB..V. GPA.... SCO.... HPR.... MVE.... DSNT.M. GBO.... SBU.... HFR.... MGO.... DHIT.M. GBA.... SBO.... HCO.... MMR.... DLST.M. GAL.... SSR.... HBU.... MBO.... DCAT.M. GAL.... SSR.... HBU.... MBO.... DAO..M. GBA.... SBO.... HBO.... MNA.... DNS.... GBO.... SLE.... HCF.... MBB.... DHA.VM. GSR.... SEC.... HMR.... MAO.... DLIT.M. GSR.... SEC.... HMR.... MAO.... DHIT.M. GMA.... SAO.... HSR.... MBB.... DNUT.M. GCA.... SCA.... HSN.... MBO.... DPRT... GAO.... SAO.... HLS.... MPA.... DD1T... GAO.... SAO.... HLS.... MPA.... DVIT.V. GNS.... SNS.... HAO.... MST.... DZAT.M. GHA.... SHA.... HNS.... MFR.... DHIT.M. GNS.... SCO.... HED.... MST.... DSZT.M. GNS.... SCO.... HED.... MST.... DKLT.M. GHA.... SHA.... HNS.... MMU.... DCDT.M. GBR.... SVIT... HHA.... MVE.... DD1T... GBD.... SBD.... HBR.... MBD.... DGA.VM. GKLT... SKL.... HVI.... MGAVD.. DCNT.M. GBC.... SCDTT.. HBC.... MCNTD.."
# define xMsgs { "", "", "", "" }

#else

typedef HunterView View;

# define ViewNew HvNew
# define decideMove decideHunterMove
# define ViewFree HvFree

// # define xPastPlays "GZA.... SED.... HZU...."
// Test strings
# define xPastPlays "GMN.... SGA.... HNU.... MTO.... DPA.V.. GED.... SBC.... HBR.... MBA.... DSTT... GLV.... SBE.... HHA.... MAL.... DHIT... GLO.... SKL.... HCO.... MGR.... DZUT... GPL.... SBD.... HAM.... MCA.... DD3T... GEC.... SPR.... HBU.... MLS.... DMUT... GLE.... SBR.... HPAV... MMA.... DZAT... GBO.... SHA.... HBU.... MBA.... DSZT.M. GCF.... SCO.... HMR.... MPA.... DHIT.M. GBO.... SBU.... HCO.... MBU.... DKLT.M. GBB.... SLI.... HAM.... MCO.... DCDT.M. GAO.... SBR.... HBU.... MFR.... DD1T.M. GMS.... SPR.... HFR.... MMU.... DGAT.M. GTS.... SBD.... HMU.... MZA.... DCN.VM. GIO.... SGAT... HVI.... MBD.... DBS..M. GBS.... SCDTT.. HBD.... MBE.... DIO..M. GCNV... SGA.... HBE.... MCN.... DTS.... GBS.... SBC.... HGA.... MVR...."
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
