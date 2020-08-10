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

# define xPastPlays "GMN.... SGA.... HFR.... MMA.... DCD.V.. GED.... SCDVD.. HMU.... MAL.... DD1T... GNS.... SCDTD.. HZA.... MMS.... DHIT... GHA.... SBE.... HSZ.... MTS.... DGAT... GBR.... SKL.... HGATD.. MIO.... DCNT... GPR.... SBC.... HKL.... MBS.... DBS.... GBD.... SCNT... HKL.... MCN.... DIO.... GCN.... SBS.... HBC.... MGA.... DTS.... GBC.... SIO.... HBD.... MBD.... DMS..M. GSZ.... SVA.... HPR.... MVI.... DALT... GVI.... SSO.... HVI.... MPR.... DSRT... GHA.... SVA.... HPR.... MHA.... DMAT... GNS.... SSO.... HBD.... MNS.... DLST... GED.... SVA.... HPR.... MAO.... DCA.V.. GED.... SVA.... HPR.... MLST... DAO.... GNS.... SIO.... HBR.... MAO.... DNS..M. GED.... SVA.... HPR.... MLS.... DEC..M. GNS.... SIO.... HHA.... MAO.... DPLT.M. GED.... SVA.... HBR.... MLS.... DLOT... GNS.... SIO.... HPR.... MCAV... DMNT... GED.... SVA.... HHA.... MLS.... DLVT... GNS.... SIO.... HPR.... MCA.... DIR.... GED.... SVA.... HPR.... MLS.... DDUT... GLOT... SIO.... HBR.... MAO.... DGWT.M. GMNT... SVA.... HPR.... MLS.... DAO.... GED.... SIO.... HVE.... MSN.... DNS.... GLVT... STS.... HGO.... MBB.... DHA.V.. GED.... SMS.... HFL.... MSN.... DBRT... GED.... SAO.... HNP.... MBB.... DPRT.M. GMN.... SGWT... HBI.... MSN.... DVIT... GED.... SDU.... HNP.... MBB.... DBDT... GNS.... SGW.... HBI.... MSN.... DKLT... GED.... SGW.... HBI.... MSN.... DCDT.V. GNS.... SAO.... HAS.... MBB.... DD1T.M. GED.... SGW.... HBI.... MSN.... DHIT.M. GNS.... SAO.... HAS.... MSR.... DGAT.M. GED.... SGW.... HBI.... MSN.... DCNT.M. GNS.... SAO.... HAS.... MBB.... DBS..M. GED.... SGW.... HBI.... MSN.... DIO..M. GNS.... SAO.... HAS.... MSR.... DTS..M. GED.... SGW.... HBI.... MSN.... DMS..M. GNS.... SAO.... HAS.... MBB.... DALT.M. GED.... SGW.... HBI.... MSN.... DSRT.M. GMN.... SAO.... HNP.... MALT... DMAT... GED.... SGW.... HBI.... MSN.... DSNT... GNS.... SAO.... HFL.... MSNTD.. DBB.... GED.... SGW.... HRO.... MBB.... DAO.... GNS.... SAO.... HBI.... MSN.... DNS.... GED.... SGW.... HNP.... MBB.... DHAT.M. GNS.... SAO.... HBI.... MSN.... DBRT.M. GED.... SGW.... HNP.... MLS.... DPRT... GNS.... SAO.... HBI.... MSN.... DVIT... GED.... SGW.... HNP.... MLS.... DBD.V.. GMN.... SAO.... HBI.... MSN.... DKLT... GED.... SGW.... HBI.... MSN.... DCDT.M. GNS.... SAO.... HAS.... MSR.... DD1T.M. GED.... SGW.... HBI.... MSN.... DHIT.M. GNS.... SAO.... HAS.... MBB.... DGAT.M. GED.... SGW.... HBI.... MSN.... DCNT.V. GNS.... SAO.... HAS.... MSR.... DBS..M. GED.... SGW.... HBI.... MSN.... DIO..M. GNS.... SAO.... HAS.... MBB.... DTS..M. GED.... SGW.... HBI.... MSN.... DMS..M. GNS.... SAO.... HAS.... MSR.... DALT.M. GED.... SGW.... HBI.... MSN.... DSRT.M. GMN.... SAO.... HRO.... MLS.... DBO.V.. GED.... SGW.... HBI.... MSN.... DBB.... GMN.... SAO.... HNP.... MALT... DAO.... GED.... SGW.... HBI.... MSN.... DNS.... GNS.... SAO.... HNP.... MBB.... DHAT... GED.... SGW.... HBI.... MSN.... DBRT.M. GNS.... SAO.... HNP.... MAL.... DPRT.V. GED.... SGW.... HBI.... MSN.... DVIT... GMN.... SAO.... HRO.... MLS.... DBDT... GED.... SGW.... HBI.... MSN.... DKLT... GMN.... SAO.... HNP.... MAL.... DCDT.M. GED.... SGW.... HBI.... MSN.... DD1T.M. GNS.... SAO.... HAS.... MBB.... DHIT.M. GED.... SGW.... HBI.... MSN.... DGA.VM. GNS.... SAO.... HAS.... MSR.... DCNT.M. GED.... SGW.... HBI.... MSN.... DBS..M. GNS.... SAO.... HAS.... MBB.... DIO..M. GED.... SGW.... HBI.... MSN.... DTS..M. GNS.... SAO.... HAS.... MSR.... DMS..M. GED.... SGW.... HBI.... MSN.... DALT.V. GNS.... SAO.... HAS.... MBB.... DSRT.M. GAO.... SMS.... HVE.... MSN.... DSNT... GGW.... SMR.... HVI.... MALT... DBB.... GAO.... SGO.... HBR.... MAL.... DAO.... GGW.... SVE.... HHA.... MMA.... DNS.... GAO.... SBD.... HBR.... MAL.... DEC.... GGW.... SKL.... HHA.... MSNT... DPL.VM. GAO.... SBD.... HBR.... MAL.... DLOT... GGW.... SKL.... HHA.... MMA.... DMNT... GAO.... SBD.... HBR.... MAL.... DLVT... GGW.... SKL.... HHA.... MBA.... DHIT... GGW.... SKL.... HBR.... MAL.... DD2T... GDU.... SCD.... HBD.... MMS.... DSWT.V. GGW.... SKL.... HVI.... MAL.... DLOT.M. GDU.... SCD.... HPR.... MBA.... DMNT.M. GGW.... SKL.... HBR.... MAL.... DLVT.M. GDU.... SCD.... HBD.... MMS.... DHIT.M. GGW.... SKL.... HVI.... MAL.... DD2T.M. GDU.... SCD.... HPR.... MBA.... DSWT.M. GGW.... SKL.... HBR.... MAL.... DLO.VM. GDU.... SCD.... HBD.... MMS.... DMNT.M. GGW.... SKL.... HVI.... MAL.... DLVT.M. GDU.... SCD.... HPR.... MBA.... DHIT.M. GGW.... SKL.... HBR.... MAL.... DD2T.M. GDU.... SCD.... HBD.... MMS.... DSWT.M. GGW.... SKL.... HVI.... MAL.... DLOT.V. GDU.... SCD.... HPR.... MBA.... DMNT.M. GGW.... SKL.... HBR.... MAL.... DLVT.M. GDU.... SCD.... HBD.... MMS.... DHIT.M. GGW.... SKL.... HVI.... MAL.... DD2T.M. GDU.... SCD.... HPR.... MBA.... DSWT.M. GGW.... SKL.... HBR.... MAL.... DLOT.M. GDU.... SCD.... HBD.... MMS.... DMN.VM. GGW.... SKL.... HVI.... MAL.... DLVT.M. GDU.... SCD.... HPR.... MBA.... DHIT.M. GGW.... SKL.... HBR.... MAL.... DD2T.M. GDU.... SCD.... HBD.... MMS.... DSWT.M. GGW.... SKL.... HVI.... MAL.... DLOT.M. GDU.... SCD.... HPR.... MBA.... DMNT.V. GGW.... SKL.... HBR.... MAL.... DLVT.M. GDU.... SCD.... HBD.... MMS.... DHIT.M. GGW.... SKL.... HVI.... MAL.... DD2T.M. GDU.... SCD.... HPR.... MBA.... DSWT.M. GGW.... SKL.... HBR.... MAL.... DLOT.M. GDU.... SCD.... HBD.... MMS.... DMNT.M. GIR.... SCD.... HVI.... MAO.... DLV.VM. GLVTTVD SKL.... HVE.... MCA.... DIR.... GIR.... SCD.... HPR.... MAO.... DDUT... GLV.... SGA.... HVE.... MCA.... DGWT.M. GLV.... SCD.... HGO.... MAO.... DAO..M. GIR.... SKL.... HVE.... MCA.... DNS..M. GLV.... SCD.... HGO.... MAO.... DHAT... GSW.... SGA.... HVE.... MCA.... DBRT... GLV.... SCD.... HGO.... MAO.... DPRT.M. GSW.... SKL.... HVE.... MCA.... DNUT.M. GLV.... SCD.... HVE.... MCA.... DFRT... GIR.... SKL.... HFL.... MMA.... DHIT... GLV.... SCD.... HVE.... MCA.... DD2T.M. GSW.... SKL.... HGO.... MMA.... DLI.VM. GLV.... SCD.... HVE.... MCA.... DHAT.M. GIR.... SKL.... HFL.... MMA.... DBRT.M. GLV.... SCD.... HVE.... MCA.... DPRT.M. GSW.... SKL.... HGO.... MMA.... DVIT.M. GLV.... SCD.... HVE.... MCA.... DZAT.M. GIR.... SKL.... HFL.... MMA.... DSJT.V. GLV.... SCD.... HVE.... MCA.... DSOT.M. GSW.... SKL.... HGO.... MMA.... DBCT.M. GLV.... SCD.... HVE.... MCA.... DKLT.M. GIR.... SKLTD.. HBD.... MAO.... DCDT.M. GLV.... SCDTD.. HVI.... MCA.... DGAT.M. GED.... SBE.... HVE.... MAO.... DCNT.M. GMN.... SKL.... HBD.... MCA.... DBS..M. GLV.... SCD.... HVE.... MAO.... DIO..M. GED.... SKL.... HMU.... MCA.... DTS.... GLV.... SCD.... HVE.... MAO.... DMS.... GIR.... SKL.... HBD.... MCA.... DALT.M. GLV.... SCD.... HVE.... MAO.... DSRT.M. GLV.... SCD.... HVE.... MCA.... DMAT... GSW.... SKL.... HGO.... MMATD.. DGRT... GIR.... SBE.... HMI.... MALT... DHIT... GAO.... SSA.... HMR.... MSJ.... DD2T... GNS.... SIO.... HMI.... MVA.... DCAT... GAO.... STS.... HMR.... MIO.... DLST.M. GNS.... SIO.... HMI.... MTS.... DMAT... GAO.... STS.... HMR.... MMS.... DGR.VM. GNS.... SIO.... HMI.... MAO.... DHIT.M. GED.... SVA.... HMR.... MMS.... DD5T.M. GNS.... SIO.... HMI.... MMS.... DCAT.M. GED.... SVA.... HZU.... MTS.... DLST.M. GNS.... SIO.... HMI.... MMS.... DSNT.M. GED.... SVA.... HMR.... MTS.... DMAT.V. GNS.... SIO.... HMI.... MMS.... DGRT.M. GED.... SVA.... HZU.... MTS.... DHIT.M. GNS.... SIO.... HMI.... MMS.... DD2T.M. GED.... SVA.... HMR.... MTS.... DCAT.M. GNS.... SIO.... HMI.... MMS.... DLST.M. GED.... SVA.... HZU.... MTS.... DMAT.M. GNS.... SIO.... HMI.... MMS.... DGR.VM. GED.... SVA.... HMR.... MTS.... DHIT.M. GNS.... SIO.... HMI.... MMS.... DD2T.M. GED.... SVA.... HZU.... MTS.... DCAT.M. GNS.... SIO.... HMI.... MMS.... DLST.M. GED.... SVA.... HMR.... MTS.... DMAT.M. GNS.... SIO.... HMI.... MMS.... DGRT.V. GED.... SVA.... HZU.... MTS.... DHIT.M. GNS.... SIO.... HMI.... MMS.... DD2T.M. GED.... SVA.... HMR.... MTS.... DCAT.M. GNS.... SIO.... HMI.... MMS.... DLST.M. GED.... SVA.... HZU.... MTS.... DMAT.M. GNS.... SIO.... HMI.... MMS.... DGRT.M. GAO.... SBS.... HGE.... MAL.... DHI.VM. GGW.... SCN.... HPA.... MMAT... DD5T.M. GDU.... SSZ.... HMA.... MCATTD."
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
