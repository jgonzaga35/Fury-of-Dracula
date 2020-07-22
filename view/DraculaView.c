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
	Round numTurn;
	int score;
	int health[NUM_PLAYERS];
	PlaceId trails[NUM_PLAYERS][TRAIL_SIZE];	// Never null
	PlaceId vampireLocation;					// Never null
	PlaceId trapLocations[TRAIL_SIZE];	
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

DraculaView DvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	DraculaView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate DraculaView\n");
		exit(EXIT_FAILURE);
	}

	return new;
}

void DvFree(DraculaView dv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	free(dv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round DvGetRound(DraculaView dv)
{
	//return GvGetRound(dv);
	return 0;
}

int DvGetScore(DraculaView dv)
{
	//return GvGetScore(dv);
	return 0;
}

int DvGetHealth(DraculaView dv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	//return GvGetHealth(dv, player);
	return 0;
}

PlaceId DvGetPlayerLocation(DraculaView dv, Player player)
{
	// Dracula knows current locations of all players, so
	// same as GvGetPlayerLocation.
	//return GvGetPlayerLocation(dv, player);
	return 1;
}

PlaceId DvGetVampireLocation(DraculaView dv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	//return GvGetVampireLocation(dv);
	return 1;
}

PlaceId *DvGetTrapLocations(DraculaView dv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	//return GvGetTrapLocations(dv, *numTraps);
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	return NULL;
}

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	//PlaceId currLoc = GvGetPlayerLocation(dv, PLAYER_DRACULA);
	//PlaceId *availableLocs = GvGetReachable(dv, PLAYER_DRACULA, dv->numTurn, currLoc, *numReturnedLocs);
	//return availableLocs;
	return NULL;
}

PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	// *numReturnedLocs = 0;
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
