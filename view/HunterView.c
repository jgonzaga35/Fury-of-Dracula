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
#include "Queue.h"
// add your own #includes here
#include <string.h>

#define TRUE	1
#define FALSE	0

struct hunterView {
	GameView  gv;
	Message *message;
};

// Function prototypes
static int isRealLocation(PlaceId location);
static int draculaNotRevealed(HunterView hv);
////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

HunterView HvNew(char *pastPlays, Message messages[])
{
	HunterView new = malloc(sizeof(struct hunterView));
	if (new == NULL) 
	{
		fprintf(stderr, "Couldn't allocate HunterView!\n");
		exit(EXIT_FAILURE);
	}
	
	new->gv = GvNew(pastPlays, messages);

	int numTurns = numTurnsPassed(new->gv);
	new->message = malloc(numTurns * sizeof(Message));
	   
	for (int i = 0; i < numTurns; i++)
		strncpy(new->message[i], messages[i], MESSAGE_SIZE);
	
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
	*round = 0;

    PlaceId location = NOWHERE;

	int i;
	
	for (i = numReturnedLocs - 1; i >= 0 ; i--) 
	{	
		if (isRealLocation(trails[i])) 
		{	
			location = trails[i];
			if (location == HIDE) location = traceHideByIndex(trails, i);
			else if (isDoubleBack(location)) location = traceDoubleBackByIndex(trails, i);

			if (isRealLocation(location))
			{
				*round = i;
				location = trails[i];
				break;
			}
		}
	}
	
	if (!isRealLocation(location) || round == 0) return NOWHERE; // No real location exist

	if (location == TELEPORT) return CASTLE_DRACULA;

    return location;
}

PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{	
	PlaceId pathTo[MAX_REAL_PLACE + 1];
	int roundArray[MAX_REAL_PLACE + 1];		// Array that stores the round that 
											// we should arrive at a location

	PlaceId src = HvGetPlayerLocation(hv, hunter);
	Round currRound = HvGetRound(hv);

	PlaceId currLocation;
	
	//Initialize
	for (PlaceId i = 0; i < MAX_REAL_PLACE + 1; i++) pathTo[i] = -1;
	for (int i = 0; i < MAX_REAL_PLACE + 1; i++) roundArray[i] = 0;
	roundArray[src] = currRound;			// we are at src at currRound
	pathTo[src] = src;

	Queue locationQ = newQueue();
	QueueJoin(locationQ, src);
	
	int found = FALSE;
	PlaceId tempLoc;
	while (!QueueIsEmpty(locationQ) && found == FALSE) 
	{
		currLocation = QueueLeave(locationQ); 
		int numReturnedLocs;
		
		PlaceId *neighbours = GvGetReachable(
			hv->gv, hunter, roundArray[currLocation], currLocation, &numReturnedLocs);

		for (int j = 0; j < numReturnedLocs; j++)
		{	
			tempLoc = neighbours[j];
			// Has not been visited
			if (pathTo[tempLoc] == -1)
			{	
				pathTo[tempLoc] = currLocation;
				// The round we can go there is the next round
				roundArray[tempLoc] = roundArray[currLocation] + 1;
				QueueJoin(locationQ, tempLoc);

				if (tempLoc == dest)
				{
					found = TRUE;
					break;
				}
			}
		}
	} 

	dropQueue(locationQ);
	
	// Compute the path to dest backwardly using pathTo array
	PlaceId tempPath[MAX_REAL_PLACE];
	tempPath[0] = dest;
	int index = 1;
	while (pathTo[tempLoc] != src)
	{
		tempPath[index] = pathTo[tempLoc];
		tempLoc = pathTo[tempLoc];
		index += 1;
	}
	
	*pathLength = index;

	PlaceId *path = malloc(index * sizeof(PlaceId));

	// Copy the result reversely into the path array
	for (int j = 0; j < index; j++) path[j] = tempPath[index - j - 1];

	return path;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

// Round should be HvGetRound(hv) + 1 as, these functions need to info about
// round after current round
PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{
	Player player = HvGetPlayer(hv);
	PlaceId location = HvGetPlayerLocation(hv, player);
	if (location == UNKNOWN) {*numReturnedLocs = 0; return NULL;}
	return GvGetReachable(hv->gv, player, HvGetRound(hv) + 1, location, numReturnedLocs);
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	Player player = HvGetPlayer(hv);
	PlaceId location = HvGetPlayerLocation(hv, player);
	if (location == UNKNOWN) {*numReturnedLocs = 0; return NULL;}
	return GvGetReachableByType(hv->gv, player, HvGetRound(hv) + 1, location, road, 
								rail, boat, numReturnedLocs);
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
	if (player == PLAYER_DRACULA && draculaNotRevealed(hv)) *numReturnedLocs = 0; return NULL;
	return GvGetReachable(hv->gv, player, HvGetRound(hv) + 1, HvGetPlayerLocation(hv, player), 
							numReturnedLocs);
}

PlaceId *HvWhereCanTheyGoByType(HunterView hv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	if (player == PLAYER_DRACULA && draculaNotRevealed(hv)) {*numReturnedLocs = 0; return NULL;}
	
	if (player == PLAYER_DRACULA) { // ensure Dracula cannot travel by rail
		return GvGetReachableByType(hv->gv, player, HvGetRound(hv) + 1, HvGetPlayerLocation(hv, player), 
								road, rail, boat, numReturnedLocs);
	} else if (PLAYER_LORD_GODALMING <= player && player <= PLAYER_MINA_HARKER) {
		return GvGetReachableByType(hv->gv, player, HvGetRound(hv) + 1, 
											HvGetPlayerLocation(hv, player), 
											road, rail, boat, numReturnedLocs);
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// Helper functions
// Check whether Dracula's location has been revealed
static int draculaNotRevealed(HunterView hv)
{
	PlaceId location = HvGetPlayerLocation(hv, PLAYER_DRACULA);
	return (location == CITY_UNKNOWN || location == SEA_UNKNOWN 
			|| location == NOWHERE);
}

// Check whether location is a real location
static int isRealLocation(PlaceId location)
{
	return (location != CITY_UNKNOWN && location != SEA_UNKNOWN && location != UNKNOWN);
}