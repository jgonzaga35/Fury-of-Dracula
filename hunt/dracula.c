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
#define SIZE_OF_FRANCE 8
#define SIZE_OF_CENTRAL_EUROPE 12
#define SIZE_OF_EAST_EUROPE 12
#define SIZE_OF_WEST_SEAS 5
#define SIZE_OF_CENTRAL_SEAS 4

void prioritiseCastleDrac(int riskLevel[], PlaceId hunterLocs[]);
PlaceId MoveToLoc(PlaceId *pastLocs, PlaceId location, int *numPastLocs);
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
	PlaceId France[] = {TOULOUSE, CLERMONT_FERRAND, PARIS, MARSEILLES, LE_HAVRE, NANTES,
						GENEVA, STRASBOURG};
	PlaceId Italy[] = {GENOA, FLORENCE, ROME, NAPLES, BAY_OF_BISCAY, VENICE, MILAN};

	///////////////////////////////////////////////////////////////////
	// ----------------------STARTING ROUND------------------------- //
	///////////////////////////////////////////////////////////////////

	// Dracula has the most movement options in STRASBOURG.
	if (round == 0) {
		registerBestPlay("KL", "come at me bro");
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

	// Go to Castle Dracula if it is safe.
	PlaceId *pastLocs = DvGetLocationHistory(dv, &numPastLocs);  
	for (int i = 0; i < numValidMoves; i++) {
		// If any of the Valid Moves correspond to CASTLE_DRACULA:
		if (MoveToLoc(pastLocs, validMoves[i], &numPastLocs) == CASTLE_DRACULA) {	
			// If there are hunters at/around CASTLE_DRACULA
			if ((huntersNearCastle(hunterLocs) <= 2) || (huntersNearCastle(hunterLocs) <= 1)) {									
				registerBestPlay(strdup(placeIdToAbbrev(validMoves[i])), "oi, you want fight?");
				return;
			} 
		} 
	}

	if (pastLocs[numPastLocs - 1] == KLAUSENBURG) {
		registerBestPlay("CD", "noice");
		return;
	}
	if (pastLocs[numPastLocs - 1] == CASTLE_DRACULA) {
		registerBestPlay("GA", "noice");
		return;
	}
	if (pastLocs[numPastLocs - 1] == GALATZ) {
		registerBestPlay("CN", "noice");
		return;
	}
	if (pastLocs[numPastLocs - 1] == CONSTANTA) {
		registerBestPlay("BS", "noice");
		return;
	}
	if (pastLocs[numPastLocs - 1] == BLACK_SEA) {
		registerBestPlay("IO", "noice");
		return;
	}
	if (pastLocs[numPastLocs - 1] == IONIAN_SEA) {
		registerBestPlay("TS", "noice");
		return;
	}
	if (pastLocs[numPastLocs - 1] == TYRRHENIAN_SEA) {
		registerBestPlay("MS", "noice");
		return;
	}
	if (pastLocs[numPastLocs - 1] == ALICANTE) {
		if (huntersInCountry(Spain, hunterLocs, SIZE_OF_SPAIN)
		+ huntersInCountry(France, hunterLocs, SIZE_OF_FRANCE) >= 3) {
			registerBestPlay("MS", "n");
			return;
		}
		for (int player = 0; player < 4; player++) {
			if (hunterLocs[player] == MEDITERRANEAN_SEA) {
				registerBestPlay("MS", "n");
				return;
			}
		}
		registerBestPlay("SR", "n");
		return;
	} 
	if (pastLocs[numPastLocs - 1] == SARAGOSSA) {
		if ((huntersInCountry(Spain, hunterLocs, SIZE_OF_SPAIN) + huntersInCountry(France, hunterLocs, SIZE_OF_FRANCE)) >= 1) {
			registerBestPlay("BA", "n");
			return;
		}
		registerBestPlay("MA", "n");
		return;
	} 	
	if (pastLocs[numPastLocs - 1] ==BARCELONA) {
		registerBestPlay("MS", "n");
		return;
	} 
	if (pastLocs[numPastLocs - 1] == MADRID) {
		if ((huntersInCountry(Spain, hunterLocs, SIZE_OF_SPAIN) + huntersInCountry(France, hunterLocs, SIZE_OF_FRANCE)) >= 1) {
			registerBestPlay("SN", "n");
			return;
		}
		registerBestPlay("LS", "n");
		return;
	} 
	if (pastLocs[numPastLocs - 1] == SANTANDER) {
		registerBestPlay("BB", "n");
		return;
	} 
	if (pastLocs[numPastLocs - 1] ==BAY_OF_BISCAY) {
		registerBestPlay("AO", "n");
		return;
	} 
	if (pastLocs[numPastLocs - 1] == LISBON) {
		registerBestPlay("CA", "n");
		return;
	} 
	if (pastLocs[numPastLocs - 1] == CADIZ) {
		registerBestPlay("GR", "n");
		return;
	} 
	if (pastLocs[numPastLocs - 1] == GRANADA) {
		registerBestPlay("AL", "n");
		return;
	} 
	if (pastLocs[numPastLocs - 1] == MEDITERRANEAN_SEA) {
		if (huntersNearCastle(hunterLocs) >= 3) {
			registerBestPlay("AL", "n");
			return;
		}
		registerBestPlay("AO", "noice");
		return;
	}
	if (pastLocs[numPastLocs - 1] == ATLANTIC_OCEAN) {
		registerBestPlay("NS", "noice");
		return;
	}
	if (pastLocs[numPastLocs - 1] == NORTH_SEA) {
		registerBestPlay("HA", "noice");
		return;
	}
	if (pastLocs[numPastLocs - 1] == HAMBURG) {
		registerBestPlay("BR", "noice");
		return;
	}
	if (pastLocs[numPastLocs - 1] == BRUSSELS) {
		registerBestPlay("PR", "noice");
		return;
	}
	if (pastLocs[numPastLocs - 1] == PRAGUE) {
		registerBestPlay("VI", "noice");
		return;
	}
	if (pastLocs[numPastLocs - 1] == VIENNA) {
		registerBestPlay("ZA", "noice");
		return;
	}
	if (pastLocs[numPastLocs - 1] == ZAGREB) {
		registerBestPlay("SJ", "noice");
		return;
	}
	if (pastLocs[numPastLocs - 1] == SARAJEVO) {
		registerBestPlay("SO", "noice");
		return;
	}
	if (pastLocs[numPastLocs - 1] == SOFIA) {
		registerBestPlay("BC", "noice");
		return;
	}
	if (pastLocs[numPastLocs - 1] == BUCHAREST) {
		registerBestPlay("KL", "noice");
		return;
	}

	// for (int i = 0; i < numValidMoves; i++) {
	// 	printf("validMoves[%d] is %s with risk %d\n", i, placeIdToName(MoveToLoc(pastLocs, validMoves[i], &numPastLocs)), riskLevel[MoveToLoc(pastLocs, validMoves[i], &numPastLocs)]);
	// }

	////////////////////////////////////////////////////////////////////
	// --------------ASSIGNING RISK LEVELS TO EACH LOCATION---------- //
	////////////////////////////////////////////////////////////////////

	// -------------LOCATIONS REACHABLE BY HUNTERS-------------------
	for (int player = 0; player < 4; player++) {

		// Hunter's Current Location: +2 Risk
		riskLevel[hunterLocs[player]] += 2;

		// Locations reachable by road: +3 Risk
		PlaceId *riskyLocsRoad = DvWhereCanTheyGoByType(dv, player, true, false, false, &numRiskyLocs);
		
		// Dracula will take risks if he is healthy enough...
		for (int i = 0; i < numRiskyLocs; i++) riskLevel[riskyLocsRoad[i]] += 3;
		
		// Locations reachable by rail: +2 Risk
		PlaceId *riskyLocsRail = DvWhereCanTheyGoByType(dv, player, false, true, false, &numRiskyLocs);
		for (int i = 0; i < numRiskyLocs; i++) riskLevel[riskyLocsRail[i]] += 4;

		PlaceId *riskyLocsSea = DvWhereCanTheyGoByType(dv, player, false, false, true, &numRiskyLocs);
		for (int i = 0; i < numRiskyLocs; i++) {
			if (placeIsLand(riskyLocsSea[i]))
				riskLevel[riskyLocsSea[i]] += 3;
			// if (placeIsSea(riskyLocsSea[i]))
			// 	riskLevel[riskyLocsSea[i]] -= 1;
		}
	}
	riskLevel[LIVERPOOL] += 1;
	// --------------LOCATIONS WITH TRAPS OR VAMPIRES PLACED--------------
	// Dracula should prioritise places with traps in them to stack traps.
	int numTraps = 0;
	PlaceId *TrapLocs = DvGetTrapLocations(dv, &numTraps);
	for (int i = 0; i < numTraps; i++) riskLevel[TrapLocs[i]] -= 1;

	// Vampire Location: Risk +1 (don't want hunters to trigger it too early!)
	riskLevel[DvGetVampireLocation(dv)] += 1;
				if (isCountry(England, DvGetPlayerLocation(dv, PLAYER_DRACULA), SIZE_OF_ENGLAND)
					||  isCountry(Italy, DvGetPlayerLocation(dv, PLAYER_DRACULA), SIZE_OF_ITALY))
				riskLevel[pastLocs[numPastLocs - 1]] += 50;
				riskLevel[pastLocs[numPastLocs - 2]] += 40;
				riskLevel[pastLocs[numPastLocs - 3]] += 40;

	// -------------LOCATIONS CONNECTED TO THE SEA-------------------------
	Map m = MapNew();	
	for (int i = 0; i < NUM_REAL_PLACES; i++) {
		// These countries are very sea-dependent and so should 
		// not be taken if low on health
		if (health <= 30) {
			if (isCountry(England, i, SIZE_OF_ENGLAND)) riskLevel[i] += 30;
			if (isCountry(Spain, i, SIZE_OF_SPAIN)) riskLevel[i] += 1;
			if (isCountry(Italy, i, SIZE_OF_ITALY)) riskLevel[i] += 1;
		}

		// Sea locations have +2 Risk.
		if (placeIsSea(i)) {
			riskLevel[i] += 2;
			if (huntersNearCastle(hunterLocs) >= 3) {

				if (health > 48) {
					riskLevel[i] -= 29;
				}
				riskLevel[pastLocs[numPastLocs - 1]] += 50;
				riskLevel[pastLocs[numPastLocs - 2]] += 40;
				riskLevel[pastLocs[numPastLocs - 3]] += 40;
				// riskLevel[SALONICA] += 10;
			}
			// riskLevel[VARNA] += 10;
			// riskLevel[ATHENS] += 10;
			// riskLevel[VALONA] += 10;
			// if (DvGetPlayerLocation(dv, PLAYER_DRACULA) == MANCHESTER)
			// 	riskLevel[LIVERPOOL] += 5;

			// Don't suicide at sea!
			if (health <= 20) riskLevel[i] += 10;
			if (health <= 10) riskLevel[i] += 20;
		}

		// Prefer to travel by road, don't lose health at sea.
		else if (health < 50 && isPortCity(i, PortCities)) {
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
		// if (count <= 3) riskLevel[i] += 2;
	}

	// if (huntersInCountry(Italy, hunterLocs, SIZE_OF_ITALY) >= 2) {
	// 	riskLevel[VARNA] += 3;
	// 	riskLevel[SALONICA] += 3;
	// 	riskLevel[ATHENS] += 3;
	// }
	// if (DvGetPlayerLocation(dv, PLAYER_DRACULA) == CASTLE_DRACULA) {
	// 	riskLevel[GALATZ] -= 3;
	// }

	// If hunters are approaching, don't repeat trail
	int average = 0;
	for (int i = 0; i < numValidMoves; i++) {
		average += riskLevel[MoveToLoc(pastLocs, validMoves[i], &numPastLocs)];
	}
	average = average / numValidMoves;
	if (average > 9) {
		riskLevel[pastLocs[numPastLocs - 1]] += 6;
		riskLevel[pastLocs[numPastLocs - 2]] += 6;
		riskLevel[pastLocs[numPastLocs - 3]] += 6;
		riskLevel[pastLocs[numPastLocs - 4]] += 6;
		riskLevel[pastLocs[numPastLocs - 5]] += 6;
		for (int i = 1; i < 5; i++) {
			ConnList list = MapGetConnections(m, pastLocs[numPastLocs - i]);
			ConnList curr = list->next;
			int count = 0;
			while (curr != NULL) {
				for (int player = 0; player < 4; player++) {
					if (curr->p == hunterLocs[player]) {
						riskLevel[curr->p] += 1;
						riskLevel[pastLocs[numPastLocs - i]] += 2; 
					}
				}
				riskLevel[curr->p] += 1;
				curr = curr->next;
			}
		}
	}
	

	MapFree(m);
	/////////////////////////////////////////////////////////////////////////////
	// ---------------------COMPUTING LOWEST RISK MOVE------------------------ //
	/////////////////////////////////////////////////////////////////////////////

	// Head to drac if its safe.
	if (huntersNearCastle(hunterLocs) <= 1) prioritiseCastleDrac(riskLevel, hunterLocs);
	if (huntersNearCastle(hunterLocs) == 1 && health >= 60) prioritiseCastleDrac(riskLevel, hunterLocs);
	if (huntersNearCastle(hunterLocs) == 4) {

	riskLevel[KLAUSENBURG] += 4;

}
	// FIND THE MOVES WITH THE MINIMUM RISK LEVEL
	int min = riskLevel[MoveToLoc(pastLocs, validMoves[numValidMoves - 1], &numPastLocs)];
	PlaceId *lowRiskMoves = malloc(sizeof(PlaceId) *numValidMoves);
	int lowRiskNum = 0;

	for (int i = 0; i < numValidMoves; i++) {
		// If the risk level of the location in ValidMoves[i] <= min
		if (riskLevel[MoveToLoc(pastLocs, validMoves[i], &numPastLocs)] <= min) {
			min = riskLevel[MoveToLoc(pastLocs, validMoves[i], &numPastLocs)];
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
	// 	printf("lowRiskMoves[%d] is %s with risk %d\n", i, placeIdToName(MoveToLoc(pastLocs, lowRiskMoves[i], &numPastLocs)), riskLevel[MoveToLoc(pastLocs, lowRiskMoves[i], &numPastLocs)]);
	// }
	// printf("risk of BLACK SEA is %d\n", riskLevel[BLACK_SEA]);

	PlaceId minimum = -1;
	for (int i = 0; i < lowRiskNum; i++) {
		// If the risk level of the location in ValidMoves[i] <= min
		if (riskLevel[MoveToLoc(pastLocs, lowRiskMoves[i], &numPastLocs)] <= min) {
			min = riskLevel[MoveToLoc(pastLocs, lowRiskMoves[i], &numPastLocs)];
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
PlaceId MoveToLoc(PlaceId *pastLocs, PlaceId location, int *numPastLocs) {
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
	riskLevel[ZAGREB] -= 3;
	riskLevel[SARAJEVO] -= 0;
	riskLevel[VIENNA] -= 0;
	riskLevel[VARNA] -= 1;
	riskLevel[PRAGUE] -= 2;
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
		if (hunterLocs[player] == SALONICA) {
			count++;
		}
		if (hunterLocs[player] == BLACK_SEA) {
			count++;
		}
		if (hunterLocs[player] == IONIAN_SEA) {
			count++;
		}
		if (hunterLocs[player] == MUNICH) {
			count++;
		}
		if (hunterLocs[player] == VENICE) {
			count++;
		}
		if (hunterLocs[player] == PRAGUE) {
			count++;
		}
		if (hunterLocs[player] == GALATZ) {
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