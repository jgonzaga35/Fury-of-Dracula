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

void decideDraculaMove(DraculaView dv)
{
	Round round = DvGetRound(dv);				  // The current round in the game.
	char *play;									  // The play to be made.
	int health= DvGetHealth(dv, PLAYER_DRACULA);  // Dracula's Blood Points.
	int numValidLocs = 0;						  // Number of Valid Locations for Dracula.	
	int numRiskyLocs = 0;					      // Number of Risky Locations for Dracula.
	PlaceId draculaLoc = DvGetPlayerLocation(dv, PLAYER_DRACULA);	// Current location of Dracula
	PlaceId *trail = DvGetTrail(dv);				// Dracula's trail
	PlaceId hunterLocs[4];                        // Array of current hunter locations.
	for (int player = 0; player < 4; player++) {
		hunterLocs[player] = DvGetPlayerLocation(dv, player);
	}

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
		if (validLocs[i] == CASTLE_DRACULA && health >= 25) {
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
				registerBestPlay(convertMove(trail, validLocs[index], draculaLoc), "Mwahahahaha");
				//strcpy(play, placeIdToAbbrev(validLocs[index]));
				free(validLocs);
				//registerBestPlay(play, "jas is best lecturer");
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
				for (int i = 0; i < numPotentialLocs; i++) {
					if (potentialLocs[i] == hunterLocs[player]) {
						//strcpy(play, placeIdToAbbrev(hunterLocs[player]));
						registerBestPlay(convertMove(trail, hunterLocs[player], draculaLoc), "Mwahahahaha");
						free(validLocs);
						free(potentialLocs);
						//registerBestPlay(play, "give me marks");
						return;
					}
				}
			}
			free(potentialLocs);
		}

		// Default: Dracula picks a random risky location.
		validLocs = DvWhereCanIGo(dv, &numValidLocs);
		int index = rand() % (numValidLocs + 1);
		registerBestPlay(convertMove(trail, validLocs[index], draculaLoc), "Mwahahahaha");
		//strcpy(play, placeIdToAbbrev(validLocs[index]));
		free(validLocs);
		//registerBestPlay(play, "Mwahahahaha");
		return;
	}

	// Default: choose a random location
	// If Dracula can go to a location that is not "risky":
	// Go to random location in ValidLocs (not necessarily a good move!)
	int index = rand() % (numValidLocs + 1);
	registerBestPlay(convertMove(trail, validLocs[index], draculaLoc), "Mwahahahaha");
	// strcpy(play, placeIdToAbbrev(validLocs[index]));
	free(validLocs);
	// registerBestPlay(play, "Mwahahahaha");
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
	int index = indexInTrail(trail, location);
	if (index != NO_EXISTENCE) return (char *) placeIdToAbbrev(102 + index);
	else return (char *) placeIdToAbbrev(location);
}

// Return the index of location in Dracula's trail, -1 if it doesn't exist
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
