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
bool isPortCity(PlaceId i);
void getHunterLocs(DraculaView dv, PlaceId hunterLocs[]);
bool shouldIGoToCastle(PlaceId hunterLocs[]);
int isDoubleBack(PlaceId location);

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

	// Where is the best city to start? Unsure...
	if (round == 0) {
		registerBestPlay("PA", "come at me bro");
		return;
	}

	// If Dracula has no valid moves, use TELEPORT.
	PlaceId *validMoves = DvGetValidMoves(dv, &numValidMoves);
	if (numValidMoves == 0 || validMoves == NULL) {
		registerBestPlay("TP", "I love COMP2521");
		return;
	}

	// Go to Castle Dracula if possible - Dracula wants to gain 10 BP.
	// Even if a hunter is there, it will be an even exchange. 
	bool hunterAtCastle = false;
	PlaceId *pastLocs = DvGetLocationHistory(dv, &numPastLocs);  
	// for (int i = 0; i < numValidMoves; i++) {
	// 	printf("validMoves[%d] is %s with risk %d\n", i, placeIdToName(MoveToLocation(pastLocs, validMoves[i], &numPastLocs)), riskLevel[MoveToLocation(pastLocs, validMoves[i], &numPastLocs)]);
	// }
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

	// Assign risk levels to each place.
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

	// Dracula should prioritise places with traps in them to stack traps.
	int numTraps = 0;
	PlaceId *TrapLocs = DvGetTrapLocations(dv, &numTraps);
	for (int i = 0; i < numTraps; i++) riskLevel[TrapLocs[i]] -= 1;

	// Vampire Location: Risk +1 (don't want hunters to trigger it too early!)
	riskLevel[DvGetVampireLocation(dv)] += 1;

	// -----CHOKEPOINTS------
	// I think these cities are risky based on 
	// how easy it is for Dracula to get cornered/do a dumb move.
	riskLevel[LISBON] = 5;
	riskLevel[HAMBURG] += 2;
	riskLevel[FLORENCE] -= 1;
	riskLevel[STRASBOURG] -= 3;
	riskLevel[NUREMBURG] -= 3;
	Map m = MapNew();
	
	// Assign a risk level to each location 
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
			if (health <= 20) {
				riskLevel[i] += 10;
			}
			// Don't suicide at sea!
			if (health <= 10) riskLevel[i] += 30;
		}

		// Prefer to travel by road, don't lose health at sea.
		else if (isPortCity(i)) riskLevel[i] += 1;

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

	// Head to drac if its safe.
	if (shouldIGoToCastle(hunterLocs)) {
		prioritiseCastleDrac(riskLevel, hunterLocs);
	}
	// Note: Once Dracula's health is between 20 and 30, try to return to CD.
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
	for (int i = 0; i < lowRiskNum; i++) {
		printf("lowRiskMoves[%d] is %s with risk %d\n", i, placeIdToName(MoveToLocation(pastLocs, lowRiskMoves[i], &numPastLocs)), riskLevel[MoveToLocation(pastLocs, lowRiskMoves[i], &numPastLocs)]);
	}
	// If there are no low risk moves pick a random valid move.
	if (lowRiskNum == 0) {
		int i = rand() % numValidMoves;
		strcpy(play, placeIdToAbbrev(validMoves[i]));
		registerBestPlay(play, "registering best play");
		return;
	}
	
	// If drac is currently at the same location as a player,
	// do not go to any of the hunter current locations.
	PlaceId dracLoc = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	for (int player = 0; player < 4; player++) {
		if (hunterLocs[player] == dracLoc) {
			riskLevel[hunterLocs[player]] += 10;
			for (int i = 0; i < 4; i++) {
				riskLevel[hunterLocs[i]] += 10;
			}
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

bool isPortCity(PlaceId i) {
	switch (i) {
		case BARI:
			i = -2;
			break;
		case ALICANTE:
			i = -2;
			break;
		case AMSTERDAM:
			i = -2;
			break;
		case ATHENS:
			i = -2;
			break;
		case CADIZ:
			i = -2;
			break;
		case GALWAY:
			i = -2;
			break;
		case LISBON:
			i = -2;
			break;
		case BARCELONA:
			i = -2;
			break;
		case BORDEAUX:
			i = -2;
			break;
		case NANTES:
			i = -2;
			break;
		case SANTANDER:
			i = -2;
			break;
		case CONSTANTA:
			i = -2;
			break;
		case VARNA:
			i = -2;
			break;
		case CAGLIARI:
			i = -2;
			break;
		case DUBLIN:
			i = -2;
			break;
		case EDINBURGH:
			i = -2;
			break;
		case LE_HAVRE:
			i = -2;
			break;
		case LONDON:
			i = -2;
			break;
		case PLYMOUTH:
			i = -2;
			break;
		case GENOA:
			i = -2;
			break;
		case HAMBURG:
			i = -2;
			break;
		case SALONICA:
			i = -2;
			break;
		case VALONA:
			i = -2;
			break;
		case LIVERPOOL:
			i = -2;
			break;
		case SWANSEA:
			i = -2;
			break;
		case MARSEILLES:
			i = -2;
			break;
		case NAPLES:
			i = -2;
			break;
		case ROME:
			i = -2;
			break;
		default:
			i = -1;
	}

	if (i == -1) {
		return false;
	} else if (i == -2) {
		return true;
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
	if (shouldIGoToCastle(hunterLocs)) {
		riskLevel[CASTLE_DRACULA] -= 2;
		riskLevel[BUDAPEST] -= 2;
		riskLevel[KLAUSENBURG] -= 2;
		riskLevel[SZEGED] -= 2;
		riskLevel[BELGRADE] -= 2;
		riskLevel[BUCHAREST] -= 2;
		riskLevel[SOFIA] -= 2;
		riskLevel[CONSTANTA] -= 2;
		riskLevel[ZAGREB] -= 2;
		riskLevel[SARAJEVO] -= 2;
		riskLevel[VIENNA] -= 2;
	}
	return;
}

bool shouldIGoToCastle(PlaceId hunterLocs[]) {
	for (int player = 0; player < 4; player++) {
		if (hunterLocs[player] == CASTLE_DRACULA) {
			return false; 
		}
		if (hunterLocs[player] == BUDAPEST) {
			return false; 
		}
		if (hunterLocs[player] == KLAUSENBURG) {
			return false; 
		}
		if (hunterLocs[player] == SZEGED) {
			return false; 
		}
		if (hunterLocs[player] == BELGRADE) {
			return false; 
		}
		if (hunterLocs[player] == BUCHAREST) {
			return false; 
		}
		if (hunterLocs[player] == SOFIA) {
			return false; 
		}
		if (hunterLocs[player] == CONSTANTA) {
			return false; 
		}
		if (hunterLocs[player] == ZAGREB) {
			return false; 
		}
		if (hunterLocs[player] == SARAJEVO) {
			return false; 
		}
	}
	return true;
}

int isDoubleBack(PlaceId location)
{
	return (location >= DOUBLE_BACK_1 && location <= DOUBLE_BACK_5);
}
