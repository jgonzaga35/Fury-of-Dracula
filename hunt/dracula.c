////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////
#include <time.h>
#include "dracula.h"
#include "DraculaView.h"
#include "Game.h"
#include "Map.h"
#include "Places.h"
#include "GameView.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUM_PORT_CITIES 28
#define SIZE_OF_ENGLAND 6
#define SIZE_OF_SPAIN 6
#define SIZE_OF_ITALY 7
#define SIZE_OF_CENTRAL_EUROPE 12
#define SIZE_OF_EAST_EUROPE 12
#define SIZE_OF_WEST_SEAS 5
#define SIZE_OF_CENTRAL_SEAS 4

void prioritiseCastleDrac(int riskLevel[], PlaceId hunterLocs[]);
PlaceId MoveToLocation(PlaceId *pastLocs, PlaceId location, int *numPastLocs);
bool isPortCity(PlaceId i, PlaceId PortCities[]);
void getHunterLocs(DraculaView dv, PlaceId hunterLocs[]);
int huntersNearCastle(PlaceId hunterLocs[]);
int huntersInCountry (PlaceId country[], PlaceId hunterLocs[], int size);
int isDoubleBack(PlaceId location);
bool isCountry (PlaceId country[], PlaceId location, int size);

void decideDraculaMove(DraculaView dv)
{
	int health = DvGetHealth(dv, PLAYER_DRACULA); // Dracula's Blood Points.
	int numValidMoves = 0;						  // Number of Valid Locations for Dracula.	
	int numRiskyLocs = 0;					      // Number of Risky Locations for Dracula.
	int numPastLocs = 0;						  // Number of Past Locations in Dracula's move history.
	PlaceId hunterLocs[4]; 
	getHunterLocs(dv, hunterLocs); 				  // Array of current hunter locations.
	                             
	Round round = DvGetRound(dv);				  // The current round in the game.
	time_t t;
	srand((unsigned) time(&t));					  // seed for random movements.  
	int riskLevel[NUM_REAL_PLACES] = {0};		  // Array containing risk levels for each place. 
	char *play = malloc(sizeof(char) * 2); 		  // The play to be made.
	PlaceId PortCities[] = {BARI, ALICANTE, AMSTERDAM, ATHENS, CADIZ, GALWAY,
							LISBON, BARCELONA, BORDEAUX, NANTES, SANTANDER,
							CONSTANTA, VARNA, CAGLIARI, DUBLIN, EDINBURGH, 
							LE_HAVRE, LONDON, PLYMOUTH, GENOA, HAMBURG,
							SALONICA, VALONA, LIVERPOOL, SWANSEA, MARSEILLES,
							NAPLES, ROME};
	PlaceId England[] = {EDINBURGH, LONDON, MANCHESTER, LIVERPOOL, SWANSEA, PLYMOUTH};
	// PlaceId Ireland[] = {GALWAY, DUBLIN, IRISH_SEA};
	// PlaceId Portugal[] = {LISBON};
	// PlaceId WesternSeas[] = {NORTH_SEA, ENGLISH_CHANNEL, IRISH_SEA, BAY_OF_BISCAY, ATLANTIC_OCEAN};
	// PlaceId CentralSeas[] = {MEDITERRANEAN_SEA, TYRRHENIAN_SEA, ADRIATIC_SEA, IONIAN_SEA};
	// PlaceId EastEurope[] = {VIENNA, SARAJEVO, ZAGREB, BUDAPEST, KLAUSENBURG, CASTLE_DRACULA,
	// 						GALATZ, CONSTANTA, BUCHAREST, SOFIA, SZEGED, VARNA};
	// PlaceId CentralEurope[] = {STRASBOURG, BRUSSELS, COLOGNE, AMSTERDAM, HAMBURG, LEIPZIG,
	// 							PRAGUE, NUREMBURG, ZURICH, MUNICH, FRANKFURT, BERLIN};
	PlaceId Spain[] = {MADRID, GRANADA, ALICANTE, SARAGOSSA, BARCELONA, SANTANDER, LISBON};
	// PlaceId France[] = {TOULOUSE, CLERMONT_FERRAND, PARIS, MARSEILLES, LE_HAVRE, NANTES,
	// 					GENEVA, STRASBOURG};
	PlaceId Italy[] = {GENOA, FLORENCE, ROME, NAPLES, BAY_OF_BISCAY, VENICE, MILAN};

	///////////////////////////////////////////////////////////////////
	// ----------------------STARTING ROUND------------------------- //
	///////////////////////////////////////////////////////////////////

	// Dracula has the most movement options in STRASBOURG.
	if (round == 0) {
		registerBestPlay("PA", "come at me bro");
		return;
	}

	///////////////////////////////////////////////////////////////////
	// ----------------------GETTING VALID MOVES-------------------- //
	///////////////////////////////////////////////////////////////////

	// If Dracula has no valid moves, use TELEPORT.
	PlaceId *validMoves = DvGetValidMoves(dv, &numValidMoves);
	if (numValidMoves == 0 || validMoves == NULL) {
		registerBestPlay("TP", "I love COMP2521");
		return;
	}

	// Go to Castle Dracula if possible - Dracula wants to gain 10 BP.
	// Even if a hunter is there, it will be an even exchange. 
	PlaceId *pastLocs = DvGetLocationHistory(dv, &numPastLocs);  
	for (int i = 0; i < numValidMoves; i++) {
		// If any of the Valid Moves correspond to CASTLE_DRACULA:
		if (MoveToLocation(pastLocs, validMoves[i], &numPastLocs) == CASTLE_DRACULA) {	
			// If there are hunters at/around CASTLE_DRACULA
			if ((health <= 10 && huntersNearCastle(hunterLocs) <= 2) || (huntersNearCastle(hunterLocs) <= 1 && huntersInCountry(Italy, hunterLocs, SIZE_OF_ITALY) < 1)) {									
				registerBestPlay(strdup(placeIdToAbbrev(validMoves[i])), "oi, you want fight?");
				return;
			}
		} 
	}

	////////////////////////////////////////////////////////////////////
	// --------------ASSIGNING RISK LEVELS TO EACH LOCATION---------- //
	////////////////////////////////////////////////////////////////////

	// -------------LOCATIONS REACHABLE BY HUNTERS-------------------
	for (int player = 0; player < 4; player++) {

		// Hunter's Current Location: +2 Risk
		riskLevel[hunterLocs[player]] += 2;

		// Locations reachable by road: +3 Risk
		// **Note that locations reachable by multiple hunters will have up to +12 Risk!
		PlaceId *riskyLocsRoad = DvWhereCanTheyGoByType(dv, player, true, false, false, &numRiskyLocs);
		
		// Dracula will take risks if he is healthy enough...
		int riskNum = 0;
		if (DvGetHealth(dv, player) <= 6 && health >= 40) riskNum = 0;
		if (DvGetHealth(dv, player) <= 6 && health < 40) riskNum = 2;
		else riskNum = 3;
		for (int i = 0; i < numRiskyLocs; i++) riskLevel[riskyLocsRoad[i]] += riskNum;
		
		// Locations reachable by rail: +2 Risk
		PlaceId *riskyLocsRail = DvWhereCanTheyGoByType(dv, player, false, true, false, &numRiskyLocs);
		for (int i = 0; i < numRiskyLocs; i++) riskLevel[riskyLocsRail[i]] += 2;
	}

	// --------------LOCATIONS WITH TRAPS OR VAMPIRES PLACED--------------
	// Dracula should prioritise places with traps in them to stack traps.
	int numTraps = 0;
	PlaceId *TrapLocs = DvGetTrapLocations(dv, &numTraps);
	for (int i = 0; i < numTraps; i++) riskLevel[TrapLocs[i]] -= 1;

	// Vampire Location: Risk +1 (don't want hunters to trigger it too early!)
	riskLevel[DvGetVampireLocation(dv)] += 1;

	// -------------LOCATIONS CONNECTED TO THE SEA-------------------------
	Map m = MapNew();	
	for (int i = 0; i < NUM_REAL_PLACES; i++) {
		// These countries are very sea-dependent and so should 
		// not be taken if low on health
		if (health <= 30) {
			if (isCountry(England, i, SIZE_OF_ENGLAND)) riskLevel[i] += 1;
			if (isCountry(Spain, i, SIZE_OF_SPAIN)) riskLevel[i] += 1;
			if (isCountry(Italy, i, SIZE_OF_ITALY)) riskLevel[i] += 1;
		}

		// Sea locations have +2 Risk.
		if (placeIsSea(i)) {
			riskLevel[i] += 2;

			// Don't suicide at sea!
			if (health <= 20) riskLevel[i] += 10;
			if (health <= 10) riskLevel[i] += 20;
		}

		// Prefer to travel by road, don't lose health at sea.
		else if (isPortCity(i, PortCities)) {
			riskLevel[i] += 1;
			if (health <= 30) {
				for (int player = 0; player < 4; player++) {
					if (hunterLocs[player] == i) {
						riskLevel[i] += 25;
					}
				}
			}
		}
		// If there are not many connections in the city,
		// it is easy for Dracula to get cornered!
		ConnList list = MapGetConnections(m, i);
		ConnList curr = list->next;
		int count = 0;
		while (curr != NULL) {
			count++;
			curr = curr->next;
		}
		if (count <= 3) riskLevel[i] += 2;
	}
	MapFree(m);

	if (huntersInCountry(Italy, hunterLocs, SIZE_OF_ITALY) >= 2) {
		riskLevel[VARNA] += 3;
		riskLevel[SALONICA] += 3;
		riskLevel[ATHENS] += 3;
	}
	if (DvGetPlayerLocation(dv, PLAYER_DRACULA) == CASTLE_DRACULA) {
		riskLevel[GALATZ] -= 3;
	}

	/////////////////////////////////////////////////////////////////////////////
	// ---------------------COMPUTING LOWEST RISK MOVE------------------------ //
	/////////////////////////////////////////////////////////////////////////////

	// Head to drac if its safe.
	if (huntersNearCastle(hunterLocs) == 0) prioritiseCastleDrac(riskLevel, hunterLocs);
	if (huntersNearCastle(hunterLocs) == 1 && health >= 60) prioritiseCastleDrac(riskLevel, hunterLocs);
	if (huntersNearCastle(hunterLocs) == 4) {
		riskLevel[ZAGREB] -= 3;
		riskLevel[MUNICH] -= 3;
		riskLevel[VIENNA] -= 3;
		riskLevel[SARAJEVO] -= 3;
		riskLevel[BLACK_SEA] -= 3;
	}

	// FIND THE MOVES WITH THE MINIMUM RISK LEVEL
	int min = riskLevel[MoveToLocation(pastLocs, validMoves[numValidMoves - 1], &numPastLocs)];
	PlaceId *lowRiskMoves = malloc(sizeof(PlaceId) *numValidMoves);
	int lowRiskNum = 0;

	for (int i = 0; i < numValidMoves; i++) {
		// If the risk level of the location in ValidMoves[i] <= min
		if (riskLevel[MoveToLocation(pastLocs, validMoves[i], &numPastLocs)] <= min) {
			min = riskLevel[MoveToLocation(pastLocs, validMoves[i], &numPastLocs)];
			lowRiskMoves[lowRiskNum] = validMoves[i];
			lowRiskNum++;
		}
	}	

	// If there are no low risk moves pick a random valid move.
	if (lowRiskNum == 0) {
		int i = rand() % numValidMoves;
		strcpy(play, placeIdToAbbrev(validMoves[i]));
		registerBestPlay(play, "noice");
		return;
	}
	
	// If Drac is very healthy, be aggressive and attack hunters.
	// Otherwise stay away from them.
	PlaceId dracLoc = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	for (int player = 0; player < 4; player++) {
		if (hunterLocs[player] == dracLoc && health <= 40) {
			riskLevel[hunterLocs[player]] += 10;
			for (int i = 0; i < 4; i++) {
				riskLevel[hunterLocs[i]] += 10;
			}
		}

		if (DvGetHealth(dv, player) < 5 && health > 20) {
			riskLevel[hunterLocs[player]] = 0;
		}
	}

	// for (int i = 0; i < lowRiskNum; i++) {
	// 	printf("lowRiskMoves[%d] is %s with risk %d\n", i, placeIdToName(MoveToLocation(pastLocs, lowRiskMoves[i], &numPastLocs)), riskLevel[MoveToLocation(pastLocs, lowRiskMoves[i], &numPastLocs)]);
	// }

	PlaceId minimum = -1;
	for (int i = 0; i < lowRiskNum; i++) {
		// If the risk level of the location in ValidMoves[i] <= min
		if (riskLevel[MoveToLocation(pastLocs, lowRiskMoves[i], &numPastLocs)] <= min) {
			min = riskLevel[MoveToLocation(pastLocs, lowRiskMoves[i], &numPastLocs)];
			minimum = lowRiskMoves[i];
		}
	}	
	// If the minimum hasn't been found, pick the first lowRiskMove.
	if (minimum == -1) minimum = lowRiskMoves[0];
	strcpy(play, placeIdToAbbrev(minimum));
	registerBestPlay(play, "gg noob");
	return;
}

