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


//----------------Prototypes-----------------------
void removeRiskyLocs(PlaceId *ValidLocs, PlaceId *riskyLocs, int *numValidLocs, int *numRiskyLocs);
static char *convertMove(PlaceId trail[TRAIL_SIZE], PlaceId location, PlaceId draculaLoc);
static int indexInTrail(PlaceId trail[TRAIL_SIZE], PlaceId location);

PlaceId MoveToLocation(PlaceId *pastLocs, PlaceId location, int *pastNum);
void decideDraculaMove(DraculaView dv)
{
	Round round = DvGetRound(dv);				  // The current round in the game.
	char *play = (char *)malloc(sizeof(char) *2);							  // The play to be made.
	int health= DvGetHealth(dv, PLAYER_DRACULA);  // Dracula's Blood Points.
	int numValidLocs = 0;						  // Number of Valid Locations for Dracula.	
	int numRiskyLocs = 0;					      // Number of Risky Locations for Dracula.
	PlaceId draculaLoc = DvGetPlayerLocation(dv, PLAYER_DRACULA);	// Current location of Dracula
	PlaceId *trail = DvGetTrail(dv);				// Dracula's trail
	PlaceId hunterLocs[4];                        // Array of current hunter locations.
	for (int player = 0; player < 4; player++) {
		hunterLocs[player] = DvGetPlayerLocation(dv, player);
	}
	time_t t;
	srand((unsigned) time(&t));
	int pastNum = 0;
	PlaceId *pastLocs = DvGetLocationHistory(dv, &pastNum);
	// for (int i = 0; i < pastNum; i++) {
	// 	printf("pastLocs[%d] is %s\n", i, placeIdToName(pastLocs[i]));
	// }

	// for (int i = 0; i < 6; i++) {
	// 	printf("trail[%d] is %s\n", i, placeIdToName(trail[i]));
	// }


	// PlaceId *validOptions = DvGetValidMoves(dv, &numValidMoves);
	// for (int i = 0; i < numValidMoves; i++) {
	// 	printf("validMoves[%d] is %s ", i, placeIdToName(validOptions[i]));
	// 	if (isDoubleBack(validOptions[i])) {
			
	// 		printf("to %s", placeIdToName(MoveToLocation(pastLocs, validOptions[i], &numReturnedLocs)));
	// 		printf(" and %s", placeIdToName(validOptions[i]));
	// 	}
	// 	printf("\n");
	// }

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


	// Dracula chooses STRASBOURG as the initial location
	// as this city has the most road locations - feel free to change
	// if you think there is a better strategy, e.g. pick based on hunter's current locations. 
	if (round == 0) {
		registerBestPlay("ST", "Mwahahahaha");
		free(play);
		free(pastLocs);
		return;
	}

	// Get possible locations for Dracula. Return TELEPORT if no valid moves.
	int numValidMoves = 0;
	PlaceId *validMoves = DvGetValidMoves(dv, &numValidMoves);
	if (numValidMoves == 0) {
		free(validMoves);
		registerBestPlay("TP", "I love COMP2521");
		free(play);
		free(pastLocs);
		free(validMoves);
		return;
	}
	
	// Go to Castle Dracula if possible - Dracula wants to gain 10 BP.
	// Even if a hunter is there, it will be an even exchange. 
	for (int i = 0; i < numValidMoves; i++) {
		if (MoveToLocation(pastLocs, validMoves[i], &pastNum) == CASTLE_DRACULA && health >= 25) {
			strcpy(play, placeIdToAbbrev(validMoves[i]));
			registerBestPlay(play, "COMP2521 > COMP1511");
			free(play);
			free(pastLocs);
			free(validMoves);
			return;
		} 
	}

	// Get reachable locations by road ("risky locations" for each hunter and remove these from Dracula's
	// valid locations array (he wants to avoid these locations as much as possible).
	for (int player = 0; player < 4; player++) {
		// riskyLocs should never be null as hunters always have a valid move.
		PlaceId *riskyLocs = DvWhereCanTheyGoByType(dv, player, true, false, true, &numRiskyLocs);
		for (int i = 0; i < numValidMoves; i++) {
			// Compare with each risky location.
			for (int j = 0; j < numRiskyLocs; j++) {
				if (MoveToLocation(pastLocs, validMoves[i], &pastNum) == riskyLocs[j]) {
					// Remove location from ValidLocs if it is a riskyLoc.
					for (int c = i; c < numValidMoves - 1; c++) {
						validMoves[c] = validMoves[c + 1];
					}
					numValidMoves = numValidMoves - 1;
				}
			}
		}
		free(riskyLocs);
	}
	
	// for (int i = 0; i < numValidMoves; i++) {
		
	// 	printf("validMoves[%d] is %s\n", i, placeIdToName(validMoves[i]));
	// }
	// printf("numValidMoves is %d\n", numValidMoves);

	// If all of Dracula's valid locations are "risky" locations:
	if (numValidMoves == 0) {
		// If Dracula is low on health try to go to sea so he does not encounter hunters.
		if (health <= 20 && health >= 6) {
			validMoves = DvWhereCanIGoByType(dv, false, true, &numValidMoves);
			if (validMoves != NULL) {
				int i = rand() % (numValidMoves);
				registerBestPlay(convertMove(trail, validMoves[i], draculaLoc), "Mwahahahaha");
				//strcpy(play, placeIdToAbbrev(validMoves[random]));
				free(play);
				free(pastLocs);
				free(validMoves);
				//registerBestPlay(play, "jas is best lecturer");
				return;
			} 
		} 

		// If Dracula is healthy or low on health (desperate), 
		// go to the hunter's current location.
		// According to the rules, hunters only encounter Drac if they 
		// arrive to the city last, so they won't encounter Drac.
		// So this might work - its either a very good idea or very bad idea
		validMoves = DvGetValidMoves(dv, &numValidMoves); 
		if (health >= 35 || health <= 5) {
			for (int player = 0; player < 4; player++) {
				for (int i = 0; i < numValidMoves; i++) {
					if (MoveToLocation(pastLocs, validMoves[i], &pastNum) == hunterLocs[player]) {
						strcpy(play, placeIdToAbbrev(validMoves[i]));
						registerBestPlay(play, "mwahahahhaa");
						//registerBestPlay(convertMove(trail, hunterLocs[player], draculaLoc), "Mwahahahaha");
						free(play);
						free(pastLocs);
						free(validMoves);
						//registerBestPlay(play, "give me marks");
						return;
					}
				}
			}
		}
		// Default: Dracula picks a random risky location.
		int i = rand() % (numValidMoves);
		//registerBestPlay(convertMove(trail, validLocs[random], draculaLoc), "Mwahahahaha");
		strcpy(play, placeIdToAbbrev(validMoves[i]));
		free(play);
		free(pastLocs);
		free(validMoves);
		//registerBestPlay(play, "Mwahahahaha");
		return;
	}

	// Default: choose a random location
	// If Dracula can go to a location that is not "risky":
	// Go to random location in ValidLocs (not necessarily a good move!)
	if (numValidMoves != 0) validMoves = DvGetValidMoves(dv, &numValidMoves); 

	int i = rand() % (numValidMoves);
	//registerBestPlay(convertMove(trail, validLocs[random], draculaLoc), "Mwahahahaha");
	strcpy(play, placeIdToAbbrev(validMoves[i]));
	registerBestPlay(play, "Mwahahahaha");
	free(play);
	free(pastLocs);
	free(validMoves);
	return;
}
 
