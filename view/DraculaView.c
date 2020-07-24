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
/**
 * Gets all the moves that Dracula can validly make this turn.
 *
 * This  function  should  return  the  moves in a dynamically allocated
 * array, and set *numReturnedMoves to the number of moves returned. The
 * array can be in any order but must contain unique entries.
 *
 * If  Dracula  has  no valid moves (other than TELEPORT), this function
 * should set *numReturnedMoves to 0 and return  an  empty  array  (NULL
 * will suffice).
 *
 * If  Dracula  hasn't  made  a move yet, set *numReturnedMoves to 0 and
 * return NULL.
 */

	*numReturnedMoves = 0;
	bool canFree = true;
	PlaceId *trail = GvGetLastMoves(dv->gv, PLAYER_DRACULA, 6, numReturnedMoves, &canFree);

	if (*numReturnedMoves == 0) {
		return NULL;
	}

	// always true for *numReturnedMoves >= 1
	int canHide = true;
	int canDoubleBack_1 = true;

	int canDoubleBack_2 = false;
	int canDoubleBack_3 = false;
	int canDoubleBack_4 = false;
	int canDoubleBack_5 = false;

	if (*numReturnedMoves == 2) {
		canDoubleBack_2 = true;
	} else if (*numReturnedMoves == 3) {
		canDoubleBack_2 = true;
		canDoubleBack_3 = true;
	} else if (*numReturnedMoves == 4) {
		canDoubleBack_2 = true;
		canDoubleBack_3 = true;
		canDoubleBack_4 = true;
	} else if (*numReturnedMoves >= 5) {
		canDoubleBack_2 = true;
		canDoubleBack_3 = true;
		canDoubleBack_4 = true;
		canDoubleBack_5 = true;
	}
	
	// Look for HIDE or DOUBLE_BACK in the trail.
	for (int i = 0; i < *numReturnedMoves; i++) {
		if (trail[i] == HIDE) {
			canHide = false;
		}
		else if (trail[i] <= DOUBLE_BACK_5 && trail[i] >= DOUBLE_BACK_1) {
			canDoubleBack_1 = false;
			canDoubleBack_2 = false;
			canDoubleBack_3 = false;
			canDoubleBack_4 = false;
			canDoubleBack_5 = false;
		}
	}

	*numReturnedMoves = 0;
	PlaceId from = GvGetPlayerLocation(dv->gv, PLAYER_DRACULA);

	//This only accounts for location moves and not HIDE or DOUBLEBACK
	PlaceId *validMoves = GvGetReachableByType(dv->gv, PLAYER_DRACULA, dv->numTurn, 
									from, true, false, true, numReturnedMoves);

	// validMoves[0] contains the current city, we need to remove this first.
	for (int i = 0; i < *numReturnedMoves; i++) {
		validMoves[i] = validMoves[i + 1];
	}
	*numReturnedMoves -= 1;

	// Add any HIDE or DOUBLE BACK moves.
	int extraMoves = 0;
	for (int i = *numReturnedMoves; i < *numReturnedMoves + 6; i++) {
		if (canHide == true) {
			validMoves[i] = HIDE;
			canHide = false;
			extraMoves++;
		} else if (canDoubleBack_1 == true) {
			validMoves[i] = DOUBLE_BACK_1;
			canDoubleBack_1 = false;
			extraMoves++;
		} else if (canDoubleBack_2 == true) {
			validMoves[i] = DOUBLE_BACK_2;
			canDoubleBack_2 = false;
			extraMoves++;
		} else if (canDoubleBack_3 == true) {
			validMoves[i] = DOUBLE_BACK_3;
			canDoubleBack_3 = false;
			extraMoves++;
		} else if (canDoubleBack_4 == true) {
			validMoves[i] = DOUBLE_BACK_4;
			canDoubleBack_4 = false;
			extraMoves++;
		} else if (canDoubleBack_5 == true) {
			validMoves[i] == DOUBLE_BACK_5;
			canDoubleBack_5 = false;
			extraMoves++;
		}
	}
	*numReturnedMoves += extraMoves;

	return validMoves;
}

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId currLoc = GvGetPlayerLocation(dv->gv, PLAYER_DRACULA);
	PlaceId *availableLocs = GvGetReachable(dv->gv, PLAYER_DRACULA, dv->numTurn, currLoc, numReturnedLocs);
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
