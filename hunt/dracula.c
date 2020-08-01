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

#include "dracula.h"
#include "DraculaView.h"
#include "Game.h"
#include "Map.h"
#include "Places.h"
#include "Queue.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void removeRiskyLocs(PlaceId *validLocs, PlaceId *riskyLocs, int *numValidLocs, int *numRiskyLocs);
void decideDraculaMove(DraculaView dv)
{
	
	Round round = DvGetRound(dv);	// The current round in the game.
	char *play;						// The play to be made.

	// First Round
	if (round == 0) {
		registerBestPlay("ST", "Mwahahahaha");
		return;
	}

	// Get possible locations for Dracula. Return TELEPORT if no valid moves.
	int numValidLocs = 0;
	PlaceId *validLocs = DvWhereCanIGo(dv, &numValidLocs);
	if (numValidLocs == 0) {
		printf("no valid locs\n");
		free(validLocs);
		registerBestPlay("TP", "I love COMP2521");
		return;
	}

	// Go to Castle Dracula if possible - Dracula wants to gain 10 BP.
	// Even if a hunter is there, it will be an even exchange. 
	for (int i = 0; i < numValidLocs; i++) {
		if (validLocs[i] == CASTLE_DRACULA) {
			registerBestPlay("CD", "COMP2521 > COMP1511");
			return;
		} 
	}

	// Get reachable locations by road ("risky locations" for each hunter and remove these from Dracula's
	// valid locations array (he wants to avoid these locations as much as possible).
	int numRiskyLocs = 0;
	for (int player = PLAYER_LORD_GODALMING; player < PLAYER_MINA_HARKER; player++) {
		// riskyLocs should never be null as hunters always have a valid move.
		PlaceId *riskyLocs = DvWhereCanTheyGoByType(dv, player, true, false, true, &numRiskyLocs);
		removeRiskyLocs(validLocs, riskyLocs, &numValidLocs, &numRiskyLocs);
		free(riskyLocs);
	}

	// If all of Dracula's valid locations are also "risky" locations:
	if (numValidLocs == 0) {
		// If possible go to the SEA to avoid encountering a hunter.
		// (Its better to lose 2 BP at SEA than lose 10 BP to a hunter!)
		validLocs = DvWhereCanIGoByType(dv, false, true, &numValidLocs);
		if (validLocs != NULL) {
			int index = rand() % (numValidLocs + 1);
			strcpy(play, placeIdToAbbrev(validLocs[index]));
			free(validLocs);
			registerBestPlay(play, "jas is best lecturer");
			return;
		}
		
		// Otherwise choose random location in the original validLocs (not necessarily a good move!)
		validLocs = DvWhereCanIGo(dv, &numValidLocs);
		numRiskyLocs = 0;
		for (int player = PLAYER_LORD_GODALMING; player < PLAYER_MINA_HARKER; player++) {
			// riskyLocs should never be null as hunters always have a valid move.
			PlaceId *riskyLocs = DvWhereCanTheyGoByType(dv, player, false, true, false, &numRiskyLocs);
			removeRiskyLocs(validLocs, riskyLocs, &numValidLocs, &numRiskyLocs);
			free(riskyLocs);
		}
	}


	// If Dracula can go to a location that is not "risky":
	// Go to random location in validLocs (not necessarily a good move!)
	int index = rand() % (numValidLocs + 1);
	strcpy(play, placeIdToAbbrev(validLocs[index]));
	free(validLocs);
	registerBestPlay(play, "Mwahahahaha");
	return;
}

void removeRiskyLocs(PlaceId *validLocs, PlaceId *riskyLocs, int *numValidLocs, int *numRiskyLocs) {
	// For each valid location of Dracula:
	for (int i = 0; i < *numValidLocs; i++) {
		// Compare with each risky location.
		for (int j = 0; j < *numRiskyLocs; j++) {
			if (validLocs[i] == riskyLocs[j]) {
				// Remove location from validLocs if it is a riskyLoc.
				for (int c = i; c < *numValidLocs - 1; c++) {
					validLocs[c] = validLocs[c + 1];
				}
				*numValidLocs = *numValidLocs - 1;
			}
		}
	}
	return;
}