// Converts the move to location
PlaceId MoveToLocation(PlaceId *pastLocs, PlaceId location, int *numPastLocs) {
	if (location == HIDE) {
		location = pastLocs[*numPastLocs - 1];
		if (isDoubleBack(location)) 
		{
			int index = location - 102;
			location = pastLocs[*numPastLocs - 1 - index]; 
		}
	}
	else if (isDoubleBack(location)) 
	{	
		int index = location - 102;
		location = pastLocs[*numPastLocs - index]; 
		if (location == HIDE) location = pastLocs[*numPastLocs - index - 1]; 
	} 
	return location;
}

bool isPortCity(PlaceId i, PlaceId PortCities[]) {
	for (int k = 0; k < NUM_PORT_CITIES; k++) {
		if (i == PortCities[k]) {
			return true;
		}
	}
	
	return false;
}


// Gets current locations of each Hunter.
void getHunterLocs(DraculaView dv, PlaceId hunterLocs[]) {
	for (int player = 0; player < 4; player++) {
		hunterLocs[player] = DvGetPlayerLocation(dv, player);
	}
}

void prioritiseCastleDrac(int riskLevel[], PlaceId hunterLocs[]) {
	riskLevel[CASTLE_DRACULA] = -10;
	riskLevel[GALATZ] = -2;
	riskLevel[BUDAPEST] -= 1;
	riskLevel[KLAUSENBURG] -= 1;
	riskLevel[SZEGED] -= 1;
	riskLevel[BELGRADE] -= 1;
	riskLevel[BUCHAREST] -= 1;
	riskLevel[SOFIA] -= 1;
	riskLevel[CONSTANTA] -= 1;
	riskLevel[ZAGREB] -= 1;
	riskLevel[SARAJEVO] -= 0;
	riskLevel[VIENNA] -= 0;
	riskLevel[VARNA] -= 1;
	return;
}

