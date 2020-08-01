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
#include <stdio.h>
#include <string.h>

void removeRiskyLocs(PlaceId *validLocs, PlaceId *riskyLocs, int *numValidLocs, int *numRiskyLocs);
void decideDraculaMove(DraculaView dv)
{
	Round round = DvGetRound(dv);
	// First Round
	// Dracula should start in a location adjacent to many other
	// locations (by road), to avoid being cornered.
	// Strasbourg has 8 adjacent road cities, so choose Strasbourg.
	if (round == 0) {
		registerBestPlay("ST", "Mwahahahaha");
		return;
	}

	// Get possible locations for Dracula. Return TELEPORT if no valid moves.
	int numValidLocs = 0;
	PlaceId *validLocs = DvWhereCanIGo(dv, &numValidLocs);
	if (numValidLocs == 0) {
		free(validLocs);
		registerBestPlay("TP", "Mwahahahaha");
		return;
	}

	// Get reachable locations ("risky locations" for each hunter and remove these, if they exist, from Dracula's
	// valid locations array (he wants to avoid these locations as much as possible).
	int numRiskyLocs = 0;
	for (int player = PLAYER_LORD_GODALMING; player < PLAYER_MINA_HARKER; player++) {
		// riskyLocs should never be null as hunters always have a valid move.
		PlaceId *riskyLocs = DvWhereCanTheyGo(dv, player, &numRiskyLocs);
		removeRiskyLocs(validLocs, riskyLocs, &numValidLocs, &numRiskyLocs);
		free(riskyLocs);
	}
	
	// If all of Dracula's valid locations are also "risky" locations
	if (numValidLocs == 0) {
		
		// If possible go to the SEA to avoid encountering a hunter.
		// (Its better to lose 2 BP at SEA than lose 10 BP to a hunter!)
		validLocs = DvWhereCanIGoByType(dv, false, true, &numValidLocs);
		if (validLocs != NULL) {
			const char *abbrev = placeIdToAbbrev(validLocs[0]);
			char *play;
			strcpy(play, abbrev);
			free(validLocs);
			registerBestPlay(play, "Mwahahahaha");
			return;
		}
		
		// Otherwise go to the first move in the original validLocs (not necessarily a good move!)
		validLocs = DvWhereCanIGo(dv, &numValidLocs);
		const char *abbrev = placeIdToAbbrev(validLocs[0]);
		char *play;
		strcpy(play, abbrev);
		free(validLocs);
		registerBestPlay(play, "Mwahahahaha");
		return;
	}

	// If Dracula can go to a location that is not "risky":
	// Go to the first move in validLocs (not necessarily a good move!)
	const char *abbrev = placeIdToAbbrev(validLocs[0]);
	char *play;
	strcpy(play, abbrev);
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