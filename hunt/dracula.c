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
#define SIZE_OF_CENTRAL_SEAS 1
#define SIZE_OF_SEAS 10

static PlaceId seas[] = {NORTH_SEA, ENGLISH_CHANNEL, IRISH_SEA, BAY_OF_BISCAY, ATLANTIC_OCEAN,
						MEDITERRANEAN_SEA, IONIAN_SEA, ADRIATIC_SEA, TYRRHENIAN_SEA, BLACK_SEA};
static PlaceId PortCities[] = {BARI, ALICANTE, AMSTERDAM, ATHENS, CADIZ, GALWAY,
						LISBON, BARCELONA, BORDEAUX, NANTES, SANTANDER,
						CONSTANTA, VARNA, CAGLIARI, DUBLIN, EDINBURGH, 
						LE_HAVRE, LONDON, PLYMOUTH, GENOA, HAMBURG,
						SALONICA, VALONA, LIVERPOOL, SWANSEA, MARSEILLES,
						NAPLES, ROME};
static PlaceId England[] = {EDINBURGH, LONDON, MANCHESTER, LIVERPOOL, SWANSEA, PLYMOUTH};
static PlaceId Ireland[] = {GALWAY, DUBLIN, IRISH_SEA};
static PlaceId Portugal[] = {LISBON};
static PlaceId WesternSeas[] = {NORTH_SEA, ENGLISH_CHANNEL, IRISH_SEA, BAY_OF_BISCAY, ATLANTIC_OCEAN};
static PlaceId CentralSeas[] = {MEDITERRANEAN_SEA};
static PlaceId EastEurope[] = {VIENNA, SARAJEVO, ZAGREB, BUDAPEST, KLAUSENBURG, CASTLE_DRACULA,
						GALATZ, CONSTANTA, BUCHAREST, SOFIA, SZEGED, VARNA};
static PlaceId CentralEurope[] = {STRASBOURG, BRUSSELS, COLOGNE, AMSTERDAM, HAMBURG, LEIPZIG,
 							PRAGUE, NUREMBURG, ZURICH, MUNICH, FRANKFURT, BERLIN};
static PlaceId Spain[] = {MADRID, GRANADA, ALICANTE, SARAGOSSA, BARCELONA, SANTANDER, LISBON};
static PlaceId France[] = {TOULOUSE, CLERMONT_FERRAND, MARSEILLES, LE_HAVRE, NANTES,
					GENEVA, STRASBOURG, BORDEAUX};
static PlaceId Italy[] = {GENOA, FLORENCE, ROME, NAPLES, BARI, VENICE, MILAN};

