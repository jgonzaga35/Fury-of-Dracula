////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////
#include <time.h>
#include "dracula.h"
#include "DraculaView.h"
#include "Game.h"
#include "Map.h"
#include "Places.h"
#include "Queue.h"
#include "GameView.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TRUE			1
#define FALSE			0
#define NO_EXISTENCE	-1

	/* Strategy Outline 
	For the first round, Dracula picks STRASBOURG as the initial location 
	as it has the most road connections.

	For every other round:
	- Get an array of all the locations Dracula can go to.
	- Get an array of all the locations Hunters can go to.
	- Remove common elements from Dracula's valid location array
	so that the array only contains locations that are "safe",
	e.g. cannot be reached by hunters in the next round. 
	- Decide on a move (using some kind of priority system)

	IDEAS
	- prioritise Castle Dracula when low on health to get blood points.
	- if there is a high chance of encountering a hunter, try to go to the sea instead.*/
//----------------Prototypes-----------------------
void removeRiskyLocs(PlaceId *ValidLocs, PlaceId *riskyLocs, int *numValidMoves, int *numRiskyLocs);
static char *convertMove(PlaceId trail[TRAIL_SIZE], PlaceId location, PlaceId draculaLoc);
static int indexInTrail(PlaceId trail[TRAIL_SIZE], PlaceId location);
PlaceId MoveToLocation(PlaceId *pastLocs, PlaceId location, int *numPastLocs);


// TODO: At the moment, 
// dracula.c cannot handle large strings due to 
// dracula: malloc.c:2385: sysmalloc: 
// Assertion `(old_top == initial_top (av) && old_size == 0) || ((unsigned long) (old_size) >= MINSIZE && 
// prev_inuse (old_top) && ((unsigned long) old_end & (pagesize - 1)) == 0)' failed.
// This error only applies to large strings
// Cant figure out why though...


