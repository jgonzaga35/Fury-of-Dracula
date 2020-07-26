////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// DraculaView.c: the DraculaView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "DraculaView.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h"

struct draculaView {
	GameView gv;
	Round numTurn;
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

DraculaView DvNew(char *pastPlays, Message messages[])
{

	DraculaView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate DraculaView\n");
		exit(EXIT_FAILURE);
	}
	new->gv = GvNew(pastPlays, messages);
	new->numTurn = GvGetRound(new->gv);

	return new;
}

void DvFree(DraculaView dv)
{
	free(dv->gv);
	free(dv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round DvGetRound(DraculaView dv)
{
	return GvGetRound(dv->gv);
}

int DvGetScore(DraculaView dv)
{
	return GvGetScore(dv->gv);
}

int DvGetHealth(DraculaView dv, Player player)
{
	return GvGetHealth(dv->gv, player);
}

PlaceId DvGetPlayerLocation(DraculaView dv, Player player)
{	
	return GvGetPlayerLocation(dv->gv, player);
}

PlaceId DvGetVampireLocation(DraculaView dv)
{
	return GvGetVampireLocation(dv->gv);
}

PlaceId *DvGetTrapLocations(DraculaView dv, int *numTraps)
{
	return GvGetTrapLocations(dv->gv, numTraps);
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{
	// If the player has not made a move yet return NULL.
	PlaceId from = GvGetPlayerLocation(dv->gv, PLAYER_DRACULA);
	if (from == NOWHERE) {
		*numReturnedMoves = 0;
		return NULL;
	}

	// Initialise array with list of reachable places.
	int numReturnedLocs = 0;
	PlaceId *validLocs = GvGetReachableByType(dv->gv, PLAYER_DRACULA, dv->numTurn, from, true, false, true, &numReturnedLocs);

	// Obtain Dracula's last 6 moves.
	bool canFree = true;
	PlaceId *trail = GvGetLastMoves(dv->gv, PLAYER_DRACULA, 6, numReturnedMoves, &canFree);

	// Determine if HIDE or DOUBLE_BACK are valid moves based on trail length.
	bool canHide = false;
	bool canDoubleBack[5];
	for (int i = 0; i < 5; i++) {
		if (*numReturnedMoves >= i + 1) {
			canHide = true;
			canDoubleBack[i] = true;
		} else {
			canDoubleBack[i] = false;
		}
	}

	// Determine if HIDE or DOUBLE_BACK are valid moves based on trail history.
	// Remove any locations in the array that have been visited already using LOCATION move.
	int removedLocs = 0;
	for (int i = 0; i < *numReturnedMoves; i++) { 
		if (isDoubleBack(trail[i])) {
			for (int j = 0; j < 5; j++) {
				canDoubleBack[j] = false;
			} 
		} 
		else if (trail[i] == HIDE) {
			canHide = false;
		} 
		else if (placeIsReal(trail[i])) {
			for (int j = 0; j < numReturnedLocs; j++) {
				if (validLocs[j] == trail[i]) {
					for (int c = j; c < numReturnedLocs - 1; c++) {
						validLocs[c] = validLocs[c + 1]; 
					}
					removedLocs++;
					break;
				}
			}
		}
	}
	// Adjust for removed locations.
	numReturnedLocs -= removedLocs;
	
	// Add valid HIDE or DOUBLE BACK moves to validMoves array.
	// Start filling in validLocs array from last index.
	int length = 0;
	int start = numReturnedLocs;
	int end = numReturnedLocs + *numReturnedMoves + 1;
	for (int i = start; i < end; i++) 
	{
		if (canHide) {
			validLocs[i] = HIDE;
			canHide = false;
			length++; 
		} 
		else if (canDoubleBack[0]) {
			validLocs[i] = DOUBLE_BACK_1;
			canDoubleBack[0] = false;
			length++;
		} 
		else if (canDoubleBack[1]) {
			validLocs[i] = DOUBLE_BACK_2;
			canDoubleBack[1] = false;
			length++;
		} 
		else if (canDoubleBack[2]) {
			validLocs[i] = DOUBLE_BACK_3;
			canDoubleBack[2] = false;
			length++;
		} 
		else if (canDoubleBack[3]) {
			validLocs[i] = DOUBLE_BACK_4;
			canDoubleBack[3] = false;
			length++;
		} 
		else if (canDoubleBack[4]) {
			validLocs[i] == DOUBLE_BACK_5;
			canDoubleBack[4] = false;
			length++;
		} 
	}

	*numReturnedMoves = numReturnedLocs + length;
	free(trail);

	// If all valid moves have been removed, return NULL.
	if (*numReturnedMoves == 0) {
		free(validLocs);
		return NULL;
	}

	return validLocs;
}

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
	PlaceId *validLocs = DvGetValidMoves(dv, numReturnedLocs);

	int numNum = 0;
	bool canFree = true;
	PlaceId *pastMoves = GvGetMoveHistory(dv->gv, PLAYER_DRACULA, &numNum, &canFree);
	PlaceId *pastLocs = GvGetLocationHistory(dv->gv, PLAYER_DRACULA, &numNum, &canFree);

	if (validLocs == NULL) {
		*numReturnedLocs = 0;
		return NULL;
	}

	canFree = true;
	for (int i = 0; i < *numReturnedLocs; i++) {
		if (validLocs[i] == HIDE) {
			validLocs[i] = pastLocs[0];
		}
		else if (isDoubleBack(validLocs[i])) {
			switch (validLocs[i]) {
				case DOUBLE_BACK_1: validLocs[i] = pastLocs[0]; break;
				case DOUBLE_BACK_2: validLocs[i] = pastLocs[1]; break;
				case DOUBLE_BACK_3: validLocs[i] = pastLocs[2]; break;
				case DOUBLE_BACK_4: validLocs[i] = pastLocs[3]; break;
				case DOUBLE_BACK_5: validLocs[i] = pastLocs[4]; break;
			}
		}
	}
	
	// Remove duplicates in the array.
	int length = 0;
	for (int i = 0; i < *numReturnedLocs; i++) {
		for (int j = i; j < *numReturnedLocs; j++) {
			if (i != j && validLocs[i] == validLocs[j]) {
				for (int c = j; c < *numReturnedLocs - 1; c++) {
					validLocs[c] = validLocs[c + 1];
				}
				*numReturnedLocs = *numReturnedLocs - 1;
				break;
			}
		}
	}

	free(pastMoves);
	free(pastLocs);
	return validLocs;
}

PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
	// Obtain list of valid locations (takes into account past moves).
	PlaceId *validLocs = DvWhereCanIGo(dv, numReturnedLocs);

	if (validLocs == NULL) {
		*numReturnedLocs = 0;
		return NULL;
	}

	// Find reachable locations by type.
	int validNum = *numReturnedLocs;
	PlaceId currentLoc = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	PlaceId *reachableLocs = GvGetReachableByType(dv->gv, PLAYER_DRACULA, dv->numTurn, currentLoc, road, false, boat, numReturnedLocs);
	int reachableNum = *numReturnedLocs;
	*numReturnedLocs = validNum;

	// Delete non-reachable locations from valid locations array.
	bool canGo = false;
	int numRemoved = 0;
	for (int i = 0; i < validNum; i++) {
		canGo = false;
		for (int j = 0; j < reachableNum; j++) {
			if (validLocs[i] == reachableLocs[j]) {
				canGo = true;
			}
		}
		if (!canGo) {
			for (int c = i; c < validNum - 1; c++) {
				validLocs[c] = validLocs[c + 1];
			}
			numRemoved++;
		}
	}
	*numReturnedLocs -= numRemoved;
	
	free(reachableLocs);

	return validLocs;	
}

PlaceId *DvWhereCanTheyGo(DraculaView dv, Player player,
                          int *numReturnedLocs)
{
	// DvWhereCanTheyGo has same function as DvWhereCanIGo if player is Dracula.
	if (player == PLAYER_DRACULA) {
		return DvWhereCanIGo(dv, numReturnedLocs);
	}

	// Else return reachable locations for the hunter.
	PlaceId current = GvGetPlayerLocation(dv->gv, player);
	PlaceId *availableLocs = GvGetReachable(dv->gv, player, dv->numTurn, current, numReturnedLocs);
	if (availableLocs == NULL) {
		*numReturnedLocs = 0;
		return NULL;
	}
	return availableLocs;
}

PlaceId *DvWhereCanTheyGoByType(DraculaView dv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	// DvWhereCanTheyGoByType has same function as DvWhereCanIGoByType if player is Dracula.
	if (player == PLAYER_DRACULA) {
		return DvWhereCanIGoByType(dv, road, boat, numReturnedLocs);
	}

	// Else return reachable locations for the hunter.
	PlaceId current = GvGetPlayerLocation(dv->gv, player);
	PlaceId *availableLocs = GvGetReachableByType(dv->gv, player, dv->numTurn, current, road, rail, boat, numReturnedLocs);
	if (availableLocs == NULL) {
		*numReturnedLocs = 0;
		return NULL;
	}
	return availableLocs;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions


