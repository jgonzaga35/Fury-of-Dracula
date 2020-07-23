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
#include <ctype.h>

#define TRUE			1
#define FALSE			0
#define TURNS_PER_ROUND	5
#define CURR_PLACE		0
#define CHARS_PER_PLAY	8

struct gameView {
	// Explicit variables
	Map map;
	Round numTurn;
	int score;
	int health[NUM_PLAYERS];
	char *pastPlays;
	Player currentPlayer;
	PlaceId draculaDroppedTrail;				// The location of the place that drop from dracula's trail
	PlaceId trails[NUM_PLAYERS][TRAIL_SIZE];	// Never null
	PlaceId vampireLocation;					// Never null
	PlaceId trapLocations[TRAIL_SIZE];			// Null at sometimes, will be fixed later
	int numTrap;
};

// Function prototypes
static void initializeHealthScoreTurnsLocation(GameView gv);
static PlaceId getLocation(char firstLetter, char secondLetter);
static void updatePlayerLocation(GameView gv, char playerAbbrev, PlaceId place);
static int isDead(GameView gv, Player player);
static char * getCmd(char *pastPlays, int index);
static void performHunterAction(GameView gv, Player player, char cmd[4], PlaceId location);
static void goToHospital(GameView gv, Player player);
static void haveRest(GameView gv, Player player);
static void performDraculaAction(GameView gv, char firstCmd, char secondCmd, PlaceId draculaLocation);
static void updateEncounters(GameView gv, char cmd);
static void addTrap(GameView gv, PlaceId location);
static void removeTrap(GameView gv, PlaceId location);
static void updateLifePoint(GameView gv, PlaceId location);
static PlaceId traceHide(GameView gv);
static PlaceId traceDoubleBack(GameView gv);
static int isDoubleBack(PlaceId location);
static PlaceId trueLocation(GameView gv, PlaceId location);
// ---------------------Making a move helper functions--------------------------
static int validPlayer(Player player);

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

GameView GvNew(char *pastPlays, Message messages[])
{
	GameView new = malloc(sizeof(struct gameView));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}

	initializeHealthScoreTurnsLocation(new);

	int length = strlen(pastPlays);
	int i = 0;
	while (i < length) {
		PlaceId location = getLocation(pastPlays[i + 1], pastPlays[i + 2]);
		updatePlayerLocation(new, pastPlays[i], location);	
		Player player = new->currentPlayer; 

		switch (pastPlays[i])
		{
		case 'G': case 'S' : case 'H' : case 'M' :
			if (isDead(new, player)) new->health[player] = GAME_START_HUNTER_LIFE_POINTS;
			char *cmd = getCmd(pastPlays, i);
			performHunterAction(new, player, cmd, location); 
			haveRest(new, player);
			break;
		case 'D':
			updateEncounters(new, pastPlays[i + 5]);
			performDraculaAction(new, pastPlays[i + 3], pastPlays[i + 4], location);

			// Update score and life points
			updateLifePoint(new, location);
			//printf("Dracula has blood of %d\n", new->health[PLAYER_DRACULA]);
			new->score -= SCORE_LOSS_DRACULA_TURN;
			break;
		default:
			break;
		}

		new->numTurn += 1;
		i += CHARS_PER_PLAY;
	}
	new->pastPlays = pastPlays;
	new->currentPlayer = new->numTurn % NUM_PLAYERS;

	// Fix blood point if they fall below 0
	for (int i = 0; i < NUM_PLAYERS; i++)
	{
		if (new->health[i] < 0) new->health[i] = 0;
	}

	return new;
}

