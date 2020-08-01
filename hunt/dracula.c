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

void removeRiskyLocs(PlaceId *ValidLocs, PlaceId *riskyLocs, int *numValidLocs, int *numRiskyLocs);
void decideDraculaMove(DraculaView dv)
{
	Round round = DvGetRound(dv);				  // The current round in the game.
	char *play;									  // The play to be made.
	int health= DvGetHealth(dv, PLAYER_DRACULA); // Dracula's Blood Points.
	int numValidLocs = 0;
	int numRiskyLocs = 0;

	// Dracula chooses STRASBOURG as the initial location.
	if (round == 0) {
		registerBestPlay("ST", "Mwahahahaha");
		return;
	}

	// Get possible locations for Dracula. Return TELEPORT if no valid moves.
	PlaceId *validLocs = DvWhereCanIGo(dv, &numValidLocs);
	if (numValidLocs == 0) {
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
	for (int player = 0; player < 4; player++) {
		// riskyLocs should never be null as hunters always have a valid move.
		PlaceId *riskyLocs = DvWhereCanTheyGoByType(dv, player, true, false, true, &numRiskyLocs);
		removeRiskyLocs(validLocs, riskyLocs, &numValidLocs, &numRiskyLocs);
		free(riskyLocs);
	}

	// If all of Dracula's valid locations are "risky" locations:
	if (numValidLocs == 0) {
		// If Dracula is low on health try to go to sea so he does not encounter hunters.
		if (health <= 20 && health >= 6) {
			validLocs = DvWhereCanIGoByType(dv, false, true, &numValidLocs);
			if (validLocs != NULL) {
				int index = rand() % (numValidLocs + 1);
				strcpy(play, placeIdToAbbrev(validLocs[index]));
				free(validLocs);
				registerBestPlay(play, "jas is best lecturer");
				return;
			} 
		} 

		// If Dracula is healthy or low on health (desperate), 
		// go to the hunter's current location.
		// According to the rules, hunters only encounter Drac if they 
		// arrive to the city last, so they won't encounter Drac.
		// So this might work - its either a very good idea or very bad idea
		if (health >= 35 || health <= 5) {
			int numPotentialLocs = 0;
			PlaceId *potentialLocs = DvWhereCanIGo(dv, &numPotentialLocs); 
			numRiskyLocs = 0;
			for (int player = 0; player < 4; player++) {
				PlaceId currentLoc = DvGetPlayerLocation(dv, player);
				for (int i = 0; i < numPotentialLocs; i++) {
					if (potentialLocs[i] == currentLoc) {
						strcpy(play, placeIdToAbbrev(currentLoc));
						free(validLocs);
						free(potentialLocs);
						registerBestPlay(play, "give me marks");
						return;
					}
				}
			}
			free(potentialLocs);
		}

		// Default: Dracula picks a random risky location.
		validLocs = DvWhereCanIGo(dv, &numValidLocs);
		int index = rand() % (numValidLocs + 1);
		strcpy(play, placeIdToAbbrev(validLocs[index]));
		free(validLocs);
		registerBestPlay(play, "Mwahahahaha");
	}

	// Default: choose a random location
	// If Dracula can go to a location that is not "risky":
	// Go to random location in ValidLocs (not necessarily a good move!)
	int index = rand() % (numValidLocs + 1);
	strcpy(play, placeIdToAbbrev(validLocs[index]));
	free(validLocs);
	registerBestPlay(play, "Mwahahahaha");
	return;
}

void removeRiskyLocs(PlaceId *ValidLocs, PlaceId *riskyLocs, int *numValidLocs, int *numRiskyLocs) {
	// For each valid location of Dracula:
	for (int i = 0; i < *numValidLocs; i++) {
		// Compare with each risky location.
		for (int j = 0; j < *numRiskyLocs; j++) {
			if (ValidLocs[i] == riskyLocs[j]) {
				// Remove location from ValidLocs if it is a riskyLoc.
				for (int c = i; c < *numValidLocs - 1; c++) {
					ValidLocs[c] = ValidLocs[c + 1];
				}
				*numValidLocs = *numValidLocs - 1;
			}
		}
	}
	return;
}