void decideDraculaMove(DraculaView dv)
{
int health = DvGetHealth(dv, PLAYER_DRACULA);  					// Dracula's Blood Points.
	int numValidMoves = 0;						  					// Number of Valid Locations for Dracula.	
	int numRiskyLocs = 0;					      					// Number of Risky Locations for Dracula.
	int numPastLocs = 0;											// Number of Past Locations in Dracula's move history.
							    // Dracula's trail
	PlaceId hunterLocs[4];                                          // Array of current hunter locations.
	for (int player = 0; player < 4; player++) {
		hunterLocs[player] = DvGetPlayerLocation(dv, player);
	}
	Round round = DvGetRound(dv);				  					// The current round in the game.

	time_t t;
	srand((unsigned) time(&t));										// seed for random movements.  
	int riskLevel[NUM_REAL_PLACES] = {0};							// Array containing risk levels for each place. 

	// Needs to be freed
	char *play = malloc(sizeof(char) *2); 		  					// The play to be made.

	// Dracula picks STRASBOURG for the first round
	// as this has the most road connections (8) and thus
	// it is hard for the hunters to corner him. 
	if (round == 0) {
		registerBestPlay("ST", "Mwahahahaha");

		return;
	}

	// If Dracula has no valid moves, use TELEPORT.
	PlaceId *validMoves = DvGetValidMoves(dv, &numValidMoves);
	if (numValidMoves == 0) {
		registerBestPlay("TP", "I love COMP2521");

		return;
	}

	/* JUST PICK THE FIRST VALID MOVE AND RETURN */
	// strcpy(play, placeIdToAbbrev(validMoves[0]));
	// registerBestPlay(play, "COMP2521 > COMP1511");
	// return;


	// Go to Castle Dracula if possible - Dracula wants to gain 10 BP.
	// Even if a hunter is there, it will be an even exchange. 
	PlaceId *pastLocs = DvGetLocationHistory(dv, &numPastLocs);  
	for (int i = 0; i < numValidMoves; i++) {
		if (MoveToLocation(pastLocs, validMoves[i], &numPastLocs) == CASTLE_DRACULA && health >= 25) {
			strcpy(play, placeIdToAbbrev(validMoves[i]));
			registerBestPlay(play, "COMP2521 > COMP1511");

			return;
		} 
	}

	// Assign risk levels to each place.
	for (int player = 0; player < 4; player++) {
		// Locations reachable by hunters through ROAD have a risk of 2.
		PlaceId *riskyLocsRoad = DvWhereCanTheyGoByType(dv, player, true, false, true, &numRiskyLocs);
		for (int i = 0; i < numRiskyLocs; i++) {
			riskLevel[riskyLocsRoad[i]] += 2;
		}
	}
	
	// Dracula should prioritise places with traps in them to stack traps.
	int numTraps = 0;
	PlaceId *TrapLocs = DvGetTrapLocations(dv, &numTraps);
	for (int i = 0; i < numTraps; i++) {
		riskLevel[TrapLocs[i]] -= -1;
	}

	// Dracula should avoid places where he has left a vampire to stop hunters
	// from activating it prematurely.
	PlaceId vampLoc = DvGetVampireLocation(dv);
	riskLevel[vampLoc] += 1;

	// Assign a risk level to each location 
	for (int i = 0; i < NUM_REAL_PLACES; i++) {
		// Sea locations have risk of 1.
		if (placeIsSea(i)) {
			riskLevel[i] += 1;
		}
		// TODO: Locations with a small number of road connections should increase risk by 1.
	}

	// FIND THE MOVES WITH THE MINIMUM RISK LEVEL
	int min = riskLevel[validMoves[0]];
	PlaceId *lowRiskMoves = malloc(sizeof(PlaceId) *numValidMoves);
	int lowRiskNum = 0;
	for (int i = 0; i < numValidMoves; i++) {
		// If the risk level of the location in ValidMoves[i] <= min
		if (riskLevel[MoveToLocation(pastLocs, validMoves[i], &numPastLocs)] <= min) {
			min = riskLevel[MoveToLocation(pastLocs, validMoves[i], &numPastLocs)];
			lowRiskMoves[lowRiskNum] = validMoves[i];
			lowRiskNum++;
		}
	}	
	
	// If the lowest risk move is still "risky":
	if (riskLevel[MoveToLocation(pastLocs, lowRiskMoves[0], &numPastLocs)] > 0) {
		if (health <= 20 && health >= 4) {
			int i = rand() % (lowRiskNum);
			// registerBestPlay(convertMove(trail, validMoves[i], draculaLoc), "Mwahahahaha");
			strcpy(play, placeIdToAbbrev(lowRiskMoves[i]));
			registerBestPlay(play, "mwahahahah");

			return;
		}
		// If dracula is healthy go to the player's current location.
		validMoves = DvGetValidMoves(dv, &numValidMoves); 
		if (health >= 35 || health <= 5) {
			for (int player = 0; player < 4; player++) {
				for (int i = 0; i < numValidMoves; i++) {
					if (MoveToLocation(pastLocs, validMoves[i], &numPastLocs) == hunterLocs[player]) {
						strcpy(play, placeIdToAbbrev(validMoves[i]));
						registerBestPlay(play, "mwahahahhaa");
						//registerBestPlay(convertMove(trail, hunterLocs[player], draculaLoc), "Mwahahahaha");
						return;
					}
				}
			}
		}

		// Default: Dracula picks a random risky location.
		int i;
		if (lowRiskNum != 0) {
			i = rand() % (lowRiskNum);
		} else {
			i = 0;
		}
		//registerBestPlay(convertMove(trail, validLocs[random], draculaLoc), "Mwahahahaha");
		
		strcpy(play, placeIdToAbbrev(lowRiskMoves[i]));
		
		registerBestPlay(play, "Mwahahahaha");
		return;

	}

	// Default: choose a random location
	// If Dracula can go to a location that is not "risky":
	// Go to random location in ValidLocs (not necessarily a good move!)
	// for (int i = 0; i < lowRiskNum; i++) {
	// 	printf("lowRiskMoves[%d] is %s\n", i, placeIdToName(lowRiskMoves[i]));
	// }
	int i;
	if (lowRiskNum != 0) {
		i = rand() % (lowRiskNum);
	} else {
		i = 0;
	}
	//registerBestPlay(convertMove(trail, validLocs[random], draculaLoc), "Mwahahahaha");
	strcpy(play, placeIdToAbbrev(lowRiskMoves[i]));
	registerBestPlay(play, "Mwahahahaha");
	return;
}

