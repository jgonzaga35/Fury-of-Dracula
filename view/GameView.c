////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// GameView.c: GameView ADT implementation
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
#include "Map.h"
#include "Places.h"

// add your own #includes here
#include <string.h>

#define TURNS_PER_ROUND	5
#define STARTING_SCORE 366
#define DELIMITER 		" "
#define CURR_PLACE		0

struct gameView {
	// Explicit variables
	Round numRound;
	int score;
	int health[NUM_PLAYERS];

	Player currentPlayer;
	PlaceId trails[NUM_PLAYERS][TRAIL_SIZE];
	PlaceId vampireLocation;
	PlaceId trapLocations[TRAIL_SIZE];
};

// Function prototypes
////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

GameView GvNew(char *pastPlays, Message messages[])
{
	GameView new = malloc(sizeof(struct gameView));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}

	return new;
}

void GvFree(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	free(gv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round GvGetRound(GameView gv)
{
	return gv->numRound;
}

Player GvGetPlayer(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return PLAYER_LORD_GODALMING;
}

int GvGetScore(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

int GvGetHealth(GameView gv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	return NOWHERE;
}

PlaceId GvGetVampireLocation(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return NOWHERE;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numTraps = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Game History

PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
/**
 * Gets  the complete move history, in chronological order, of the given
 * player as shown in the play string.
 *
 * This  function  should  return  the  moves in a dynamically allocated
 * array, and set *numReturnedMoves to the number of moves returned.
 *
 * This  function can decide whether the caller is allowed to  modify or
 * free the returned array. (If the array is part of the  GameView  data
 * structure,  you may not want the caller to modify or free it.) If the
 * returned array can be modified/freed, set *canFree to true  to  avoid
 * memory leaks. Otherwise, set it to false.
 */
	/* 
	// Dynamically allocate array of PlaceIds
	PlaceId *history;

	// How many PlaceIds to allocate?? Unsure...
	history = (PlaceId *)malloc(sizeof(PlaceId)*TRAIL_SIZE);

	// To count numReturnedMoves:
	// 1. Initialise history array with -1.
	// 2. Fill in history array with move history.
	// 3. Count elements in array until history[i] == -1.

	// Fill in PlaceId array with move history of given player.
	// How to find move history of given player???
	for (int i = 0; i < TRAIL_SIZE; i++) {
		history[i] = 

	}

	// When should the caller of this function be allowed
	// to modify or free the array?? 
	*/
	
	*numReturnedMoves = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	*canFree = false;
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO

// static void performDraculaAction(GameView gv, char cmd, int turns) {
// 	switch(cmd) {
// 		case '.': break;
// 		case 'T':
// 			placeEncounter();
// 			break;
// 		case 'M':
// 			break;
// 		case 'V':
// 			if (turns == 5) gv->score -= SCORE_LOSS_VAMPIRE_MATURES;
// 			else gv->vampireLocation = gv->trails[PLAYER_DRACULA][CURR_PLACE];
// 			break;
// 	}
// }
