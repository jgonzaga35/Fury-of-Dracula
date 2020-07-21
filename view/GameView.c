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

#define STRLEN_OF_ROUND 40
#define NUM_OF_PLAYER	4
#define TURNS_PER_ROUND	5
#define STARTING_SCORE 366
#define DELIMITER 		" "
#define CURR_PLACE		0

struct gameView {
	// Explicit variables
	Round numRound;
	int score;
	int health[NUM_PLAYERS];
	char *pastPlays;
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
	return gv->currentPlayer;
}

int GvGetScore(GameView gv)
{
	return gv->score;
}

int GvGetHealth(GameView gv, Player player)
{ // -------- not sure if this is right until we compile --------
	int h = gv->health[player];
	if 
	(	
		0 <= player && player <= 3 && // hunter
		0 <= h && h <= GAME_START_HUNTER_LIFE_POINTS // health allowable
	) return h;
	
	if (player == 4 && h >= 0) return h; // Dracula

	return -1; // error
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION

	// If the player hasn't had a turn (The id doesn't apear in the first four play)

	// If hunter, reuturn last play's location

	// If Dracula, 
	

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
	/* 
	// Return NULL if no history yet?
	if (gv->pastPlays == NULL) {
		*numReturnedMoves = 0;
		*canFree = false;
		return NULL;
	}

	// Dynamically allocate array of PlaceIds
	PlaceId *pastMoves;

	char playerName = '\0';
	switch (player) {
		case 0: playerName = 'G'; // Lord Godalming
				break;
		case 1: playerName = 'S': // Dr Seward
				break;
		case 2: playerName = 'H'; // Van Helsing
				break;
		case 3: playerName = 'M'; // Mina Harker
				break;
		case 4: playerName = 'D'; // Dracula
				break;
	}
	assert(playerName != '\0');
	// How many PlaceIds to allocate?? Unsure...
	pastMoves = (PlaceId *)malloc(sizeof(PlaceId)*100);

	// Fill in PlaceId array with move history of given player.
	// Loop through pastPlays string...
	char *pastPlays = strdup(gv->pastPlays);
	int i = 0;
	for (char *move = strtok(pastPlays, " "); move != NULL; move = strtok(NULL, " ")) {
		if (move[0] == playerName) {
			// Doesnt work. move is a char * but pastMoves[i] is 
			// of type PlaceId.
			pastMoves[i] = move;
			i++; 
		}
	}
	*numReturnedMoves = i;

	// once the caller of this function is finished using this data
	// they can free it
	*canFree = false; 

	// Reverse order of array so the most recent moves
	// are in the lowest value indexes of array. 
	int start = 0;
	int end = i;
	char temp;
	while (start < end) {
		temp = pastMoves[start];
		pastMoves[start] = pastMoves[end];
		pastMoves[end] = temp;
		start++;
		end--;
	}
	// pastMoves returns an array filled with moves from oldest to newest.
	return pastMoves;
	*/
	
	*numReturnedMoves = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	/*
	PlaceId *MoveHistory = GvGetMoveHistory(gv, player, numReturnedMoves, true);
	if (MoveHistory == NULL) return NULL;

	PlaceId *LastMoves = (PlaceId *)malloc(sizeof(PlaceId)*100);
	int i = 0;
	while (i < numMoves && i < numReturnedMoves) {
		// Put moves from entire history from most recent to oldest
		// into LastMoves
		LastMoves[i] = MoveHistory[i];
		i++;
	}
	*canFree = true;
	return LastMoves;

	*/
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
