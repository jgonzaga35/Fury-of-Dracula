////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// HunterView.c: the HunterView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10   v3.0    Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Game.h"
#include "GameView.h"
#include "HunterView.h"
#include "Map.h"
#include "Places.h"
// add your own #includes here

struct hunterView {
	GameView  gv;
	Message *message;
};

// Function prototypes
static int isRealLocation(PlaceId location);
////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

HunterView HvNew(char *pastPlays, Message messages[])
{
	HunterView new = malloc(sizeof(struct hunterView));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate HunterView!\n");
		exit(EXIT_FAILURE);
	}
	
	new->gv = GvNew(pastPlays, messages);

	int numTurns = numTurnsPassed(new->gv);
	new->message = malloc(numTurns * sizeof(Message));
	   
	for (int i = 0; i < numTurns; i++) {
		strncpy(new->message[i], messages[i], MESSAGE_SIZE);
	}
	
	return new;
}

void HvFree(HunterView hv)
{
	GvFree(hv->gv);
	free(hv->message);
	free(hv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round HvGetRound(HunterView hv)
{
	return GvGetRound(hv->gv);
}

Player HvGetPlayer(HunterView hv)
{
	return GvGetPlayer(hv->gv);
}

int HvGetScore(HunterView hv)
{
	return GvGetScore(hv->gv);
}

int HvGetHealth(HunterView hv, Player player)
{
	return GvGetHealth(hv->gv, player);
}

PlaceId HvGetPlayerLocation(HunterView hv, Player player)
{	
	return GvGetPlayerLocation(hv->gv, player);
}

PlaceId HvGetVampireLocation(HunterView hv)
{
	return GvGetVampireLocation(hv->gv);
}

////////////////////////////////////////////////////////////////////////
// Utility Functions

PlaceId HvGetLastKnownDraculaLocation(HunterView hv, Round *round)
{
	int numReturnedLocs = 0;
	bool canFree = false;
	PlaceId *trails = GvGetLocationHistory(hv->gv, PLAYER_DRACULA, &numReturnedLocs, &canFree);
	
	int i;
	PlaceId location;
	for (i = 0; i < numReturnedLocs; i++)
	{
		location = trails[i];
		if (isRealLocation(location)) break;
	}
	
	if (i == 0) return;	// No trail
	if (!isRealLocation(location)) return NOWHERE;	// No real location exist

	*round = HvGetRound(hv) - i;
	if (location == TELEPORT) return CASTLE_DRACULA;
	
	return location;
}

static int isRealLocation(PlaceId location)
{
	return (location != CITY_UNKNOWN && location != SEA_UNKNOWN 
			&& !isDoubleBack(location) && location != HIDE && location != UNKNOWN);
}

PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*pathLength = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

// PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
// {
// 	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
// 	Player currHunter = HvGetPlayer(hv);
// 	PlaceId currLoc = GvGetPlayerLocation(hv, currHunter);
// 	PlaceId *availableLocs = GvGetReachable(hv, currHunter, hv->gv->numTurn, currLoc, *numReturnedLocs);
// 	return availableLocs;
// }

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION

	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *HvWhereCanTheyGoByType(HunterView hv, Player player,
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
