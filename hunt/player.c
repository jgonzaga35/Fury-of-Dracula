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

# define xPastPlays "GMN.... SGA.... HNU.... MTO.... DKL.V.. GED.... SBC.... HBR.... MBA.... DCDT... GNS.... SGA.... HPR.... MMS.... DD1T... GHA.... SKLV... HVI.... MTS.... DGAT... GBR.... SGATD.. HBD.... MIO.... DCNT... GPR.... SCDTT.. HGA.... MBS.... DBS.... GBD.... SBE.... HCD.... MCNT... DIO.... GCN.... SSO.... HGA.... MBS.... DATT... GBC.... SVR.... HCD.... MCN.... DVAT... GCN.... SBE.... HGA.... MBS.... DSJT... GBC.... SVR.... HKL.... MCN.... DZAT... GCN.... SCN.... HBE.... MGA.... DVIT... GBS.... SBC.... HVR.... MCD.... DPRT... GVR.... SCN.... HBE.... MGA.... DNU.VM. GBS.... SBD.... HKL.... MKL.... DSTT.M. GVR.... SZAT... HBD.... MBE.... DZUT.M. GSO.... SMU.... HBE.... MBD.... DMRT... GBE.... SZA.... HVIT... MVI.... DTOT... GBD.... SMU.... HPRT... MPR.... DSRT... GBR.... SNUV... HBR.... MBR.... DHIT... GLI.... SMU.... HPR.... MSTT... DMAT... GCO.... SMI.... HFR.... MBU.... DGRT.M. GPA.... SMU.... HST.... MCO.... DCAT.M. GLE.... SST.... HMU.... MFR.... DAO..M. GLE.... SST.... HMU.... MFR.... DLST.M. GBU.... SBU.... HMI.... MCO.... DSNT.M. GBO.... SBO.... HMR.... MBU.... DBB..M. GAL.... SSR.... HMS.... MBO.... DNAT.M. GAL.... SSR.... HMS.... MBO.... DLET.M. GMA.... SAL.... HAO.... MSR.... DHIT... GGR.... SLST... HBB.... MMA.... DEC.... GCA.... SSNT... HBO.... MLS.... DPLT... GAO.... SMA.... HBB.... MSR.... DLOT... GLS.... SBO.... HSN.... MBO.... DMNT.M. GAL.... SPA.... HSR.... MBB.... DEDT.M. GLS.... SBU.... HAL.... MSN.... DNS..M. GLS.... SBU.... HAL.... MSN.... DHAT... GAO.... SLE.... HLS.... MBB.... DHIT.M. GEC.... SEC.... HAO.... MBO.... DLIT.M. GNS.... SNS.... HGW.... MMR.... DNU.VM. GNS.... SNS.... HGW.... MMR.... DPRT.M. GED.... SED.... HAO.... MMS.... DVIT... GMN.... SNS.... HNS.... MAO.... DZAT.M. GSW.... SAO.... HAO.... MGW.... DHIT.M. GSW.... SAO.... HAO.... MGW.... DSZT.M. GIR.... SNS.... HNS.... MAO.... DKLT.V. GAO.... SHA.... HAM.... MNS.... DCDT.M. GMS.... SBR.... HBU.... MHA.... DD1T.M. GTS.... SPR.... HFR.... MVI.... DGAT.M. GIO.... SBD.... HMU.... MBD.... DCNT.M. GBS.... SGAT... HVI.... MKLT... DBS..M. GCNT... SCN.... HBD.... MBE.... DIO.... GBS.... SGA.... HBE.... MCN.... DTS..M. GCN.... SCN.... HCN.... MBC.... DMS..M. GVR.... SGA.... HBS.... MGA.... DAO.... GBE.... SCN.... HVR.... MCN.... DNS.... GBC.... SGA.... HBS.... MVR.... DHAT... GGA.... SBC.... HCN.... MBS.... DLIT... GCN.... SVI.... HBS.... MIO.... DNUT... GBC.... SBD.... HIO.... MAS.... DSTT... GBC.... SBD.... HIO.... MAS.... DZUT... GSZ.... SPR.... HTS.... MIO.... DMIT... GVI.... SHAT... HGO.... MTS.... DMRT... GHA.... SBR.... HFL.... MGO.... DTOT.M. GCO.... SHA.... HVE.... MMRT... DSRT.M. GBU.... SCO.... HGO.... MCF.... DHI.VM. GMR.... SMR.... HMR.... MGE.... DMAT.M. GTOT... SCF.... HMS.... MMR.... DGRT.M. GBA.... SBO.... HMR.... MCF.... DCAT... GSRTV.. SBA.... HMS.... MBO.... DAO.... GMAT... SLS.... HAL.... MBB.... DGWT... GCAT... SCA.... HMA.... MAO.... DDUT... GGRT... SGR.... HLS.... MCA.... DIR.... GSZ.... SCA.... HAL.... MLS.... DSWT... GVI.... SGR.... HMS.... MMA.... DLOT... GHA.... SCA.... HAL.... MCA.... DPLT... GNS.... SLS.... HGR.... MMA.... DEC..M. GED.... SAL.... HAL.... MAL.... DLET.M. GLV.... SSR.... HGR.... MGR.... DHI.V.. GED.... SAL.... HAL.... MAL.... DBUT.M. GED.... SAL.... HAL.... MAL.... DSTT.M. GMN.... SLS.... HLS.... MMS.... DD1T.M. GLO.... SAO.... HAO.... MAO.... DZUT... GMN.... SIR.... HIR.... MIR.... DMUT.M. GMN.... SIR.... HIR.... MIR.... DZAT.V. GED.... SAO.... HAO.... MAO.... DHIT.M. GNS.... SEC.... HBB.... MBB.... DSZT.M. GAM.... SPL.... HAO.... MAO.... DKLT.M. GAM.... SPL.... HAO.... MAO.... DCDT.M. GBU.... SEC.... HMS.... MMS.... DD1T.M. GFR.... SNS.... HTS.... MTS.... DGAT.M. GMU.... SHA.... HIO.... MIO.... DCN.VM. GZA.... SBR.... HBS.... MBS.... DBS..M. GSZ.... SVI.... HCNV... MCN.... DIO..M. GBE.... SBC.... HBS.... MGAT... DTS..M. GVR.... SGA.... HCN.... MCN.... DMS..M. GCN.... SCN.... HBC.... MVR.... DAO.... GGA.... SBS.... HGA.... MCN.... DNS.... GCN.... SCN.... HCN.... MBC.... DHAT... GVR.... SGA.... HBS.... MGA.... DLIT... GVR.... SGA.... HBS.... MGA.... DNUT... GBS.... SCN.... HIO.... MCN.... DSTT... GIO.... SBS.... HTS.... MBS.... DZUT... GSA.... SCN.... HNP.... MCN.... DMIT... GSA.... SCN.... HNP.... MCN.... DVE.VM. GSO.... SSZ.... HFL.... MBC.... DAS..M. GSZ.... SPR.... HGO.... MSZ.... DBIT.M. GKL.... SNU.... HVEV... MBD.... DROT.M. GBD.... SFR.... HMU.... MVE.... DTS..M. GVI.... SST.... HVE.... MMU.... DCGT.M. GVE.... SMU.... HFL.... MVE.... DMS.... GAS.... SVI.... HMI.... MFL.... DBAT... GVE.... SVE.... HMU.... MBIT... DTOT... GFL.... SFL.... HVE.... MAS.... DHIT.M. GNP.... SRO.... HAS.... MIO.... DSRT... GMI.... SBI.... HIO.... MSA.... DMAT.M. GMI.... SBI.... HIO.... MSA.... DD1T... GGO.... SNP.... HTS.... MIO.... DGR.VM. GMR.... STS.... HMS.... MTS.... DCAT.M. GGE.... SNP.... HMR.... MNP.... DHIT.M. GGE.... SNP.... HMR.... MNP.... DLST.M. GCF.... STS.... HMS.... MTS.... DMAT.M. GBO.... SGO.... HBA.... MMS.... DSNT.M. GCO.... SMR.... HAL.... MMR.... DBB..V. GCO.... SMR.... HAL.... MMR.... DNAT.M. GAM.... SMS.... HLST... MMS.... DHIT.M. GNS.... SAO.... HMAT... MAL.... DPAT... GAO.... SCA.... HSNT... MSR.... DSTT... GBB.... SLS.... HMA.... MBO.... DD1T... GBO.... SAO.... HLS.... MSR.... DZUT... GBB.... SCA.... HMA.... MAL.... DMU.VM. GAO.... SAO.... HGR.... MSN.... DZAT.M. GAO.... SAO.... HGR.... MSN.... DHIT.M. GBB.... SEC.... HAL.... MBB.... DSZT.M. GBO.... SLE.... HMS.... MBO.... DKLT.M. GCO.... SBU.... HMR.... MBA.... DCDT.M. GHA.... SCO.... HMI.... MMS.... DD1T.V. GBR.... SFR.... HMU.... MTS.... DGAT.M. GVI.... SMU.... HZAT... MIO.... DCNT... GBC.... SZA.... HSZT... MBS.... DBS.... GGAT... SBD.... HGA.... MCNT... DIO..M. GCN.... SBE.... HCN.... MBS.... DTS..M. GBC.... SGA.... HVR.... MCN.... DMS..M. GCN.... SBC.... HCN.... MVR.... DAO.... GGA.... SCN.... HBC.... MBS.... DNS.... GCN.... SGA.... HCN.... MCN.... DHAT... GVR.... SBC.... HGA.... MBS.... DLIT... GSO.... SSO.... HCD.... MCN.... DNUT... GSO.... SSO.... HCD.... MCN.... DSTT... GSA.... SSA.... HGA.... MBS.... DZUT... GIO.... SIO.... HCN.... MIO.... DMIT... GSA.... SSA.... HBC.... MSA.... DVET.M. GSA.... SSA.... HBC.... MSA.... DFLT.M. GSO.... SBE.... HVI.... MIO.... DROT.M. GSZ.... SVI.... HPR.... MTS.... DBIT.M. GVR.... SMU.... HNU.... MNP.... DAS..M. GVR.... SMU.... HNU.... MNP.... DIO..M. GBS.... SMI.... HMU.... MTS.... DVAT.M. GIO.... SMR.... HMI.... MGO.... DSOT.M. GSA.... SGE.... HGE.... MGE.... DBCT.M. GSA.... SGE.... HGE.... MGE.... DHIT.M. GIO.... SFL.... HRO.... MMR.... DGAT... GAS.... SNP.... HBI.... MMI.... DCDT... GIO.... STS.... HAS.... MVE.... DKLT.M. GBS.... SIO.... HIO.... MBD.... DBET.M. GCN.... SBS.... HBS.... MKLT... DSJT.M. GBCT... SCN.... HCN.... MCDT... DZAT... GBET... SBC.... HBC.... MGAT... DVIT... GSJT... SBE.... HSO.... MBD.... DBDT... GZAT... SKL.... HBDTD.. MVIT... DSZ.V.. GSZVD.. SBC.... HKL.... MBD.... DBET... GZA.... SBETD.. HBED... MBED... DBCT... GSZ.... SSO.... HBE.... MBE.... DGAT... GKL.... SBE.... HSJ.... MBCT... DCDT... GGAT... SKL.... HBE.... MGA.... DKLT... GBD.... SCDT... HBC.... MBC.... DSZT... GKLT... SKL.... HGA.... MGA.... DZAT... GBE.... SBD.... HCD.... MBC.... DMUT... GKL.... SBE.... HKL.... MBE.... DZUT... GCD.... SKL.... HBC.... MBD.... DGET... GKL.... SCD.... HBE.... MBC.... DPAT... GBD.... SGA.... HGA.... MGA.... DNAT.M. GBE.... SBD.... HCD.... MBC.... DCF.VM. GSJ.... SKL.... HGA.... MSZ.... DTOT.M. GSJ.... SKL.... HGA.... MSZ.... DSRT.M. GZA.... SBD.... HBD.... MZA.... DMAT.M. GMU.... SVE.... HVI.... MMU.... DGRT.M. GVE.... SFL.... HMU.... MMI.... DHIT.M. GVE.... SFL.... HMU.... MMI.... DCAT.V. GGO.... SGE.... HST.... MMR.... DLST.M. GMR.... SCF.... HPA.... MCF.... DSNT.M. GLE.... SBO.... HBU.... MBO...."
# define xMsgs { "", "", "", "" }

#else

typedef HunterView View;

# define ViewNew HvNew
# define decideMove decideHunterMove
# define ViewFree HvFree

// # define xPastPlays "GZA.... SED.... HZU...."
// Test strings
# define xPastPlays "GMN.... SGA.... HFR.... MMA.... DCD.V.. GED.... SCDVD.. HMU.... MAL.... DD1T... GNS.... SCDTD.. HZA.... MMS.... DHIT... GHA.... SBE.... HSZ.... MTS.... DGAT... GBR.... SBC.... HGATD.. MIO.... DCNT... GPR.... SGA.... HKL.... MBS.... DBS.... GVI.... SCNT... HGA...."
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
