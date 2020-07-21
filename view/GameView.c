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
#define TURNS_PER_ROUND	5
#define DELIMITER 		" "
#define CURR_PLACE		0

struct gameView {
	// Explicit variables
	Round numTurn;
	int score;
	int health[NUM_PLAYERS];
	char *pastPlays;
	Player currentPlayer;
	PlaceId trails[NUM_PLAYERS][TRAIL_SIZE];	// Never null
	PlaceId vampireLocation;					// Never null
	PlaceId trapLocations[TRAIL_SIZE];			// Null at sometimes, will be fixed later
	int numTrap;
};

// Function prototypes
static void initializeHealthScoreTurnsLocation(GameView gv);
static PlaceId getLocation(char firstLetter, char secondLetter);
static void updatePlayerLocation(GameView gv, Player player, PlaceId place);
static int isDead(GameView gv, Player player);
static char * getCmd(char *pastPlays, int index);
static void performHunterAction(GameView gv, Player player, char cmd[4], PlaceId location);
static void goToHospital(GameView gv, Player player);
static void haveRest(GameView gv, Player player, PlaceId location);
static void performDraculaAction(GameView gv, char firstCmd, char secondCmd, PlaceId draculaLocation);
static void updateEncounters(GameView gv, char cmd);

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

GameView GvNew(char *pastPlays, Message messages[])
{
	GameView new = malloc(sizeof(struct gameView));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}

	int length = strlen(pastPlays);

	initializeHealthScoreTurnsLocation(new);

	int i;
	while (i < length) {
		PlaceId location = getLocation(pastPlays[i + 1], pastPlays[i + 2]);
		updatePlayerLocation(new, pastPlays[i], location);
		char p = pastPlays[i];
		Player player = new->currentPlayer; 

		switch (p)
		{
		case 'G': case 'S' : case 'H' : case 'M' :
			if (isDead(new, player)) new->health[player] = GAME_START_HUNTER_LIFE_POINTS;
			char *cmd = getCmd(pastPlays, i);
			performHunterAction(new, player, cmd, location); 
			haveRest(new, player, location);
			break;
		case 'D':
			performDraculaAction(new, pastPlays[i + 4], pastPlays[i + 5], location);
			updateEncounters(new, pastPlays[i + 5]);
			break;
		default:
			break;
		}


		new->numTurn += 1;
		i += 8;
	}

	return new;
}

static void initializeHealthScoreTurnsLocation(GameView gv) 
{
	gv->score = GAME_START_SCORE;
	gv->numTurn = 0;
	gv->numTrap = 0;

	for (int i = 0; i < NUM_PLAYERS; i++) {
		gv->health[i] = GAME_START_HUNTER_LIFE_POINTS;
		for (int j = 0; j < TRAIL_SIZE; j++) {
			gv->trails[i][j] = NOWHERE;
		}
	}
	gv->health[PLAYER_DRACULA] = GAME_START_BLOOD_POINTS;

	gv->vampireLocation = NOWHERE;

	return;
}

// Update the currentPlay and their location
static void updatePlayerLocation(GameView gv, Player player, PlaceId place)
{
	switch (player)
	{
	case PLAYER_LORD_GODALMING:
		gv->currentPlayer = PLAYER_LORD_GODALMING;
		break;
	case PLAYER_DR_SEWARD:
		gv->currentPlayer = PLAYER_DR_SEWARD;
		break;
	case PLAYER_VAN_HELSING:
		gv->currentPlayer = PLAYER_VAN_HELSING;
		break;
	case PLAYER_MINA_HARKER:
		gv->currentPlayer = PLAYER_MINA_HARKER;
		break;
	case PLAYER_DRACULA:
		gv->currentPlayer = PLAYER_DRACULA;
		break;
	default:
		fprintf(stderr, "Player character is incorrent exp: GSHMD act: %c\n", player);
        exit(EXIT_FAILURE);
		break;
	}

	for (int i = TRAIL_SIZE - 1; i > 0; i--) {
		gv->trails[player][i] = gv->trails[player][i - 1];
	}

	gv->trails[player][CURR_PLACE] = place;
}