int huntersNearCastle(PlaceId hunterLocs[]) {
	int count = 0;
	for (int player = 0; player < 4; player++) {
		if (hunterLocs[player] == CASTLE_DRACULA) {
			count++;
		}
		if (hunterLocs[player] == BUDAPEST) {
			count++;
		}
		if (hunterLocs[player] == KLAUSENBURG) {
			count++;
		}
		if (hunterLocs[player] == SZEGED) {
			count++;
		}
		if (hunterLocs[player] == BELGRADE) {
			count++;
		}
		if (hunterLocs[player] == BUCHAREST) {
			count++;
		}
		if (hunterLocs[player] == SOFIA) {
			count++;
		}
		if (hunterLocs[player] == CONSTANTA) {
			count++;
		}
		if (hunterLocs[player] == ZAGREB) {
			count++;
		}
		if (hunterLocs[player] == SARAJEVO) {
			count++;
		}
		if (hunterLocs[player] == VIENNA) {
			count++;
		}
		if (hunterLocs[player] == VARNA) {
			count++;
		}
	}
	return count;
}

int isDoubleBack(PlaceId location)
{
	return (location >= DOUBLE_BACK_1 && location <= DOUBLE_BACK_5);
}

bool isCountry (PlaceId country[], PlaceId location, int size) {
	for (int i = 0; i < size; i++) {
		if (location == country[i]) {
			return true;
		}
	}
	return false;
}

int huntersInCountry (PlaceId country[], PlaceId hunterLocs[], int size) {
	int count = 0;
	for (int player = 0; player < 4; player++) {
		if (isCountry(country, hunterLocs[player], size)) {
			count++;
		}
	}

	return count;
}