// Return the correct name of the move by considering hide and double back
static char *convertMove(PlaceId trail[TRAIL_SIZE], PlaceId location, PlaceId draculaLoc)
{	
	int i;
	int doubleBack = FALSE;
	int hide = FALSE;
	for (i = 0; i < TRAIL_SIZE - 1; i++)
	{
		if (isDoubleBack(trail[i])) doubleBack = TRUE;
		if (trail[i] == HIDE) hide = TRUE;
	}

	// Determine D1 or HIDE
	if (doubleBack && location == draculaLoc) return (char *) placeIdToAbbrev(HIDE);
	if (hide && location == draculaLoc) return (char *) placeIdToAbbrev(DOUBLE_BACK_1);

	// Determine whether D2-5
	int random = indexInTrail(trail, location);
	if (random != NO_EXISTENCE) return (char *) placeIdToAbbrev(102 + random);
	else return (char *) placeIdToAbbrev(location);
}

// Return the random of location in Dracula's trail, -1 if it doesn't exist
static int indexInTrail(PlaceId trail[TRAIL_SIZE], PlaceId location)
{	
	int i;
	// The last move is irrelevant as it will be dropped
	for (i = 0; i < TRAIL_SIZE - 1; i++)
	{
		if (trail[i] == location) return i + 1;
	}
	return NO_EXISTENCE;
}

// Converts the move to location
PlaceId MoveToLocation(PlaceId *pastLocs, PlaceId location, int *pastNum) {

	if (location == HIDE)
		location = pastLocs[*pastNum - 1];
	else if (isDoubleBack(location)) 
	{
		switch (location) 
		{
			case DOUBLE_BACK_1: location = pastLocs[*pastNum - 1]; break;
			case DOUBLE_BACK_2: location = pastLocs[*pastNum - 2]; break;
			case DOUBLE_BACK_3: location = pastLocs[*pastNum - 3]; break;
			case DOUBLE_BACK_4: location = pastLocs[*pastNum - 4]; break;
			case DOUBLE_BACK_5: location = pastLocs[*pastNum - 5]; break;
		}
	} 
	return location;
}