static PlaceId getLocation(char firstLetter, char secondLetter)
{
	char name[3];
	name[0] = firstLetter;
	name[1] = secondLetter;
	name[2] = '\0';
	return placeAbbrevToId(name);
}

// Check if the hunter is dead
static int isDead(GameView gv, Player player)
{
	return gv->health[player] <= 0; 
}

// Extract the four character encounter code
static char *getCmd(char *pastPlays, int index) 
{
	char cmd[4];
	for (int i = 0; i < 4; i++) 
	{
		cmd[i] = pastPlays[index + 4];
	}
	return cmd[0];
}

// Reset the life point and shift the trail to hospital
static void goToHospital(GameView gv, Player player) 
{
	gv->health[player] = 0;
	for (int i = TRAIL_SIZE; i > 0; i--) 
	{
		gv->trails[player][i] = gv->trails[player][i - 1];
	}
	gv->trails[player][CURR_PLACE] = ST_JOSEPH_AND_ST_MARY;
}

// Deal with hunter's encounter
static void performHunterAction(GameView gv, Player player, char cmd[4], PlaceId location)
{
	for (int i = 0; i < 4; i++) {
		switch (cmd[i])
		{
		case 'T':
			// TODO: Remove the trap from the traplocation
			gv->health[player] -= LIFE_LOSS_TRAP_ENCOUNTER;
			if (isDead(gv, player)) 
			{
				goToHospital(gv, player);
				goto exit_forloop;
			}
			break;
		case 'V':
			gv->vampireLocation = NOWHERE;
			break;
		case 'D':
			gv->health[player] -= LIFE_LOSS_DRACULA_ENCOUNTER;
			gv->health[PLAYER_DRACULA] -= LIFE_LOSS_HUNTER_ENCOUNTER;
			if (isDead(gv, player)) 
			{
				goToHospital(gv, player);
				goto exit_forloop;
			}
			break;
		case '.':
			break;
		default:
			fprintf(stderr, "Hunter encounter character is incorrent exp: TVD act: %c\n", cmd[i]);
        	exit(EXIT_FAILURE);
			break;
		}
	}
	exit_forloop: ;
}

// Check if the hunter performed rest
static void haveRest(GameView gv, Player player, PlaceId location) {
	if (gv->trails[player][CURR_PLACE + 1] == location) {
		gv->health[player] += LIFE_GAIN_REST;
		if (gv->health[player] >= GAME_START_HUNTER_LIFE_POINTS) gv->health[player] = GAME_START_HUNTER_LIFE_POINTS;
	}
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
	return gv->numTurn / TURNS_PER_ROUND;
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
	return gv->vampireLocation;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	*numTraps = gv->numTrap;
	return gv->trapLocations;
}

////////////////////////////////////////////////////////////////////////
// Game History

PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	// Return NULL if no history yet?
	if (gv->pastPlays == NULL) {
		*numReturnedMoves = 0;
		*canFree = false;
		return NULL;
	}

	char playerName = '\0';
	switch (player) {
		case PLAYER_LORD_GODALMING: playerName = 'G'; 
				break;
		case PLAYER_DR_SEWARD: playerName = 'S'; 
				break;
		case PLAYER_VAN_HELSING: playerName = 'H'; 
				break;
		case PLAYER_MINA_HARKER: playerName = 'M'; 
				break;
		case PLAYER_DRACULA: playerName = 'D';
				break;
	}
	// error handling
	assert(playerName != '\0');

	// Dynamically allocate array of PlaceIds
	// How many PlaceIds to allocate?? Unsure...
	PlaceId *pastMoves = (PlaceId *)malloc(sizeof(PlaceId)*100);

	// Fill in PlaceId array with move history of given player.
	// Loop through pastPlays string...
	char *pastPlays = strdup(gv->pastPlays);
	int i = 0;
	char *placeAbbrev;
	for (char *move = strtok(pastPlays, " "); move != NULL; move = strtok(NULL, " ")) {
		if (move[0] == playerName) {
			// Store each move by their PlaceId.
			char abbreviation[] = {move[1], move[2]};
			placeAbbrev = strdup(abbreviation);
			pastMoves[i] = placeAbbrevToId(placeAbbrev);
			i++; 
		}
	}

	// Reverse order of array so the most recent moves
	// are in the lowest value indexes of array. 
	PlaceId start = 0;
	PlaceId end = i;
	PlaceId temp;
	while (start < end) {
		temp = pastMoves[start];
		pastMoves[start] = pastMoves[end];
		pastMoves[end] = temp;
		start++;
		end--;
	}

	// numReturnedMoves = no. of iterations through pastPlays.
	*numReturnedMoves = i;

	// caller of this function should free this afterwards.
	*canFree = true;

	// pastMoves returns an array filled with moves from oldest to newest.
	return pastMoves;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	PlaceId *MoveHistory = GvGetMoveHistory(gv, player, numReturnedMoves, canFree);
	if (MoveHistory == NULL) return NULL;

	PlaceId *LastMoves = (PlaceId *)malloc(sizeof(PlaceId)*100);
	int i = 0;
	while (i < numMoves && i < *numReturnedMoves) {
		// Put moves from entire history from most recent to oldest
		// into LastMoves
		LastMoves[i] = MoveHistory[i];
		i++;
	}
	// don't know if this is correct
	free(MoveHistory);

	// return answer
	*canFree = true;
	return LastMoves;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	// Error handling
	assert(gv != NULL);

	// Special Case: empty pastPlays string
	if (gv->pastPlays == NULL) {
		*numReturnedLocs = 0;
		*canFree = true;
		return NULL;
	}

	// Create dynamically allocated PlaceId array.
	// *** I DONT KNOW IF THIS WORKS YET ***
	PlaceId *pastMoves = GvGetMoveHistory(gv, player, numReturnedLocs, canFree);
	PlaceId *pastLocs = (PlaceId *)malloc(sizeof(PlaceId)*100);
	assert(pastLocs != NULL);
	int i = 0;
	int j = 0;
	while (i < *numReturnedLocs) {
		// Ignore Hide or BackTrack moves from Dracula.
		if (pastMoves[i] >= HIDE && pastLocs[i] <= TELEPORT) {
			
		} else {
			pastLocs[j] = pastMoves[i];
			j++;
		}
		i++;
	}

	*numReturnedLocs = j;
	*canFree = true;
	free(pastMoves);
	return pastLocs;
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId *LocHistory = GvGetLocationHistory(gv, player, numReturnedLocs, canFree);
	if (LocHistory == NULL) return NULL;

	PlaceId *LastLocs = (PlaceId *)malloc(sizeof(PlaceId)*100);
	int i = 0;
	while (i < numLocs && i < *numReturnedLocs) {
		// Put moves from entire history from most recent to oldest
		// into LastMoves
		LastLocs[i] = LocHistory[i];
		i++;
	}
	// don't know if this is correct
	free(LocHistory);

	// return answer
	*canFree = true;
	return LastLocs;

	// *numReturnedLocs = 0;
	// *canFree = false;
	// return 0;
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

// Place encounter at Dracula's location
static void performDraculaAction(GameView gv, char firstCmd, char secondCmd, PlaceId draculaLocation) 
{	
	int N = gv->numTrap;
	switch(firstCmd) {
		case '.': break;
		case 'T':
			for (int i = N; i > 0; i--) {
				gv->trapLocations[N] = gv->trapLocations[N];
			}
			gv->trapLocations[CURR_PLACE] = draculaLocation;
			gv->numTrap += 1;
			break;
	}
	
	switch(secondCmd) {
		case '.': break;
		case 'V':
			gv->vampireLocation = draculaLocation;
			break;
	}
}

static void updateEncounters(GameView gv, char cmd)
{
	switch(cmd) {
		case '.': break;
		case 'M':
			// TODO: Remove the last trap in the trails;
			break;
		case 'V':
			gv->score -= SCORE_LOSS_DRACULA_TURN;
			gv->vampireLocation = NOWHERE;
			break;
	}
}