// Converts the move to location
PlaceId MoveToLocation(PlaceId *pastLocs, PlaceId location, int *numPastLocs) {

	if (location == HIDE)
		location = pastLocs[*numPastLocs - 1];
	else if (location >= DOUBLE_BACK_1 && location <= DOUBLE_BACK_5) 
	{
		if (location == DOUBLE_BACK_1) {
			location = pastLocs[*numPastLocs - 1]; 
		}
		if (location == DOUBLE_BACK_2) {
			location = pastLocs[*numPastLocs - 2]; 
		}
		if (location == DOUBLE_BACK_3) {
			location = pastLocs[*numPastLocs - 3]; 
		}
		if (location == DOUBLE_BACK_4) {
			location = pastLocs[*numPastLocs - 4]; 
		}
		if (location == DOUBLE_BACK_5) {
			location = pastLocs[*numPastLocs - 5]; 
		}
	} 
	return location;
}




	// Get reachable locations by road ("risky locations" for each hunter and remove these from Dracula's
	// valid locations array (he wants to avoid these locations as much as possible).
	// for (int player = 0; player < 4; player++) {
	// 	// riskyLocs should never be null as hunters always have a valid move.
	// 	PlaceId *riskyLocs = DvWhereCanTheyGoByType(dv, player, true, false, true, &numRiskyLocs);
	// 	for (int i = 0; i < numValidMoves; i++) {
	// 		// Compare with each risky location.
	// 		for (int j = 0; j < numRiskyLocs; j++) {
	// 			if (MoveToLocation(pastLocs, validMoves[i], &numPastLocs) == riskyLocs[j]) {
	// 				printf("validMoves[%d] (%s) is a riskyLoc!\n", i, placeIdToAbbrev(validMoves[i]));
	// 				// Remove location from ValidLocs if it is a riskyLoc.
	// 				for (int c = i; c < numValidMoves - 1; c++) {
	// 					validMoves[c] = validMoves[c + 1];
	// 				}
	// 				numValidMoves = numValidMoves - 1;
	// 			}
	// 		}
	// 	}
	// 	free(riskyLocs);
	// }

	// Find the valid moves with the lowest risk:
	// for (int i = 0; i < numValidMoves; i++) {
	// 	PlaceId Loc = MoveToLocation(pastLocs, validMoves[i], &numPastLocs);
	// 	printf("validMoves[%d] is %s and has risk of %d\n", i, placeIdToName(validMoves[i]), riskLevel[Loc]);
	// }




 

	// // If all of Dracula's valid locations are "risky" locations:
	// if (numValidMoves == 0) {
	// 	// If Dracula is low on health try to go to sea so he does not encounter hunters.
	// 	if (health <= 20 && health >= 6) {
	// 		validMoves = DvWhereCanIGoByType(dv, false, true, &numValidMoves);
	// 		if (validMoves != NULL) {
	// 			int i = rand() % (numValidMoves);
	// 			registerBestPlay(convertMove(trail, validMoves[i], draculaLoc), "Mwahahahaha");
	// 			//strcpy(play, placeIdToAbbrev(validMoves[random]));
	// 			free(play);
	// 			free(pastLocs);
	// 			free(validMoves);
	// 			//registerBestPlay(play, "jas is best lecturer");
	// 			return;
	// 		} 
	// 	} 

	// 	// If Dracula is healthy or low on health (desperate), 
	// 	// go to the hunter's current location.
	// 	// According to the rules, hunters only encounter Drac if they 
	// 	// arrive to the city last, so they won't encounter Drac.
	// 	// So this might work - its either a very good idea or very bad idea
	// 	validMoves = DvGetValidMoves(dv, &numValidMoves); 
	// 	if (health >= 35 || health <= 5) {
	// 		for (int player = 0; player < 4; player++) {
	// 			for (int i = 0; i < numValidMoves; i++) {
	// 				if (MoveToLocation(pastLocs, validMoves[i], &numPastLocs) == hunterLocs[player]) {
	// 					printf("going to player location!\n");
	// 					strcpy(play, placeIdToAbbrev(validMoves[i]));
	// 					registerBestPlay(play, "mwahahahhaa");
	// 					//registerBestPlay(convertMove(trail, hunterLocs[player], draculaLoc), "Mwahahahaha");
	// 					free(play);
	// 					free(pastLocs);
	// 					free(validMoves);
	// 					//registerBestPlay(play, "give me marks");
	// 					return;
	// 				}
	// 			}
	// 		}
	// 	}
	// 	// Default: Dracula picks a random risky location.
	// 	int i = rand() % (numValidMoves);
	// 	//registerBestPlay(convertMove(trail, validLocs[random], draculaLoc), "Mwahahahaha");
	// 	strcpy(play, placeIdToAbbrev(validMoves[i]));
	// 	free(play);
	// 	free(pastLocs);
	// 	free(validMoves);
	// 	//registerBestPlay(play, "Mwahahahaha");
	// 	return;
	// }

	// // Default: choose a random location
	// // If Dracula can go to a location that is not "risky":
	// // Go to random location in ValidLocs (not necessarily a good move!)
	// if (numValidMoves == 0) validMoves = DvGetValidMoves(dv, &numValidMoves); 
	// printf("now the valid moves are:\n");
	// for (int i = 0; i < numValidMoves; i++) {
		
	// 	printf("validMoves[%d] is %s\n", i, placeIdToName(validMoves[i]));
	// }
	// int i = rand() % (numValidMoves);
	// //registerBestPlay(convertMove(trail, validLocs[random], draculaLoc), "Mwahahahaha");
	// strcpy(play, placeIdToAbbrev(validMoves[i]));
	// registerBestPlay(play, "Mwahahahaha");
	// free(play);
	// free(pastLocs);
	// free(validMoves);
	// return;