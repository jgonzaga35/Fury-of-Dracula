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
// add your own #includes here

// TODO: ADD YOUR OWN STRUCTS HERE

struct draculaView {
	// not yet finished adding variables to this struct
	GameView gv;
	Round numTurn;
	// not sure if we need these variables - maybe
	// we can just get them using Gv functions?
	//int score;
	//int health[NUM_PLAYERS];
	//PlaceId trails[NUM_PLAYERS][TRAIL_SIZE];	// Never null
	//PlaceId vampireLocation;					// Never null
	//PlaceId trapLocations[TRAIL_SIZE];	
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
	// Steps:
	// Get reachable locations from current locations.
	// Look through trail, and determine if Drac can make HIDE or DOUBLE BACK move.
	// Add these to array.
	// Remove locations that Drac has moved to before.

	bool canFree = true;
	int numReturnedLocs = 0;
	PlaceId curr = GvGetPlayerLocation(dv->gv, PLAYER_DRACULA);
	PlaceId *validLocs = GvGetReachableByType(dv->gv, PLAYER_DRACULA, dv->numTurn, curr, true, false, true, &numReturnedLocs);
	PlaceId *trail = GvGetLastMoves(dv->gv, PLAYER_DRACULA, 6, numReturnedMoves, &canFree);
	//PlaceId *visited = GvGetLastLocations(dv->gv, PLAYER_DRACULA, 5, numReturnedMoves, &canFree);
	
	bool canHide = true;
	bool canDoubleBack[5];
	canDoubleBack[0] = true;

	// Look through trail and if there are any HIDE or DOUBLE_BACKS set to false;
	// Also remove any locs in trail visited by location move from validLocs.
	int removedLocs = 0;
	for (int i = 0; i < *numReturnedMoves; i++) { 
		if (isDoubleBack(trail[i])) {
			for (int j = 1; j < 5; j++) {
				canDoubleBack[j] = false;
			} 
		} 
		else if (trail[i] == HIDE) {
			canHide = false;
		} 
		else if (placeIsReal(trail[i])) {
			// remove from validLocs if it is there.
			for (int j = 0; j < numReturnedLocs; j++) {
				if (trail[i] == validLocs[j]) {
					for (int c = j; c < numReturnedLocs - 1; c++) {
						validLocs[c] = validLocs[c + 1]; 
					}
					removedLocs++;
				}
			}
		}
	}
	assert(canDoubleBack[1] == false);
	numReturnedLocs -= removedLocs;
	PlaceId *validMoves;

	int length = 0;
	// Add validLocs and valid HIDE or DOUBLE BACK moves to validMoves array
	for (int i = numReturnedLocs; i < numReturnedLocs + 5; i++) {
		if (canHide) {
			validLocs[i] = HIDE;
			canHide = false;
			// length++; 
		} 
		else if (canDoubleBack[0]) {
			validLocs[i] = DOUBLE_BACK_1;
			canDoubleBack[0] = false;
			length++;
		} else if (canDoubleBack[1] == true) {
			printf("happy\n");
			validLocs[i] = DOUBLE_BACK_2;
			canDoubleBack[1] = false;
			length++;
		} else if (canDoubleBack[2]) {
			validLocs[i] = DOUBLE_BACK_3;
			canDoubleBack[2] = false;
			length++;
		} else if (canDoubleBack[3]) {
			validLocs[i] = DOUBLE_BACK_4;
			canDoubleBack[3] = false;
			length++;
		} else if (canDoubleBack[4]) {
			validLocs[i] == DOUBLE_BACK_5;
			canDoubleBack[4] = false;
			length++;
		} 
	}
	numReturnedLocs += length;
	*numReturnedMoves = numReturnedLocs;

	free(trail);

	return validLocs;
}

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
	PlaceId *availableLocs = DvGetValidMoves(dv, numReturnedLocs);
	for (int i = 0; i < *numReturnedLocs; i++) {
		printf("availableLocs[i] is %s\n", placeIdToAbbrev(availableLocs[i]));
	}
	bool canFree = true;
	printf("*numReturnedLocs is %d\n", *numReturnedLocs);

	int length = 0;
	for (int i = 0; i < *numReturnedLocs; i++) {
		if (isDoubleBack(availableLocs[i]) || availableLocs[i] == HIDE) {
			for (int c = i; c < *numReturnedLocs - 1; c++) {
				availableLocs[c] = availableLocs[c + 1]; 
			}
			length++;
		}
	}
	*numReturnedLocs -= length;
	return availableLocs;
}

PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *DvWhereCanTheyGo(DraculaView dv, Player player,
                          int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *DvWhereCanTheyGoByType(DraculaView dv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
