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
#include "GameView.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

PlaceId MoveToLocation(PlaceId *pastLocs, PlaceId location, int *numPastLocs);
bool isPortCity(PlaceId i);
void decideDraculaMove(DraculaView dv)
{
	int health = DvGetHealth(dv, PLAYER_DRACULA);  		// Dracula's Blood Points.
	int numValidMoves = 0;						  		// Number of Valid Locations for Dracula.	
	int numRiskyLocs = 0;					      		// Number of Risky Locations for Dracula.
	int numPastLocs = 0;								// Number of Past Locations in Dracula's move history.
	PlaceId hunterLocs[4];                              // Array of current hunter locations.
	for (int player = 0; player < 4; player++) {
		hunterLocs[player] = DvGetPlayerLocation(dv, player);
	}
	Round round = DvGetRound(dv);				  		// The current round in the game.
	time_t t;
	srand((unsigned) time(&t));							// seed for random movements.  
	int riskLevel[NUM_REAL_PLACES] = {0};				// Array containing risk levels for each place. 
	char *play = malloc(sizeof(char) *2); 		  		// The play to be made.

	// Dracula picks STRASBOURG for the first round
	// as this has the most road connections (8) and thus
	// it is hard for the hunters to corner him. 
	if (round == 0) {
		registerBestPlay("PA", "Mwahahahaha");
		return;
	}

	// If Dracula has no valid moves, use TELEPORT.
	PlaceId *validMoves = DvGetValidMoves(dv, &numValidMoves);
	if (numValidMoves == 0 || validMoves == NULL) {
		registerBestPlay("TP", "I love COMP2521");
		return;
	}

	// Go to Castle Dracula if possible - Dracula wants to gain 10 BP.
	// Even if a hunter is there, it will be an even exchange. 
	PlaceId *pastLocs = DvGetLocationHistory(dv, &numPastLocs);  
	for (int i = 0; i < numValidMoves; i++) {
		if (MoveToLocation(pastLocs, validMoves[i], &numPastLocs) == CASTLE_DRACULA) {
			strcpy(play, placeIdToAbbrev(validMoves[i]));
			registerBestPlay(play, "COMP2521 > COMP1511");
			return;
		} 
	}

	// Assign risk levels to each place.
	// int CastleProximity = 0;
	for (int player = 0; player < 4; player++) {
		// Locations reachable by hunters through ROAD have a risk of 2.
		riskLevel[hunterLocs[player]] += 2;
		PlaceId *riskyLocsRoad = DvWhereCanTheyGoByType(dv, player, true, false, true, &numRiskyLocs);
		for (int i = 0; i < numRiskyLocs; i++) {
			riskLevel[riskyLocsRoad[i]] += 2;
		}

		PlaceId *riskyLocsRail = DvWhereCanTheyGoByType(dv, player, false, true, false, &numRiskyLocs);
		for (int i = 0; i < numRiskyLocs; i++) {
			if (health <= 15) {
				riskLevel[riskyLocsRail[i]] += 1;
			}
			riskLevel[riskyLocsRail[i]] += 1;
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

	// Bad places to go to 
	// If drac goes here he always seems to
	// go to NORTH SEA and loses health.
	riskLevel[LISBON] = 5;

	// FLORENCE only connects to PORT cities,
	// which can be bad. 
	riskLevel[FLORENCE] -= 1;

	Map m = MapNew();
	
	// Assign a risk level to each location 
	for (int i = 0; i < NUM_REAL_PLACES; i++) {
		// Sea locations have risk of 1.
		if (placeIsSea(i)) {
			riskLevel[i] += 2;
			if (health <= 20) {
				riskLevel[i] += 10;
			}
			// Don't suicide at sea!
			if (health <= 10) {
				riskLevel[i] += 20;
			}
		}
		else if (isPortCity(i)) {
			riskLevel[i] += 1;
		}

		ConnList list = MapGetConnections(m, i);
		ConnList curr = list->next;
		int count = 0;
		while (curr != NULL) {
			count++;
			curr = curr->next;
		}
		if (count <= 3) {
			riskLevel[i] += 2;
		}
		// TODO: Locations with a small number of road connections should increase risk by 1.
	}
	MapFree(m);

	// Note: Once Dracula's health is between 20 and 30, try to return to CD.
	// FIND THE MOVES WITH THE MINIMUM RISK LEVEL
	
	int min = riskLevel[MoveToLocation(pastLocs, validMoves[numValidMoves - 1], &numPastLocs)];
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

	// If there are no low risk moves pick a random valid move.
	if (lowRiskNum == 0) {
		int i = rand() % numValidMoves;
		strcpy(play, placeIdToAbbrev(validMoves[i]));
		registerBestPlay(play, "mwahahahah");
		return;
	}
	
	// If drac is currently at the same location as a player,
	// do not go to any of the hunter current locations.
	PlaceId dracLoc = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	for (int player = 0; player < 4; player++) {
		if (hunterLocs[player] == dracLoc) {
			riskLevel[hunterLocs[player]] += 10;
			for (int i = 0; i < 4; i++) {
				riskLevel[hunterLocs[i]] += 10;
			}
		}
	}
	// If the lowest risk move is still "risky":
	if (riskLevel[MoveToLocation(pastLocs, lowRiskMoves[0], &numPastLocs)] > 0) {
		// If dracula is healthy go to the player's current location.
		validMoves = DvGetValidMoves(dv, &numValidMoves); 
		if (health >= 60) {
			for (int player = 0; player < 4; player++) {
				for (int i = 0; i < numValidMoves; i++) {
					if (MoveToLocation(pastLocs, validMoves[i], &numPastLocs) == hunterLocs[player]) {
						strcpy(play, placeIdToAbbrev(validMoves[i]));
						registerBestPlay(play, "mwahahahhaa");
						return;
					}
				}
			}
		}
		if (health >= 40 || (health >= 8 && health <= 16)) {
			// Go to the sea if possible.
			for (int j = 0; j < numValidMoves; j++) {
				if (placeIsSea(validMoves[j])) {
					strcpy(play, placeIdToAbbrev(validMoves[j]));
					registerBestPlay(play, "mwahahahah");
					return;
				}
			}

			// Pick a random risky location otherwise.
			int i = rand() % (lowRiskNum);
			strcpy(play, placeIdToAbbrev(lowRiskMoves[i]));
			registerBestPlay(play, "mwahahahah");
			return;
		}
	}

	// Default: choose a random location
	// If Dracula can go to a location that is not "risky":
	// Go to random location in ValidLocs (not necessarily a good move!)
	int i;
	if (lowRiskNum != 0) {
		i = rand() % (lowRiskNum);
	} else {
		i = 0;
	}

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

bool isPortCity(PlaceId i) {
	switch (i) {
		case BARI:
			i = -2;
			break;
		case ALICANTE:
			i = -2;
			break;
		case AMSTERDAM:
			i = -2;
			break;
		case ATHENS:
			i = -2;
			break;
		case CADIZ:
			i = -2;
			break;
		case GALWAY:
			i = -2;
			break;
		case LISBON:
			i = -2;
			break;
		case BARCELONA:
			i = -2;
			break;
		case BORDEAUX:
			i = -2;
			break;
		case NANTES:
			i = -2;
			break;
		case SANTANDER:
			i = -2;
			break;
		case CONSTANTA:
			i = -2;
			break;
		case VARNA:
			i = -2;
			break;
		case CAGLIARI:
			i = -2;
			break;
		case DUBLIN:
			i = -2;
			break;
		case EDINBURGH:
			i = -2;
			break;
		case LE_HAVRE:
			i = -2;
			break;
		case LONDON:
			i = -2;
			break;
		case PLYMOUTH:
			i = -2;
			break;
		case GENOA:
			i = -2;
			break;
		case HAMBURG:
			i = -2;
			break;
		case SALONICA:
			i = -2;
			break;
		case VALONA:
			i = -2;
			break;
		case LIVERPOOL:
			i = -2;
			break;
		case SWANSEA:
			i = -2;
			break;
		case MARSEILLES:
			i = -2;
			break;
		case NAPLES:
			i = -2;
			break;
		case ROME:
			i = -2;
			break;
		default:
			i = -1;
	}

	if (i == -1) {
		return false;
	} else if (i == -2) {
		return true;
	}
	return false;
}