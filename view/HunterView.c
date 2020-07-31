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
	
	if (!isRealLocation(location) || round == 0) return NOWHERE;	// No real location exist

	if (location == TELEPORT) return CASTLE_DRACULA;

    return location;
}

PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{
	printf("DESTINATION: %d\n", dest);
	PlaceId visited[MAX_REAL_PLACE];
	PlaceId *path = malloc(sizeof(PlaceId *));	
	PlaceId currLocation;
	PlaceId src = HvGetPlayerLocation(hv, hunter);

	for (PlaceId i = 0; i < MAX_REAL_PLACE; i++) visited[i] = -1;
	
	printf("SOURCE: %d\n", src);
	
	// Initialize Queue and visited array
	Queue locationQ = newQueue();
	QueueJoin(locationQ, src);
	visited[src] = src;
	int numLocations = 0;
	
	Round currRound = HvGetRound(hv);
	printf("ROUND1: %d\n", currRound);
	int roundArray[70];				// Array that stores the round that we should arrive at a location
	for (int k = 0; k < 71; k++) 
		{roundArray[k] = 0;}
	roundArray[src] = currRound;			// we are at src at currRound
	
	while (!QueueIsEmpty(locationQ)) 
	{
		currLocation = QueueLeave(locationQ); 
		int numReturnedLocs;
		
		PlaceId *neighbours = getNeighbours(hv->gv, hunter, roundArray[currLocation], 
											currLocation, &numReturnedLocs);
		
		if (currLocation == dest)
		{
			int length = 0; 
			PlaceId temp[numLocations];
			while (currLocation != src) 
			{
				temp[length] = currLocation;
				length++;
				currLocation = visited[currLocation];
			}
			
			int index = length - 1;
			for(int k = 0; k < length; k++) {
				path[k] = temp[index];
				index--; 
			}
			printf("PATH: \n");
			for(int m = 0; m < length; m++) 
				{printf("%s\n", placeIdToName(path[m]));}

			*pathLength = length;
			printf("PATHLENGTH: %d\n", *pathLength);
			break;
		} 
		else 
		{
			for (int j = 0; j < numReturnedLocs; j++)
			{
				if (visited[neighbours[j]] == -1)
				{	
					visited[neighbours[j]] = currLocation;
					numLocations++;
					QueueJoin(locationQ, neighbours[j]);
					roundArray[neighbours[j]] = roundArray[currLocation]++;
					printf("CURR neigh: %s\n", placeIdToName(neighbours[j]));
				}
			}
		}

		printf("CURRLOC: %s ||| ROUNDNUM: %d\n", placeIdToName(currLocation), roundArray[currLocation]);

	} 	

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
static int draculaNotRevealed(HunterView hv)
{
	PlaceId location = HvGetPlayerLocation(hv, PLAYER_DRACULA);
	return (location == CITY_UNKNOWN || location == SEA_UNKNOWN 
			|| location == NOWHERE);
}

static int isRealLocation(PlaceId location)
{
	return (location != CITY_UNKNOWN && location != SEA_UNKNOWN && location != UNKNOWN);
}