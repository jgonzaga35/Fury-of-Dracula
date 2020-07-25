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
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	/*int numReturnedLocs = 0;
	bool canFree = false;
	PlaceId *trails = GvGetLocationHistory(hv->gv, PLAYER_DRACULA, &numReturnedLocs, &canFree);
	
	int i;
	PlaceId location;
	for (i = 0; i < numReturnedLocs; i++)
	{
		location = trails[i];
		if (isRealLocation(location)) break;
	}
	
	if (!isRealLocation(location) || i == 0) return NOWHERE;	// No real location exist

	*round = HvGetRound(hv) - i;
	if (location == TELEPORT) return CASTLE_DRACULA;
	
	return location;*/
	
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
	// TODO: Use standard BFS + A find neighbouring function (Make this in Map.c)
	PlaceId visited[NUM_REAL_PLACES];
	for (int i = 0; NUM_REAL_PLACES; i++) 
	{
		visited[i] = -1;
	}

	PlaceId prev[NUM_REAL_PLACES];
	for (int j = 0; NUM_REAL_PLACES; j++) 
	{
		prev[j] = 0;
	}
	
	PlaceId src = HvGetPlayerLocation(hv, hunter);
	Queue locationQ = newQueue();
	QueueJoin(locationQ, src);

	int currLocation = src;
	int currLength = 0;

	while (QueueIsEmpty(locationQ) == 0 && currLocation != dest) {
		currLocation = QueueLeave(locationQ);
		
		int numReturnedLocs;
		int *locations = getNeighbours(hv->gv, currLocation, hunter, GvGetRound(hv->gv), 
										&numReturnedLocs);
		
		
	}

	/*PlaceId src = HvGetPlayerLocation(hv, hunter); 
	int numReturnedLocs;
	
	PlaceId *locations = getNeighbours(hv->gv, src, hunter, HvGetRound(hv), &numReturnedLocs);
	*pathLength = 0;
	
	PlaceId visited[numReturnedLocs];
	for (int i = 0; i < numReturnedLocs; i++) {
		visited[i] = -1;
	}
	
	visited[src] = src;

	Queue locationQ = newQueue();
	QueueJoin(locationQ, src);
	
	int found = 0; 
	
	while (!QueueIsEmpty(locationQ)) {
	   	PlaceId v = QueueLeave(locationQ);
	   	if (v == dest) {
			found = 1;
			break;
	   	}
	   	for (int w = 0; w < numReturnedLocs; w++) {
	      	if (visited[w] == -1) {
	        	visited[w] = v;
	        	QueueJoin(locationQ, w);
	    	}
	    }
	}
	
	// if path to dest is found, want to path array to be in source to destination order
	if (found == 0) {
		// printf("hello\n"); // check if entering if condition
		int length = 0; 
		PlaceId currLocation = dest;
		PlaceId *path;
		PlaceId temp[numReturnedLocs];
		while (currLocation != src) {
			temp[length] = currLocation;
			length++;
			currLocation = visited[currLocation];
		}
		
		// store locations in path array
		int index = length - 1;
		for(int j = 0; j < length - 1; j++) {
			path[j] = temp[index];
			index--;
		}
		
		*pathLength = length - 1;
		return path; 
	}*/

	return NULL;
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