void GvFree(GameView gv)
{
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
	return gv->health[player];
	// int h = gv->health[player];
	// if 
	// (	
	// 	0 <= player && player <= 3 && // hunter
	// 	0 <= h && h <= GAME_START_HUNTER_LIFE_POINTS // health allowable
	// ) return h;
	
	// if (player == 4 && h >= 0) return h; // Dracula

	// return -1; // error
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{	
	PlaceId location = gv->trails[player][CURR_PLACE];
	return trueLocation(gv, location);
}

PlaceId GvGetVampireLocation(GameView gv)
{
	return gv->vampireLocation;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{	
	// printf("Number of trap is %d\n", gv->numTrap);
	// printf("Trap 1 is at %d\n", gv->trapLocations[0]);
	// printf("Trap 2 is at %d\n", gv->trapLocations[1]);
	// printf("Trap 3 is at %d\n", gv->trapLocations[2]);
	*numTraps = gv->numTrap;
	PlaceId *trapLocation = malloc(*numTraps * sizeof(PlaceId));

	// for (int i = 0, j = 0; i < TRAIL_SIZE; i++) {
	// 	if (gv->trapLocations[i] != NOWHERE)
	// 	{
	// 		trapLocation[j] = gv->trapLocations[i];
	// 		j++;
	// 	}
	// }

	for (int i = 0; i < *numTraps; i++)
	{
		trapLocation[i] = gv->trapLocations[i];
	}

	return trapLocation;
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
			char abbreviation[] = {move[1], move[2], '\0'};
			placeAbbrev = strdup(abbreviation);
			pastMoves[i] = placeAbbrevToId(placeAbbrev);
			i++; 
		}
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
	if (MoveHistory == NULL) {
		*canFree = true;
		*numReturnedMoves = 0;
		return NULL;
	}

	PlaceId *LastMoves = (PlaceId *)malloc(sizeof(PlaceId)*100);
	int i = 0;
	while (i < numMoves && i < *numReturnedMoves) {
		LastMoves[i] = MoveHistory[i];
		i++;
	}

	free(MoveHistory);
	*numReturnedMoves = i;
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

	// for hunters, GvGetMoveHistory = GvGetLocationHistory;
	PlaceId *pastMoves = GvGetMoveHistory(gv, player, numReturnedLocs, canFree);
	// assuming GvGetMoveHistory changes value of numReturnedLocs to numReturnedMoves...
	if (player >= PLAYER_LORD_GODALMING && player <= PLAYER_MINA_HARKER) {
		*canFree = true;
		return pastMoves;
	} 

	// For Dracula:
	PlaceId *pastLocs = (PlaceId *)malloc(sizeof(PlaceId)*100);
	assert(pastLocs != NULL);
	int i = 0;
	// messy, will fix later
	// might be bug if i + 6 exceeds array index.
	while (i < *numReturnedLocs) {
		if (pastMoves[i] == HIDE) {
			pastLocs[i] = pastMoves[i - 1];
			while (pastLocs[i] >= HIDE && pastLocs[i] <= DOUBLE_BACK_5) {
				if (pastLocs[i] == DOUBLE_BACK_1) {
					pastLocs[i] = pastMoves[i - 2];
				}
				else if (pastLocs[i] == DOUBLE_BACK_2) {
					pastLocs[i] = pastLocs[i - 3];
				}
				else if (pastLocs[i] == DOUBLE_BACK_3) {
					pastLocs[i] = pastMoves[i - 4];
				}
				else if (pastLocs[i] == DOUBLE_BACK_4) {
					pastLocs[i] = pastMoves[i - 5];
				}
				else if (pastLocs[i] == DOUBLE_BACK_5) {
					pastLocs[i] = pastMoves[i - 6];
				}
			}
		} 
		else if (pastMoves[i] == DOUBLE_BACK_1) {
			pastLocs[i] = pastMoves[i - 1];
		}
		else if (pastMoves[i] == DOUBLE_BACK_2) {
			pastLocs[i] = pastMoves[i - 2];
		}
		else if (pastMoves[i] == DOUBLE_BACK_3) {
			pastLocs[i] = pastMoves[i - 3];
		}
		else if (pastMoves[i] == DOUBLE_BACK_4) {
			pastLocs[i] = pastMoves[i - 4];
		}
		else if (pastMoves[i] == DOUBLE_BACK_5) {
			pastLocs[i] = pastMoves[i - 5];
		}
		else if (pastMoves[i] == TELEPORT) {
			pastLocs[i] = CASTLE_DRACULA;
		} else {
			pastLocs[i] = pastMoves[i];
		}
		i++;
	}

	*numReturnedLocs = i;
	*canFree = true;
	free(pastMoves);
	return pastLocs;
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	PlaceId *pastLocs = GvGetLocationHistory(gv, player, numReturnedLocs, canFree);
	if (pastLocs == NULL) {
		*canFree = true;
		*numReturnedLocs = 0;	
		return NULL;
	}

	PlaceId *lastLocs = (PlaceId *)malloc(sizeof(PlaceId)*100);
	int i = 0;
	while (i < numLocs && i < *numReturnedLocs) {
		lastLocs[i] = pastLocs[i];
		i++;
	}

	free(pastLocs);
	*numReturnedLocs = i;
	*canFree = true;
	return lastLocs;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

// returns ture if valid player, else returns false
static int validPlayer(Player player) {
	if (PLAYER_LORD_GODALMING <= player && player <= PLAYER_DRACULA) return 1;
	else return 0;
}

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{
	if (gv == NULL) return NULL;
	if (!validPlayer(player)) return NULL;

	*numReturnedLocs = 0;
	Map europe = MapNew();
	ConnList CNN;
	
	if (PLAYER_LORD_GODALMING <= player && player <= PLAYER_MINA_HARKER) {
		// Player is a hunter

	} else if (player == PLAYER_DRACULA) { // Player is dracula

	}

	MapFree(europe);
	return NULL;
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	if (gv == NULL) return NULL;
	if (!validPlayer(player)) return NULL;	
	//TODO:Justin could you check if this is corret, I add the (player) so that it can compile
	if (!(numReturnedLocs >= 0)) *numReturnedLocs = 0;
	//printf("HELLO\n");
	PlaceId *allowableCNC = malloc(MAX_REAL_PLACE * sizeof(PlaceId));
	//printf("whats cooking\n");
	ConnList CNC = MapGetConnections(gv->map, from); // reachable connections
	//printf("END\n");
	if (road) getRoadCNC(CNC, allowableCNC, numReturnedLocs);
	if (rail) getRailCNC(CNC, allowableCNC, numReturnedLocs, round, player, from);
	if (boat) getBoatCNC(CNC, allowableCNC, numReturnedLocs);

	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// Helper functions

// Set the initial state of all varaibles
static void initializeHealthScoreTurnsLocation(GameView gv) 
{
	gv->score = GAME_START_SCORE;
	gv->numTurn = 0;
	gv->numTrap = 0;

	for (int i = 0; i < NUM_PLAYERS; i++) 
	{
		gv->health[i] = GAME_START_HUNTER_LIFE_POINTS;
		for (int j = 0; j < TRAIL_SIZE; j++) 
		{
			gv->trails[i][j] = NOWHERE;
			gv->trapLocations[j] = NOWHERE;
		}
	}
	gv->health[PLAYER_DRACULA] = GAME_START_BLOOD_POINTS;

	gv->draculaDroppedTrail = NOWHERE;
	gv->vampireLocation = NOWHERE;

	return;
}

// Return the place represent by the abbreviation
static PlaceId getLocation(char firstLetter, char secondLetter)
{
	char abbrev[3];
	abbrev[0] = firstLetter;
	abbrev[1] = secondLetter;
	abbrev[2] = '\0';

	if (!strcmp("C?", abbrev)) 
	{
		return CITY_UNKNOWN;
	} else if (!strcmp("S?", abbrev)) 
	{
		return SEA_UNKNOWN;
	} else if(!strcmp("HI", abbrev)) 
	{
		return HIDE;
	} else if (!strcmp("TP", abbrev)) 
	{
		return TELEPORT;
	} else if (abbrev[0] == 'D' && isdigit(abbrev[1])) 
	{	
		int i = abbrev[1] - '0';
		//printf("Location return is D%d\n", i);
		return (102 + i);
	}
	return placeAbbrevToId(abbrev);
}

// Update the currentPlayer and their respective location
static void updatePlayerLocation(GameView gv, char playerAbbrev, PlaceId place)
{
	switch (playerAbbrev)
	{
	case 'G':
		gv->currentPlayer = PLAYER_LORD_GODALMING;
		break;
	case 'S':
		gv->currentPlayer = PLAYER_DR_SEWARD;
		break;
	case 'H':
		gv->currentPlayer = PLAYER_VAN_HELSING;
		break;
	case 'M':
		gv->currentPlayer = PLAYER_MINA_HARKER;
		break;
	case 'D':
		gv->currentPlayer = PLAYER_DRACULA;
		break;
	default:
		fprintf(stderr, "Player character is incorrent exp: GSHMD act: %c\n", playerAbbrev);
        exit(EXIT_FAILURE);
		break;
	}

	// Record the location that will be dropped from the trail
	if (playerAbbrev == 'D') gv->draculaDroppedTrail = gv->trails[PLAYER_DRACULA][TRAIL_SIZE - 1];

	Player player = gv->currentPlayer;
	for (int i = TRAIL_SIZE - 1; i > 0; i--) {
		gv->trails[player][i] = gv->trails[player][i - 1];
	}

	gv->trails[player][CURR_PLACE] = place;
}

// Check if the hunter is dead
static int isDead(GameView gv, Player player)
{
	return gv->health[player] <= 0; 
}

// Extract the four character encounter code
static char *getCmd(char *pastPlays, int index) 
{
	char *cmd = malloc(4 * sizeof(char));
	for (int i = 0; i < 4; i++) 
	{
		cmd[i] = pastPlays[index + 3 + i];
	}
	return cmd;
}

// Reset the life point, shift the trail to hospital and decrease the score
static void goToHospital(GameView gv, Player player) 
{
	gv->health[player] = 0;
	for (int i = TRAIL_SIZE; i > 0; i--) 
	{
		gv->trails[player][i] = gv->trails[player][i - 1];
	}
	gv->trails[player][CURR_PLACE] = ST_JOSEPH_AND_ST_MARY;

	gv->score -= SCORE_LOSS_HUNTER_HOSPITAL;
}

// Deal with hunter's encounter
static void performHunterAction(GameView gv, Player player, char cmd[4], PlaceId location)
{
	for (int i = 0; i < 4; i++) {
		switch (cmd[i])
		{
		case 'T':
			removeTrap(gv, location);
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
static void haveRest(GameView gv, Player player) {
	if (gv->trails[player][CURR_PLACE + 1] == gv->trails[player][CURR_PLACE]) {
		gv->health[player] += LIFE_GAIN_REST;
		if (gv->health[player] >= GAME_START_HUNTER_LIFE_POINTS) gv->health[player] = GAME_START_HUNTER_LIFE_POINTS;
	}
}
// Add a trap
static void addTrap(GameView gv, PlaceId location)
{
	gv->trapLocations[gv->numTrap] = location;
	gv->numTrap += 1;
}

// Remove a trap
static void removeTrap(GameView gv, PlaceId location)
{	
	int i;
	for (i = 0; i < TRAIL_SIZE; i++) {
		if (gv->trapLocations[i] == location)
		{
			for (int j = i; j < TRAIL_SIZE - 1; j++) {
				gv->trapLocations[j] = gv->trapLocations[j + 1];
			}
		}
	}

	gv->numTrap -= 1;
}

// Place encounter at Dracula's location
static void performDraculaAction(GameView gv, char firstCmd, char secondCmd, PlaceId draculaLocation) 
{	
	switch(firstCmd) {
		case '.': break;
		case 'T':
			// TODO: still not sure whether use ture location
			addTrap(gv, trueLocation(gv, draculaLocation));
			break;
	}
	
	switch(secondCmd) {
		case '.': break;
		case 'V':
			gv->vampireLocation = trueLocation(gv, draculaLocation);
			break;
	}
}

static void updateEncounters(GameView gv, char cmd)
{
	switch(cmd) {
		case '.': break;
		case 'M':
			removeTrap(gv, gv->draculaDroppedTrail);
			break;
		case 'V':
			gv->score -= SCORE_LOSS_VAMPIRE_MATURES;
			gv->vampireLocation = NOWHERE;
			break;
	}
}

// Trace what place hide refer to
static PlaceId traceHide(GameView gv)
{	
	PlaceId location = gv->trails[PLAYER_DRACULA][1];

	if (location == TELEPORT) return CASTLE_DRACULA;
	if (!isDoubleBack(location)) return location;

	int backIndex = location - 102;
	// If HIDE refers to D5
	if (backIndex == 5) return gv->draculaDroppedTrail;
	return gv->trails[PLAYER_DRACULA][backIndex + 1];
}

// Trace what place DB refer to
static PlaceId traceDoubleBack(GameView gv)
{	
	// Get the backIndex
	PlaceId doubleBack = gv->trails[PLAYER_DRACULA][0];
	int backIndex = doubleBack - 102;
	//printf("The backIndex is %d\n", backIndex);
	PlaceId location = gv->trails[PLAYER_DRACULA][backIndex];
	//printf("The location returned for DB is %d\n", location);

	if (location == TELEPORT) return CASTLE_DRACULA;
	if (location != HIDE) return location;
	
	// If D5 refers to HIDE
	if (backIndex == 5) return gv->draculaDroppedTrail;
	return gv->trails[PLAYER_DRACULA][backIndex + 1];
}

// Check if Dracula is in sea or Castle
static void updateLifePoint(GameView gv, PlaceId location) 
{	
	PlaceId realLocation = trueLocation(gv, location);
	if (placeIdToType(realLocation) == SEA) gv->health[PLAYER_DRACULA] -= LIFE_LOSS_SEA;
	else if (realLocation == CASTLE_DRACULA) gv->health[PLAYER_DRACULA] += LIFE_GAIN_CASTLE_DRACULA;
}

// Check if the location is Double back
static int isDoubleBack(PlaceId location) 
{
	return (location >= DOUBLE_BACK_1 && location <= DOUBLE_BACK_5);
}

// Return the actual location
static PlaceId trueLocation(GameView gv, PlaceId location) {
	if (location == TELEPORT) return CASTLE_DRACULA;
	else if (location == HIDE) return traceHide(gv);
	else if (isDoubleBack(location)) return traceDoubleBack(gv);
	return location;
}