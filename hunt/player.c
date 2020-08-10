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

# define xPastPlays "GMN.... SGA.... HFR.... MMA.... DCD.V.. GED.... SCDVD.. HMU.... MAL.... DD1T... GNS.... SCDTD.. HZA.... MMS.... DHIT... GHA.... SBE.... HSZ.... MTS.... DGAT... GBR.... SKL.... HGATD.. MIO.... DCNT... GPR.... SBC.... HKL.... MBS.... DBS.... GBD.... SCNT... HKL.... MCN.... DIO.... GCN.... SBS.... HBC.... MGA.... DAS.... GBC.... SCN.... HBD.... MBD.... DBIT.M. GSZ.... SGA.... HVE.... MVI.... DNPT... GVI.... SCD.... HMI.... MMU.... DHIT... GHA.... SCD.... HMI.... MMI.... DTS.... GNS.... SCD.... HMI.... MMR.... DMS.... GNS.... SCD.... HMI.... MTO.... DCG.V.. GNS.... SCD.... HMI.... MTO.... DD3..M. GNS.... SCD.... HMI.... MTO.... DIO..M. GNS.... SCD.... HMI.... MTO.... DBS..M. GNS.... SCD.... HMI.... MTO.... DCNT... GNS.... SCD.... HMI.... MTO.... DGAT... GNS.... SCD.... HMI.... MTO.... DCDT.V. GNS.... SCDTD.. HMI.... MTO.... DD1T... GNS.... SCDTD.. HMI.... MTO.... DHIT... GNS.... SBE.... HMI.... MTO.... DKLT... GNS.... SKLTD.. HMI.... MTO.... DBDT.M. GNS.... SCDT... HMI.... MTO.... DSZT.M. GNS.... SCD.... HMI.... MTO.... DZAT... GNS.... SCD.... HMI.... MTO.... DSJ.V.. GNS.... SCD.... HMI.... MTO.... DSOT... GNS.... SCD.... HMI.... MTO.... DVRT... GNS.... SCD.... HMI.... MTO.... DD2T.M. GNS.... SCD.... HMI.... MTO.... DBCT.M. GNS.... SCD.... HMI.... MTO.... DGAT.M. GNS.... SCD.... HMI.... MTO.... DCDT.V. GNS.... SCDTD.. HMI.... MTO.... DHIT.M. GNS.... SCDTD.. HMI.... MTO.... DKLT.M. GNS.... SBE.... HMI.... MTO.... DD4T.M. GNS.... SKLT... HMI.... MTO.... DCNT.M. GNS.... SCD.... HMI.... MTO.... DBS..M. GNS.... SCD.... HMI.... MTO.... DVRT... GNS.... SCD.... HMI.... MTO.... DSO.V.. GNS.... SCD.... HMI.... MTO.... DSJT... GNS.... SCD.... HMI.... MTO.... DZAT.M. GNS.... SCD.... HMI.... MTO.... DSZT.M. GNS.... SCD.... HMI.... MTO.... DBDT... GNS.... SCD.... HMI.... MTO.... DVIT.M. GNS.... SCD.... HMI.... MTO.... DPRT.V. GNS.... SCD.... HMI.... MTO.... DNUT.M. GNS.... SCD.... HMI.... MTO.... DD2T.M. GNS.... SCD.... HMI.... MTO.... DBRT.M. GNS.... SCD.... HMI.... MTO.... DLIT.M. GNS.... SCD.... HMI.... MTO.... DFRT.M. GNS.... SCD.... HMI.... MTO.... DHIT.M. GNS.... SCD.... HMI.... MTO.... DNU.VM. GNS.... SCD.... HMI.... MTO.... DPRT.M. GNS.... SCD.... HMI.... MTO.... DVIT.M. GNS.... SCD.... HMI.... MTO.... DZAT.M. GNS.... SCD.... HMI.... MTO.... DSZT.M. GNS.... SCD.... HMI.... MTO.... DBDT.M. GNS.... SCD.... HMI.... MTO.... DKLT.V. GNS.... SCD.... HMI.... MTO.... DGAT.M. GNS.... SCD.... HMI.... MTO.... DHIT.M. GNS.... SCD.... HMI.... MTO.... DD2T.M. GNS.... SCD.... HMI.... MTO.... DBCT.M. GNS.... SCD.... HMI.... MTO.... DSOT.M. GNS.... SCD.... HMI.... MTO.... DBET.M. GNS.... SCD.... HMI.... MTO.... DSZ.VM. GNS.... SCD.... HMI.... MTO.... DZAT.M. GNS.... SCD.... HMI.... MTO.... DSJT.M. GNS.... SCD.... HMI.... MTO.... DD2T.M. GNS.... SCD.... HMI.... MTO.... DBDT.M. GNS.... SCD.... HMI.... MTO.... DVIT.M. GNS.... SCD.... HMI.... MTO.... DPRT.V. GNS.... SCD.... HMI.... MTO.... DNUT.M. GNS.... SCD.... HMI.... MTO.... DFRT.M. GNS.... SCD.... HMI.... MTO.... DHIT.M. GNS.... SCD.... HMI.... MTO.... DD3T.M. GNS.... SCD.... HMI.... MTO.... DLIT.M. GNS.... SCD.... HMI.... MTO.... DBRT.M. GNS.... SKL.... HMU.... MBA.... DPR.VM. GED.... SBE.... HNUT... MMA.... DVIT.M. GED.... SSJ.... HPRV... MMA.... DMUT.M. GED.... SSJ.... HPR.... MMA.... DZUT... GED.... SSJ.... HPR.... MMA.... DHIT.M. GED.... SSJ.... HPR.... MMA.... DD2T.M. GED.... SSJ.... HPR.... MMA.... DMRT... GED.... SSJ.... HPR.... MMA.... DTOT.M. GED.... SSJ.... HPR.... MMA.... DCFT.M. GED.... SSJ.... HPR.... MMA.... DGET.M. GED.... SSJ.... HPR.... MMA.... DZUT.M. GED.... SSJ.... HPR.... MMA.... DHIT.M. GED.... SSJ.... HPR.... MMA.... DD2T.M. GED.... SSJ.... HPR.... MMA.... DMR.VM. GED.... SSJ.... HPR.... MMA.... DCFT.M. GED.... SSJ.... HPR.... MMA.... DGET.M. GED.... SSJ.... HPR.... MMA.... DZUT.M. GED.... SSJ.... HPR.... MMA.... DHIT.M. GED.... SSJ.... HPR.... MMA.... DD2T.M. GED.... SSJ.... HPR.... MMA.... DMIT.V. GED.... SSJ.... HPR.... MMA.... DVET.M. GED.... SSJ.... HPR.... MMA.... DMUT.M. GED.... SSJ.... HPR.... MMA.... DZUT.M. GED.... SSJ.... HPR.... MMA.... DHIT.M. GED.... SSJ.... HPR.... MMA.... DD2T.M. GED.... SSJ.... HPR.... MMA.... DMIT.M. GED.... SSJ.... HPR.... MMA.... DMR.VM. GED.... SSJ.... HPR.... MMA.... DTOT.M. GED.... SSJ.... HPR.... MMA.... DCFT.M. GED.... SSJ.... HPR.... MMA.... DGET.M. GED.... SSJ.... HPR.... MMA.... DHIT.M. GED.... SSJ.... HPR.... MMA.... DD2T.M. GED.... SSJ.... HPR.... MMA.... DZUT.V. GED.... SSJ.... HPR.... MMA.... DMRT.M. GED.... SSJ.... HPR.... MMA.... DCFT.M. GED.... SSJ.... HPR.... MMA.... DGET.M. GED.... SSJ.... HPR.... MMA.... DHIT.M. GED.... SSJ.... HPR.... MMA.... DD5T.M. GED.... SSJ.... HPR.... MMA.... DZUT.M. GED.... SSJ.... HPR.... MMA.... DMR.VM. GED.... SSJ.... HPR.... MMA.... DTOT.M. GED.... SSJ.... HPR.... MMA.... DCFT.M. GED.... SSJ.... HPR.... MMA.... DHIT.M. GMN.... SBE.... HFR.... MMA.... DD2T.M. GLV.... SKL.... HST.... MMA.... DNAT.M. GIR.... SKL.... HPA.... MMA.... DBB..V. GDU.... SKL.... HPA.... MMA.... DSNT.M. GDU.... SKL.... HPA.... MMA.... DSRT.M. GDU.... SKL.... HPA.... MMA.... DTOT.M. GDU.... SKL.... HPA.... MMA.... DMRT.M. GDU.... SKL.... HPA.... MMA.... DGOT.M. GDU.... SKL.... HPA.... MMA.... DHIT... GDU.... SKL.... HPA.... MMA.... DVE.VM. GDU.... SKL.... HPA.... MMA.... DFLT.M. GDU.... SKL.... HPA.... MMA.... DD1T.M. GDU.... SKL.... HPA.... MMA.... DROT.M. GDU.... SKL.... HPA.... MMA.... DNPT.M. GDU.... SKL.... HPA.... MMA.... DHIT.M. GDU.... SKL.... HPA.... MMA.... DBIT.V. GDU.... SKL.... HPA.... MMA.... DAS..M. GGW.... SBE.... HGE.... MGR.... DVET.M. GAO.... SSJ.... HMI.... MGR.... DD2..M. GAO.... SVA.... HVET... MGR.... DIO..M. GAO.... SVA.... HVE.... MGR.... DTS..M. GAO.... SVA.... HVE.... MGR.... DCGT.M. GAO.... SVA.... HVE.... MGR.... DHI.V.. GAO.... SVA.... HVE.... MGR.... DMS.... GAO.... SVA.... HVE.... MGR.... DBAT... GAO.... SVA.... HVE.... MGR.... DTOT... GAO.... SVA.... HVE.... MGR.... DD1T... GAO.... SVA.... HVE.... MGR.... DCFT.M. GAO.... SVA.... HVE.... MGR.... DHIT.V. GAO.... SVA.... HVE.... MGR.... DPAT... GAO.... SVA.... HVE.... MGR.... DBUT.M. GAO.... SVA.... HVE.... MGR.... DSTT.M. GAO.... SVA.... HVE.... MGR.... DD3T.M. GAO.... SVA.... HVE.... MGR.... DCFT.M. GAO.... SVA.... HVE.... MGR.... DHIT.M. GAO.... SVA.... HVE.... MGR.... DPA.VM. GAO.... SVA.... HVE.... MGR.... DBUT.M. GAO.... SVA.... HVE.... MGR.... DSTT.M. GAO.... SVA.... HVE.... MGR.... DD2T.M. GAO.... SVA.... HVE.... MGR.... DLET.M. GAO.... SVA.... HVE.... MGR.... DHIT.M. GAO.... SVA.... HVE.... MGR.... DPAT.V. GAO.... SVA.... HVE.... MGR.... DBUT.M. GAO.... SVA.... HVE.... MGR.... DSTT.M. GAO.... SVA.... HVE.... MGR.... DD2T.M. GAO.... SVA.... HVE.... MGR.... DLET.M. GAO.... SVA.... HVE.... MGR.... DHIT.M. GAO.... SVA.... HVE.... MGR.... DPAT.M. GAO.... SVA.... HVE.... MGR.... DBU.VM. GIR.... SIO.... HGO.... MCA.... DSTT.M. GLV.... SIO.... HMR.... MCA.... DNUT.M. GLV.... SIO.... HCF.... MCA.... DPRT.M. GLV.... SIO.... HCF.... MCA.... DVIT.M. GLV.... SIO.... HCF.... MCA.... DZAT.M. GLV.... SIO.... HCF.... MCA.... DSZT.V. GLV.... SIO.... HCF.... MCA.... DKLT.M. GLV.... SIO.... HCF.... MCA...."
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