bool shouldIGoToCastleDrac(PlaceId *pastLocs, PlaceId *validMoves, int numPastLocs, int numValidMoves, int hunterNum, PlaceId hunterLocs[]);
bool isValid (char *play, PlaceId *validMoves, int numValidMoves);
void prioritiseCastleDrac(int riskLevel[], PlaceId hunterLocs[]);
PlaceId MoveToLoc(PlaceId *pastLocs, PlaceId location, int *numPastLocs);
bool isPortCity(PlaceId i, PlaceId PortCities[]);
void getHunterLocs(DraculaView dv, PlaceId hunterLocs[]);
int huntersNearCD(PlaceId hunterLocs[]);
int huntersInCountry (PlaceId country[], PlaceId hunterLocs[], int size);
int isDoubleBack(PlaceId location);
bool isCountry (PlaceId country[], PlaceId location, int size);
bool LoopStrat(PlaceId *pastLocs, PlaceId *validMoves, int numValidMoves, int numPastLocs, PlaceId hunterLocs[]);
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

	///////////////////////////////////////////////////////////////////
	// ----------------------STARTING ROUND------------------------- //
	///////////////////////////////////////////////////////////////////

	// Dracula has the most movement options in STRASBOURG.
	if (round == 0) {
		if (huntersNearCD(hunterLocs) <= 1) {
			registerBestPlay("KL", "come at me bro"); return;
		} else {
			registerBestPlay("ST", "come at me bro"); return;
		}
	}
	// If Dracula has no valid moves, use TELEPORT.
	PlaceId *validMoves = DvGetValidMoves(dv, &numValidMoves);
	PlaceId *pastLocs = DvGetLocationHistory(dv, &numPastLocs);  
	if (validMoves == NULL) { registerBestPlay("TP", "liam neesons"); return;}

	// Go to Castle Dracula if it is safe.
	if (shouldIGoToCastleDrac(pastLocs, validMoves, numPastLocs, numValidMoves, 1, hunterLocs)) return;

	// Debugging...
	bool canPrint = true;
	if (canPrint) {
		for (int i = 0; i < numValidMoves; i++) {
			printf("validMoves[%d] is %s with risk %d\n", i, placeIdToName(MoveToLoc(pastLocs, validMoves[i], &numPastLocs)), riskLevel[MoveToLoc(pastLocs, validMoves[i], &numPastLocs)]);
		}
	}

	////////////////////////////////////////////////////////////////////
	// --------------ASSIGNING RISK LEVELS TO EACH LOCATION---------- //
	////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////
	// -------------LOCATIONS REACHABLE BY HUNTERS------------------- //
	////////////////////////////////////////////////////////////////////

	int hunterRisk[4];
	for (int player = PLAYER_LORD_GODALMING; player < PLAYER_DRACULA; player++) {
		if (health >= 60) hunterRisk[player] = DvGetHealth(dv, player) - 6; 
		else hunterRisk[player] = DvGetHealth(dv, player) - 4;
		if (health < 10) { 
			hunterRisk[player] = 16;
		}
	}

	Map m = MapNew();	
	for (int player = 0; player < 4; player++) {
		// Hunter's current location
		if (placeIsLand(hunterLocs[player])) {
			if (health >= 40) riskLevel[hunterLocs[player]] += hunterRisk[player];
			else riskLevel[hunterLocs[player]] += 9;
		}
		// Locations reachable by road
		PlaceId *riskyLocsRoad = DvWhereCanTheyGoByType(dv, player, true, false, false, &numRiskyLocs);
		for (int i = 0; i < numRiskyLocs; i++) if (placeIsLand(riskyLocsRoad[i])) riskLevel[riskyLocsRoad[i]] += hunterRisk[player];
		
		// Locations reachable by rail
		PlaceId *riskyLocsRail = DvWhereCanTheyGoByType(dv, player, false, true, false, &numRiskyLocs);
		for (int i = 0; i < numRiskyLocs; i++) if (placeIsLand(riskyLocsRail[i])) riskLevel[riskyLocsRail[i]] += hunterRisk[player] + 1;

		// Locations reachable by sea
		PlaceId *riskyLocsSea = DvWhereCanTheyGoByType(dv, player, false, false, true, &numRiskyLocs);
		for (int i = 0; i < numRiskyLocs; i++) if (placeIsLand(riskyLocsSea[i])) riskLevel[riskyLocsSea[i]] += hunterRisk[player];

		// Risky Locs in general
		PlaceId *riskyLocs = DvWhereCanTheyGo(dv, player, &numRiskyLocs);
		int numMyLocs = 0;
		PlaceId *myLocs = DvWhereCanIGoByType(dv, true, false, &numMyLocs);
		for (int i = 0; i < numRiskyLocs ; i++) {
			ConnList list = MapGetConnections(m, riskyLocs[i]);
			for (int j = 0; j < numMyLocs; j++) {
				ConnList myList = MapGetConnections(m, myLocs[j]);
				// If any of the reachable locations from the hunter's reachable locations
				// are in mine.
				ConnList curr = myList->next;
				for (ConnList curr = list->next; curr != NULL; curr = curr->next) {
					// printf("curr is %s\n", placeIdToName(curr->p));
					if (placeIsLand(curr->p)) {
						for (ConnList curr2 = myList->next; curr2 != NULL; curr2 = curr2->next) {
							if (curr->p == curr2->p) {
								riskLevel[curr2->p] += hunterRisk[player];
							}
							
						}
					}
				}
			}
		}
	}
	MapFree(m);
	/////////////////////////////////////////////////////////////////////////
	// --------------LOCATIONS WITH TRAPS OR VAMPIRES PLACED-------------- //
	/////////////////////////////////////////////////////////////////////////

	// Avoid vampires
	riskLevel[DvGetVampireLocation(dv)] += 1;

	// Dracula should prioritise places with traps in them to stack traps.
	int numTraps = 0;
	PlaceId *TrapLocs = DvGetTrapLocations(dv, &numTraps);
	for (int i = 0; i < numTraps; i++) {
		bool isDangerous = false;
		for (int player = 0; player < 4; player++) {
			if (TrapLocs[i] == hunterLocs[player]) {
				riskLevel[TrapLocs[i]] += 1;
				isDangerous = true;
			}
		} 
		if (!isDangerous) riskLevel[TrapLocs[i]] -= 1;
	}

	//////////////////////////////////////////////////////////////////////////
	// -------------LOCATIONS CONNECTED TO THE SEA------------------------- //
	//////////////////////////////////////////////////////////////////////////

	if (health <= 20) for (int i = 0; i < SIZE_OF_SEAS; i++) riskLevel[seas[i]] += 5;

	for (int i = 0; i < 4; i++) {
		if (placeIsSea(hunterLocs[i])) {
			riskLevel[PortCities[i]] += 2;
		}
	}

	// Don't go to sea if low on health.
	if (health <= 10) {
		for (int i = 0; i < SIZE_OF_SEAS; i++) riskLevel[seas[i]] += 10;
		for (int i = 0; i < NUM_PORT_CITIES; i++) riskLevel[PortCities[i]] += 6;
	}

	// Remain at sea if healthy.
	if (health >= 30) {	
		for (int i = 0; i < SIZE_OF_SEAS; i++) riskLevel[seas[i]] += 8;
		for (int i = 0; i < NUM_PORT_CITIES; i++) riskLevel[PortCities[i]] += 8;
	}

	// Flee to sea if in danger.
	if (huntersNearCD(hunterLocs) >= 3) {
		for (int i = 0; i < NUM_PORT_CITIES; i++) riskLevel[PortCities[i]] -= 6;
		if (health > 6) for (int i = 0; i < SIZE_OF_SEAS; i++) riskLevel[seas[i]] -= 3;
	}

	// If hunters are approaching, don't repeat trail
	for (int i = 1; i < 6 && i < numPastLocs; i++) {
		for (int player = 0; player < 4; player++) {
			if (hunterLocs[player] == pastLocs[numPastLocs - i]) {
				if (DvGetHealth(dv, player) > 4) riskLevel[pastLocs[numPastLocs - i]] += 15;
			}
		}
	}
	
	/////////////////////////////////////////////////////////////////////////////
	// ---------------------COMPUTING LOWEST RISK MOVE------------------------ //
	/////////////////////////////////////////////////////////////////////////////
	if (DvGetPlayerLocation(dv, PLAYER_DRACULA) == TYRRHENIAN_SEA) {
		for (int i = 0; i < SIZE_OF_ITALY; i++) {
			riskLevel[Italy[i]] += 102;
		}
	}

	for (int player = 0; player < 4; player++) {
		if ((DvGetPlayerLocation(dv, PLAYER_DRACULA) == hunterLocs[player])) {
			int number = 0;
			PlaceId *whereCanIGo = DvWhereCanIGo(dv, &number);
			for (int i = 0; i < number; i++) {
				if (isPortCity(whereCanIGo[i], PortCities)) {
					riskLevel[whereCanIGo[i]] -= 10;
				}
				if (placeIsSea(whereCanIGo[i])) {
					riskLevel[whereCanIGo[i]] -= 15;
				}
			}
		}
	}

	if (placeIsSea(DvGetPlayerLocation(dv, PLAYER_DRACULA))) {
		for (int i = 1; i < 6 && i < numPastLocs; i++) riskLevel[pastLocs[numPastLocs - i]] += 15;
	}
	printf("risk of black sea is %d\n", riskLevel[BLACK_SEA]);

	// Should I Loop around the MAP???
	if (health >= 14) if (LoopStrat(pastLocs, validMoves, numValidMoves, numPastLocs, hunterLocs)) return;
	
	// Head to drac if its safe.
	riskLevel[VALONA] = 20;
	riskLevel[ATHENS] = 20;
	riskLevel[SALONICA] = 20;
	if (huntersNearCD(hunterLocs) <= 1) prioritiseCastleDrac(riskLevel, hunterLocs);
	if (huntersNearCD(hunterLocs) <= 2 && health >= 60) prioritiseCastleDrac(riskLevel, hunterLocs);

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
		strcpy(play, placeIdToAbbrev(validMoves[0]));
		registerBestPlay(play, "Random");
		return;
	}
	
	// If Drac is very healthy, be aggressive and attack hunters.
	// Otherwise stay away from them.
	// PlaceId dracLoc = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	// for (int player = 0; player < 4; player++) {
	// 	if (hunterLocs[player] == dracLoc && health <= 40) {
	// 		for (int i = 0; i < 4; i++) {
	// 			if (placeIsLand(riskLevel[hunterLocs[i]])) riskLevel[hunterLocs[i]] += 10;
	// 		}
	// 	}
	// 	if (DvGetHealth(dv, player) < 5 && health > 20) {
	// 		riskLevel[hunterLocs[player]] = 0;
	// 	}
	// }
	// for (int i = 0; i < lowRiskNum; i++) {
	// 	printf("lowRiskMoves[%d] is %s with risk %d\n", i, placeIdToName(MoveToLoc(pastLocs, lowRiskMoves[i], &numPastLocs)), riskLevel[MoveToLoc(pastLocs, lowRiskMoves[i], &numPastLocs)]);
	// }
	// printf("risk of BLACK SEA is %d\n", riskLevel[MEDITERRANEAN_SEA]);

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

int huntersNearCD(PlaceId hunterLocs[]) {
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
		if (hunterLocs[player] == TYRRHENIAN_SEA) {
			count++;
		}
		if (hunterLocs[player] == ATHENS) {
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

bool isValid (char *play, PlaceId *validMoves, int numValidMoves) {
	for (int i = 0; i < numValidMoves; i++) {
		if (strstr(placeIdToAbbrev(validMoves[i]), play)) {
			return true;
		}
	}

	return false;
}

bool LoopStrat(PlaceId *pastLocs, PlaceId *validMoves, int numValidMoves, int numPastLocs, PlaceId hunterLocs[]) {
	printf("HELLOsasa\n");
	if (pastLocs[numPastLocs - 1] == KLAUSENBURG) {
		if (!isValid("CD", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("CD", "noice");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == CASTLE_DRACULA) {
		if (!isValid("GA", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("GA", "noice");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == GALATZ) {
		if (!isValid("CN", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("CN", "noice");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == CONSTANTA) {
		if (!isValid("BS", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("BS", "noice");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == BLACK_SEA) {
		if (!isValid("IO", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("IO", "noice");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == IONIAN_SEA) {
		if (!isValid("TS", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("TS", "noice");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == TYRRHENIAN_SEA) {
		if (!isValid("MS", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("MS", "noice");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == ALICANTE) {
		for (int player = 0; player < 4; player++) {
			if (hunterLocs[player] == SARAGOSSA) {
				if (isValid("MS", validMoves, numValidMoves)) {
				registerBestPlay("MS", "n");
				return true;
			}
			} 
		}
		if (huntersInCountry(Spain, hunterLocs, SIZE_OF_SPAIN)
		+ huntersInCountry(France, hunterLocs, SIZE_OF_FRANCE) >= 2) {
			if (isValid("MS", validMoves, numValidMoves)) {
				registerBestPlay("MS", "n");
				return true;
			}
		}
		for (int player = 0; player < 4; player++) {
			if (hunterLocs[player] == MEDITERRANEAN_SEA) {
				if (!isValid("MS", validMoves, numValidMoves)) {

				} else {
					registerBestPlay("MS", "n");
					return true;
				}
			}
		}
		if (!isValid("SR", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("SR", "n");
			return true;
		}
	} 
	if (pastLocs[numPastLocs - 1] == SARAGOSSA) {
		if ((huntersInCountry(Spain, hunterLocs, SIZE_OF_SPAIN) + huntersInCountry(France, hunterLocs, SIZE_OF_FRANCE)) > 1) {
			if (!isValid("BA", validMoves, numValidMoves)) {

			} else {
				registerBestPlay("BA", "n");
				return true;
			}
		}
		if (!isValid("MA", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("MA", "n");
			return true;
		}
	} 	
	if (pastLocs[numPastLocs - 1] ==BARCELONA) {
		if (!isValid("MS", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("MS", "n");
			return true;
		}
	} 
	if (pastLocs[numPastLocs - 1] == MADRID) {
		if ((huntersInCountry(Spain, hunterLocs, SIZE_OF_SPAIN) + huntersInCountry(France, hunterLocs, SIZE_OF_FRANCE)) >= 1) {
			if (!isValid("SN", validMoves, numValidMoves)) {

			} else {
				registerBestPlay("SN", "n");
				return true;
			}
		}
		if (!isValid("LS", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("LS", "n");
			return true;
		}
	} 
	if (pastLocs[numPastLocs - 1] == SANTANDER) {
		if (huntersInCountry(Spain, hunterLocs, SIZE_OF_SPAIN) + huntersInCountry(France, hunterLocs, SIZE_OF_FRANCE) <= 1) {
			if (isValid("LS", validMoves, numValidMoves)) {
				registerBestPlay("LS", "he");
				return true;
			}
		}
		if (!isValid("BB", validMoves, numValidMoves)) {
		} else {
			registerBestPlay("BB", "n");
			return true;
		}
	} 
	if (pastLocs[numPastLocs - 1] ==BAY_OF_BISCAY) {
		if (!isValid("AO", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("AO", "n");
			return true;
		}
	} 
	if (pastLocs[numPastLocs - 1] == LISBON) {
		if ((huntersInCountry(Spain, hunterLocs, SIZE_OF_SPAIN) + huntersInCountry(France, hunterLocs, SIZE_OF_FRANCE) + huntersInCountry(CentralSeas, hunterLocs, SIZE_OF_CENTRAL_SEAS)) > 1) {
			if (!isValid("AO", validMoves, numValidMoves)) {

			} else {
				registerBestPlay("AO", "n");
				return true;
			}
		}
		if (!isValid("CA", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("CA", "n");
			return true;
		}
	} 
	if (pastLocs[numPastLocs - 1] == CADIZ) {
		if (!isValid("GR", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("GR", "n");
			return true;
		}
	} 
	if (pastLocs[numPastLocs - 1] == GRANADA) {
		if (!isValid("AL", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("AL", "n");
			return true;
		}
	} 
	if (pastLocs[numPastLocs - 1] == MEDITERRANEAN_SEA) {
		if (huntersNearCD(hunterLocs) >= 3 || huntersInCountry(France, hunterLocs, SIZE_OF_FRANCE) < 1) {
			if (isValid("AL", validMoves, numValidMoves)) {
				registerBestPlay("AL", "n");
				return true;
			}
		}
		if (isValid("AO", validMoves, numValidMoves)) {
			registerBestPlay("AO", "noice");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == ATLANTIC_OCEAN) {
		if (huntersInCountry(CentralEurope, hunterLocs, SIZE_OF_CENTRAL_EUROPE) 
			+ huntersInCountry(Spain, hunterLocs, SIZE_OF_SPAIN) >= 2) {
			if (isValid("MS", validMoves, numValidMoves)) {
				registerBestPlay("MS", "noice");
				return true;
			}
		}
		if (isValid("NS", validMoves, numValidMoves)) {
			registerBestPlay("NS", "noice");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == NORTH_SEA) {
		if (!isValid("HA", validMoves, numValidMoves)) {

		} else {
			for (int player = 0; player < 4; player++) {
				if (hunterLocs[player] == HAMBURG) {
					registerBestPlay("EC", "noice");
					return true;
				}
			}
			registerBestPlay("HA", "noice");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == HAMBURG) {
		if (!isValid("BR", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("BR", "noice");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == BRUSSELS) {
		if (!isValid("PR", validMoves, numValidMoves)) {

		} else {
			if (huntersNearCD(hunterLocs) >= 2) {
				registerBestPlay("LI", "noice");
				return true;
			}
			registerBestPlay("PR", "noice");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == ENGLISH_CHANNEL) {
		if (!isValid("AO", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("AO", "noice");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == PRAGUE) {
		if (isValid("VI", validMoves, numValidMoves)) {
			if (huntersNearCD(hunterLocs) >= 2) {
				return false;
			}
			registerBestPlay("VI", "noice");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == VIENNA) {
		if (huntersNearCD(hunterLocs) >= 2) {
			return false;
		}
		bool canGo = true;
		for (int player = 0; player < 4; player++) {
			if (hunterLocs[player] == BUDAPEST) {
				canGo = false;
			}
			if (hunterLocs[player] == SZEGED) {
				canGo = false;
			}
		}
		if (canGo) {
			if (isValid("BD", validMoves, numValidMoves)) {
				registerBestPlay("BD", "teehee");
				return true;
			}
		}
		if (!isValid("ZA", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("ZA", "noice");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == ZAGREB) {
		if (!isValid("SJ", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("SJ", "noice");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == SARAJEVO) {
		if (!isValid("SO", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("SO", "noice");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == SOFIA) {
		if (!isValid("BC", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("BC", "noice");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == BUDAPEST) {
		if (isValid("KL", validMoves, numValidMoves)) {
			if (huntersNearCD(hunterLocs) >= 2) {
				return false;
			}
			registerBestPlay("KL", "noice");
			return true;
		}
	}	
	if (pastLocs[numPastLocs - 1] == BUCHAREST) {
		if (!isValid("KL", validMoves, numValidMoves)) {

		} else {
			registerBestPlay("KL", "noice");
			return true;
		}
	}
	return false;
}


bool shouldIGoToCastleDrac(PlaceId *pastLocs, PlaceId *validMoves, int numPastLocs, int numValidMoves, int hunterNum, PlaceId hunterLocs[]) {
	for (int i = 0; i < numValidMoves; i++) {
		// If any of the Valid Moves correspond to CASTLE_DRACULA:
		if (MoveToLoc(pastLocs, validMoves[i], &numPastLocs) == CASTLE_DRACULA
			&& huntersNearCD(hunterLocs) <= hunterNum) {										
			registerBestPlay(strdup(placeIdToAbbrev(validMoves[i])), "oi, you want fight?");
			return true;
		} 
	}
	return false;
}


	// if (isCountry(England, DvGetPlayerLocation(dv, PLAYER_DRACULA), SIZE_OF_ENGLAND)
	// 	||  isCountry(Italy, DvGetPlayerLocation(dv, PLAYER_DRACULA), SIZE_OF_ITALY))
	// riskLevel[pastLocs[numPastLocs - 1]] += 6;
	// riskLevel[pastLocs[numPastLocs - 2]] += 6;
	// riskLevel[pastLocs[numPastLocs - 3]] += 6;


		// int average = 0;
	// for (int i = 0; i < numValidMoves; i++) {
	// 	average += riskLevel[MoveToLoc(pastLocs, validMoves[i], &numPastLocs)];
	// }
	// average = average / numValidMoves;
	// if (average > 9) {
	// 	for (int i = 0; i < 6; i++) {
	// 		riskLevel[pastLocs[numPastLocs - i]] += 3;
	// 	}
	// }