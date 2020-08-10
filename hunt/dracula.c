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
#define SIZE_OF_ENGLAND 7
#define SIZE_OF_SPAIN 11
#define SIZE_OF_ITALY 7
#define SIZE_OF_FRANCE 8
#define SIZE_OF_CENTRAL_EUROPE 12
#define SIZE_OF_EAST_EUROPE 12
#define SIZE_OF_WEST_SEAS 5
#define SIZE_OF_CENTRAL_SEAS 1
#define SIZE_OF_SEAS 10
#define SIZE_OF_UPPER_EUROPE 9
#define SIZE_OF_CD_AREA 19

static PlaceId areaCD[] = {CASTLE_DRACULA, BUDAPEST, KLAUSENBURG, SZEGED, BELGRADE,
							BUCHAREST, SOFIA, CONSTANTA, ZAGREB, SARAJEVO, VIENNA, SALONICA,
							BLACK_SEA, IONIAN_SEA, VENICE, PRAGUE, GALATZ, TYRRHENIAN_SEA,
							ATHENS};
static PlaceId seas[] = {NORTH_SEA, ENGLISH_CHANNEL, IRISH_SEA, BAY_OF_BISCAY, ATLANTIC_OCEAN,
						MEDITERRANEAN_SEA, IONIAN_SEA, ADRIATIC_SEA, TYRRHENIAN_SEA, BLACK_SEA};
static PlaceId PortCities[] = {BARI, ALICANTE, AMSTERDAM, ATHENS, CADIZ, GALWAY,
						LISBON, BARCELONA, BORDEAUX, NANTES, SANTANDER,
						CONSTANTA, VARNA, CAGLIARI, DUBLIN, EDINBURGH, 
						LE_HAVRE, LONDON, PLYMOUTH, GENOA, HAMBURG,
						SALONICA, VALONA, LIVERPOOL, SWANSEA, MARSEILLES,
						NAPLES, ROME};
static PlaceId England[] = {EDINBURGH, ENGLISH_CHANNEL, LONDON, MANCHESTER, LIVERPOOL, SWANSEA, PLYMOUTH};
static PlaceId CentralEurope[] = {STRASBOURG, BRUSSELS, COLOGNE, AMSTERDAM, HAMBURG, LEIPZIG,
 							PRAGUE, NUREMBURG, ZURICH, MUNICH, FRANKFURT, BERLIN};
static PlaceId Spain[] = {LISBON, MEDITERRANEAN_SEA, ATLANTIC_OCEAN, MADRID, GRANADA, ALICANTE, SARAGOSSA, BARCELONA, SANTANDER, CADIZ, LISBON};
static PlaceId France[] = {TOULOUSE, CLERMONT_FERRAND, MARSEILLES, LE_HAVRE, NANTES,
					GENEVA, STRASBOURG, BORDEAUX};
static PlaceId Italy[] = {GENOA, FLORENCE, ROME, NAPLES, BARI, VENICE, MILAN};
static PlaceId UpperEurope[] = {AMSTERDAM, HAMBURG, BRUSSELS, BERLIN, LEIPZIG
								, FRANKFURT, PRAGUE, NUREMBURG, VIENNA};
bool shouldIGoToCastleDrac(PlaceId *pastLocs, PlaceId *validMoves, int numPastLocs, int numValidMoves, int hunterNum, PlaceId hunterLocs[]);
bool isValid (char *play, PlaceId *validMoves, int numValidMoves);
void prioritiseCastleDrac(int riskLevel[], PlaceId hunterLocs[]);
bool iAmNearCD(PlaceId location);
PlaceId MoveToLoc(PlaceId *pastLocs, PlaceId location, int *numPastLocs);
bool isPortCity(PlaceId i);
void getHunterLocs(DraculaView dv, PlaceId hunterLocs[]);
int huntersNearCD(PlaceId hunterLocs[]);
int huntersInCountry (PlaceId country[], PlaceId hunterLocs[], int size);
int isDoubleBack(PlaceId location);
bool isCountry (PlaceId country[], PlaceId location, int size);
bool LoopStrat(PlaceId *pastLocs, PlaceId *validMoves, int numValidMoves, int numPastLocs, PlaceId hunterLocs[]);
void decideDraculaMove(DraculaView dv)
{
	PlaceId currLoc = DvGetPlayerLocation(dv, PLAYER_DRACULA); // Dracula's current location.
	int health = DvGetHealth(dv, PLAYER_DRACULA); // Dracula's Blood Points.
	int numValidMoves = 0;						  // Number of Valid Locations for Dracula.	
	int numRiskyLocs = 0;					      // Number of Risky Locations for Dracula.
	int numPastLocs = 0;						  // Number of Past Locations in Dracula's move history.
	PlaceId hunterLocs[4]; 
	getHunterLocs(dv, hunterLocs); 				  // Array of current hunter locations.
	Round round = DvGetRound(dv);				  // The current round in the game.
	int riskLevel[NUM_REAL_PLACES] = {0};		  // Array containing risk levels for each place. 
	char *play = NULL;	  // The play to be made.
	PlaceId *validMoves = DvGetValidMoves(dv, &numValidMoves);
	PlaceId *pastLocs = DvGetLocationHistory(dv, &numPastLocs);  

	///////////////////////////////////////////////////////////////////
	// ----------------------STARTING ROUND------------------------- //
	///////////////////////////////////////////////////////////////////

	// Round 0 Move
	if (round == 0) {
		if (huntersNearCD(hunterLocs) <= 1) {
			registerBestPlay("CD", "liam neesons"); 
			return;
		}
		else {
			for (int player = 0; player < 4; player++) {
				if (hunterLocs[player] == STRASBOURG) {
					registerBestPlay("AL", "liam neesons"); 
					return;
				}
			}
		 	registerBestPlay("ST", "liam neesons"); 
			return;
		}
	}

	// Round 1 Move
	if (round == 1 && pastLocs[0] == CASTLE_DRACULA) {
		if (huntersNearCD(hunterLocs) <= 1) registerBestPlay("D1", "liam neesons"); return;
	}

	// Round 2 Move
	if (round == 2 && pastLocs[0] == CASTLE_DRACULA) {
		if (huntersNearCD(hunterLocs) <= 1) registerBestPlay("HI", "liam neesons"); return;
	}
	
	// If Dracula has no valid moves, use TELEPORT.
	if (validMoves == NULL) { 
		registerBestPlay("TP", "liam neesons i am neesons"); 
		return;
	}

	riskLevel[VALONA] = riskLevel[ATHENS] = riskLevel[SALONICA] = riskLevel[COLOGNE] = 20;
	if (pastLocs[numPastLocs - 1] == KLAUSENBURG && isValid("CD", validMoves, numValidMoves)) {
		riskLevel[GALATZ] += 100;
	}
	// Go to Castle Dracula if it is safe.
	if (shouldIGoToCastleDrac(pastLocs, validMoves, numPastLocs, numValidMoves, 1, hunterLocs)) return;
	else riskLevel[CASTLE_DRACULA] += 20;

	////////////////////////////////////////////////////////////////////
	// --------------ASSIGNING RISK LEVELS TO EACH LOCATION---------- //
	////////////////////////////////////////////////////////////////////

	int hunterRisk[4];
	for (int player = PLAYER_LORD_GODALMING; player < PLAYER_DRACULA; player++) {
		// If Dracula is very healthy: Hunters have a risk value of maximum 3.
		if (health >= 60) hunterRisk[player] = DvGetHealth(dv, player) - 6; 

		// Usually hunters have a risk value of maximum 6.
		else hunterRisk[player] = DvGetHealth(dv, player) - 3;

		// If low on health, hunters have a risk value of 30.
		if (health < 20) hunterRisk[player] = 30;
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
		for (int i = 0; i < numRiskyLocs; i++) if (placeIsLand(riskyLocsRoad[i])) riskLevel[riskyLocsRoad[i]] += hunterRisk[player] + 10;
		
		// Locations reachable by rail
		PlaceId *riskyLocsRail = DvWhereCanTheyGoByType(dv, player, false, true, false, &numRiskyLocs);
		for (int i = 0; i < numRiskyLocs; i++) if (placeIsLand(riskyLocsRail[i])) riskLevel[riskyLocsRail[i]] += hunterRisk[player] + 10;

		// Locations reachable by sea
		PlaceId *riskyLocsSea = DvWhereCanTheyGoByType(dv, player, false, false, true, &numRiskyLocs);
		for (int i = 0; i < numRiskyLocs; i++) if (placeIsLand(riskyLocsSea[i])) riskLevel[riskyLocsSea[i]] += hunterRisk[player] + 10;

		// // Risky Locs in general
		PlaceId *riskyLocs = DvWhereCanTheyGo(dv, player, &numRiskyLocs);
		int numMyLocs = 0;
		PlaceId *myLocs = DvWhereCanIGoByType(dv, true, false, &numMyLocs);
		for (int i = 0; i < numRiskyLocs ; i++) {
			ConnList list = MapGetConnections(m, riskyLocs[i]);
			for (int j = 0; j < numMyLocs; j++) {
				ConnList myList = MapGetConnections(m, myLocs[j]);
				ConnList curr = myList->next;
				for (ConnList curr = list->next; curr != NULL; curr = curr->next) {
					if (placeIsLand(curr->p)) {
						for (ConnList curr2 = myList->next; curr2 != NULL; curr2 = curr2->next) {
							if (curr->p == curr2->p) riskLevel[curr2->p] += 1;
						}
					}
				}
			}
		}
	}
	MapFree(m);

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

	// Don't backtrack at CD!
	if (iAmNearCD(currLoc)) for (int i = 1; i <= numPastLocs && i < 6; i++) riskLevel[pastLocs[numPastLocs - i]] += 20;

	// If low on health, do not go to Seas. 
	// Else prefer to not travel by sea to avoid wasting health.
	if (health <= 20) for (int i = 0; i < SIZE_OF_SEAS; i++) riskLevel[seas[i]] += 20;
	if (health <= 16) for (int i = 0; i < SIZE_OF_SEAS; i++) riskLevel[seas[i]] += 30;
	else for (int i = 0; i < SIZE_OF_SEAS; i++) riskLevel[seas[i]] += 1;
	if (health <= 6) for (int i = 0; i < SIZE_OF_SEAS; i++) riskLevel[seas[i]] += 300;

	// Do not go to Port cities - decrease chance of being forced out to sea.
	if (isPortCity(currLoc) && health < 30) {
		for (int i = 1; i <= numPastLocs && i < 6; i++) riskLevel[pastLocs[numPastLocs - i]] += 1;
	}

	// // Special Case: Prioritise Budapest over Zagreb if in Vienna and there are not many hunters near CD.
	// if (huntersNearCD(hunterLocs) <= 1 && currLoc == VIENNA) {
	// 	riskLevel[ZAGREB] += 2;
	// 	riskLevel[BUDAPEST] -= 1;
	// }

	// // Prefer to choose Alicante over Barcelona if in MEDITERRANEAN SEA.
	// if (currLoc == MEDITERRANEAN_SEA) {
	// 	riskLevel[BARCELONA] += 2;
	// 	riskLevel[ALICANTE] -= 2;
	// }

	// Port Cities are more risky if hunters are at sea.
	for (int i = 0; i < 4; i++) if (placeIsSea(hunterLocs[i])) {
		for (int j = 0; j < NUM_PORT_CITIES; j++) {
			riskLevel[PortCities[j]] += 1;
		}
	}

	// Don't go to sea if low on health.
	if (health <= 10) {
		for (int i = 0; i < SIZE_OF_SEAS; i++) riskLevel[seas[i]] += 10;
		for (int i = 0; i < NUM_PORT_CITIES; i++) riskLevel[PortCities[i]] += 6;
	}

	// Flee to sea if in danger near CD.
	if (huntersNearCD(hunterLocs) >= 3) {
		for (int i = 0; i < NUM_PORT_CITIES; i++) riskLevel[PortCities[i]] -= 14;
		if (health > 6) {
			riskLevel[BLACK_SEA] -= 15;
			riskLevel[ADRIATIC_SEA] -= 5;
		}
	}

	// If hunters are approaching, don't backtrack.
	for (int i = 1; i < 6 && i <= numPastLocs; i++) {
		for (int player = 0; player < 4; player++) {
			if (hunterLocs[player] == pastLocs[numPastLocs - i]) {
				if (DvGetHealth(dv, player) > 4) riskLevel[pastLocs[numPastLocs - i]] += 15;
			}
		}
	}

	// Prioritise Castle Dracula if no hunters there.
	if (huntersNearCD(hunterLocs) == 0) prioritiseCastleDrac(riskLevel, hunterLocs);
	
	/////////////////////////////////////////////////////////////////////////////
	// ---------------------COMPUTING LOWEST RISK MOVE------------------------ //
	/////////////////////////////////////////////////////////////////////////////
	
	// Avoid going to Italy from Tyrrhenian Sea as it is easy to get cornered.
	if (currLoc == TYRRHENIAN_SEA) for (int i = 0; i < SIZE_OF_ITALY; i++) riskLevel[Italy[i]] += 10;
	
	// The sea is safe if you and a hunter are in the same location.
	for (int player = 0; player < 4; player++) {
		if ((currLoc == hunterLocs[player])) {
			int number = 0;
			PlaceId *whereCanIGo = DvWhereCanIGo(dv, &number);
			for (int i = 0; i < number; i++) if (placeIsSea(whereCanIGo[i])) riskLevel[whereCanIGo[i]] -= 15;
		}
	}

	// Don't backtrack at sea!
	if (placeIsSea(currLoc) || health <= 8) for (int i = 1; i < 6 && i <= numPastLocs; i++) riskLevel[pastLocs[numPastLocs - i]] += 10;

	// Try to go to BLACK_SEA if at CONSTANTA.
	if (currLoc == CONSTANTA) {
		for (int i = 0; i < numValidMoves; i++) {
			if (MoveToLoc(pastLocs, validMoves[i], &numPastLocs) == BLACK_SEA) {
				if (health >= 6) riskLevel[BLACK_SEA] -= 30;
			}
		}
	}

	// Use the loop strategy if possible...
	if (health >= 22) if (LoopStrat(pastLocs, validMoves, numValidMoves, numPastLocs, hunterLocs)) return;
	
	// Conditions for prioritising castle dracula
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

	// for (int i = 0; i < numValidMoves; i++) {
	// 	printf("validMoves[%d] is %s with risk %d\n", i, placeIdToName(MoveToLoc(pastLocs, validMoves[i], &numPastLocs)), riskLevel[MoveToLoc(pastLocs, validMoves[i], &numPastLocs)]);
	// }
	// If there are no low risk moves pick a random valid move.
	if (lowRiskNum == 0) {
		// strcpy(play, placeIdToAbbrev(validMoves[0]));
		registerBestPlay(strdup(placeIdToAbbrev(validMoves[0])), "liam neesons");
		return;
	}

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
	registerBestPlay(strdup(placeIdToAbbrev(minimum)), "liam neesons");
	free(lowRiskMoves);
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

bool isPortCity(PlaceId i) {
	for (int k = 0; k < NUM_PORT_CITIES; k++) if (i == PortCities[k]) return true;
	return false;
}


// Gets current locations of each Hunter.
void getHunterLocs(DraculaView dv, PlaceId hunterLocs[]) {
	for (int player = 0; player < 4; player++) hunterLocs[player] = DvGetPlayerLocation(dv, player);
}

void prioritiseCastleDrac(int riskLevel[], PlaceId hunterLocs[]) {
	riskLevel[CASTLE_DRACULA] = -50;
	riskLevel[GALATZ] = -5;
	riskLevel[BUDAPEST] -= 2;
	riskLevel[KLAUSENBURG] -= 5;
	riskLevel[SZEGED] -= 3;
	riskLevel[BELGRADE] -= 2;
	riskLevel[BUCHAREST] -= 4;
	riskLevel[SOFIA] -= 2;
	riskLevel[CONSTANTA] -= 2;
	riskLevel[ZAGREB] -= 3;
	riskLevel[VARNA] -= 1;
	riskLevel[PRAGUE] -= 2;
	riskLevel[VIENNA] -= 1;
	return;
}

bool iAmNearCD(PlaceId location) {
	for (int i = 0; i < SIZE_OF_CD_AREA; i++) {
		if (location == areaCD[i]) return true;
	}
	return false;
}

int huntersNearCD(PlaceId hunterLocs[]) 
{
	int count = 0;
	for (int player = 0; player < 4; player++) {
		if (isCountry(areaCD, hunterLocs[player], SIZE_OF_CD_AREA)) count++;
	}
	return count;
}

int isDoubleBack(PlaceId location)
{
	return (location >= DOUBLE_BACK_1 && location <= DOUBLE_BACK_5);
}

bool isCountry (PlaceId country[], PlaceId location, int size) 
{
	for (int i = 0; i < size; i++) {
		if (location == country[i]) return true;
	}
	return false;
}

int huntersInCountry (PlaceId country[], PlaceId hunterLocs[], int size) 
{
	int count = 0;
	for (int player = 0; player < 4; player++) {
		if (isCountry(country, hunterLocs[player], size)) {
			count++;
		}
	}
	return count;
}

bool isValid (char *play, PlaceId *validMoves, int numValidMoves) {
	for (int i = 0; i < numValidMoves; i++) 
		if (strstr(placeIdToAbbrev(validMoves[i]), play)) return true;
	return false;
}

bool LoopStrat(PlaceId *pastLocs, PlaceId *validMoves, int numValidMoves, int numPastLocs, PlaceId hunterLocs[]) {
	if (pastLocs[numPastLocs - 1] == KLAUSENBURG) {
		if (isValid("CD", validMoves, numValidMoves)) {
			registerBestPlay("CD", "liam neesons");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == CASTLE_DRACULA) {
		if (isValid("GA", validMoves, numValidMoves)) {
			registerBestPlay("GA", "liam neesons");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == GALATZ) {
		if (isValid("CN", validMoves, numValidMoves)) {
			registerBestPlay("CN", "liam neesons");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == CONSTANTA) {
		if (isValid("BS", validMoves, numValidMoves)) {
			registerBestPlay("BS", "liam neesons");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == BLACK_SEA) {
		if (isValid("IO", validMoves, numValidMoves)) {
			registerBestPlay("IO", "liam neesons");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == IONIAN_SEA) {
		if (isValid("TS", validMoves, numValidMoves)) {
			registerBestPlay("TS", "liam neesons");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == TYRRHENIAN_SEA) {
		if (isValid("MS", validMoves, numValidMoves)) {
			registerBestPlay("MS", "liam neesons");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == ALICANTE) {
		if (huntersInCountry(Spain, hunterLocs, SIZE_OF_SPAIN) >= 1) {
			if (isValid("MS", validMoves, numValidMoves)) {
				registerBestPlay("MS", "liam neesons");
				return true;
			} 
		}
		for (int player = 0; player < 4; player++) {
			if (hunterLocs[player] == MEDITERRANEAN_SEA) {
				if (isValid("MS", validMoves, numValidMoves)) {
					registerBestPlay("MS", "liam neesons");
					return true;
				}
			}
		}
		if (isValid("SR", validMoves, numValidMoves)) {
			registerBestPlay("SR", "liam neesons");
			return true;
		}
	} 
	if (pastLocs[numPastLocs - 1] == SARAGOSSA) {
		if ((huntersInCountry(Spain, hunterLocs, SIZE_OF_SPAIN) + huntersInCountry(France, hunterLocs, SIZE_OF_FRANCE)) > 1) {
			for (int player = 0; player < 4; player++) {
				if (hunterLocs[player] == TOULOUSE || hunterLocs[player] == MEDITERRANEAN_SEA) {
					if (isValid("SN", validMoves, numValidMoves)) {
						registerBestPlay("SN", "liam neesons");
						return true;
					}
				}
			}
			if (isValid("BA", validMoves, numValidMoves)) {
				registerBestPlay("BA", "liam neesons");
				return true;
			}
		}
		for (int player = 0; player < 4; player++) {
			if (hunterLocs[player] == MADRID || hunterLocs[player] == LISBON || hunterLocs[player] == BARCELONA) {
				if (isValid("BO", validMoves, numValidMoves)) {
					registerBestPlay("BO", "liam neesons");
					return true;
				}
			}
		}
		if (isValid("MA", validMoves, numValidMoves)) {
			registerBestPlay("MA", "liam neesons");
			return true;
		}
	} 	
	if (pastLocs[numPastLocs - 1] == BORDEAUX) {
		if (isValid("BB", validMoves, numValidMoves)) {
			registerBestPlay("BB", "liam neesons");
			return true;
		}
	} 
	if (pastLocs[numPastLocs - 1] == BARCELONA) {
		if (isValid("MS", validMoves, numValidMoves)) {
			registerBestPlay("MS", "liam neesons");
			return true;
		}
	} 
	if (pastLocs[numPastLocs - 1] == MADRID) {
		if ((huntersInCountry(Spain, hunterLocs, SIZE_OF_SPAIN) + huntersInCountry(France, hunterLocs, SIZE_OF_FRANCE)) >= 1) {
			if (isValid("SN", validMoves, numValidMoves)) {
				registerBestPlay("SN", "liam neesons");
				return true;
			}
		}
		if (isValid("LS", validMoves, numValidMoves)) {
			registerBestPlay("LS", "liam neesons");
			return true;
		}
	} 
	if (pastLocs[numPastLocs - 1] == SANTANDER) {
		if (huntersInCountry(Spain, hunterLocs, SIZE_OF_SPAIN) + 
		huntersInCountry(France, hunterLocs, SIZE_OF_FRANCE) <= 1) {
			if (isValid("LS", validMoves, numValidMoves)) {
				registerBestPlay("LS", "liam neesonse");
				return true;
			}
		}
		if (isValid("BB", validMoves, numValidMoves)) {
			registerBestPlay("BB", "liam neesons");
			return true;
		}
	} 
	if (pastLocs[numPastLocs - 1] ==BAY_OF_BISCAY) {
		if (isValid("AO", validMoves, numValidMoves)) {
			registerBestPlay("AO", "liam neesons");
			return true;
		}
	} 
	if (pastLocs[numPastLocs - 1] == LISBON) {
		if ((huntersInCountry(Spain, hunterLocs, SIZE_OF_SPAIN) 
		+ huntersInCountry(France, hunterLocs, SIZE_OF_FRANCE)) > 1) {
			if (isValid("AO", validMoves, numValidMoves)) {
				registerBestPlay("AO", "liam neesons");
				return true;
			}
		}
		if (isValid("CA", validMoves, numValidMoves)) {
			registerBestPlay("CA", "liam neesons");
			return true;
		}
	} 
	if (pastLocs[numPastLocs - 1] == CADIZ) {
		if (huntersInCountry(Spain, hunterLocs, SIZE_OF_SPAIN) >= 1) {
			if (isValid("AO", validMoves, numValidMoves)) {
				registerBestPlay("AO", "liam neesons");
				return true;
			}
		}
		if (isValid("GR", validMoves, numValidMoves)) {
			registerBestPlay("GR", "liam neesons");
			return true;
		}
	} 
	if (pastLocs[numPastLocs - 1] == GRANADA) {
		if (isValid("AL", validMoves, numValidMoves)) {
			registerBestPlay("AL", "liam neesons");
			return true;
		}
	} 
	if (pastLocs[numPastLocs - 1] == MEDITERRANEAN_SEA) {
		if (huntersNearCD(hunterLocs) >= 3 || huntersInCountry(France, hunterLocs, SIZE_OF_FRANCE) <= 1) {
			if (isValid("AL", validMoves, numValidMoves)) {
				registerBestPlay("AL", "liam neesons");
				return true;
			}
		}
		if (isValid("AO", validMoves, numValidMoves)) {
			registerBestPlay("AO", "liam neesons");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == ATLANTIC_OCEAN) {
		if (huntersInCountry(Spain, hunterLocs, SIZE_OF_SPAIN) == 0) {
			for (int i = 0; i < numValidMoves; i++) {
				if (isCountry(Spain, MoveToLoc(pastLocs, validMoves[i], &numPastLocs), SIZE_OF_SPAIN)) {
					registerBestPlay(strdup(placeIdToAbbrev(validMoves[i])), "liam neesons");
					return true;
				}
			}
		}
		if (huntersInCountry(Spain, hunterLocs, SIZE_OF_SPAIN) < 1) {
			if (isValid("MS", validMoves, numValidMoves)) {
				registerBestPlay("MS", "liam neesons");
				return true;
			}
		}
		if (isValid("NS", validMoves, numValidMoves)) {
			registerBestPlay("NS", "liam neesons");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == NORTH_SEA) {
		if (isValid("HA", validMoves, numValidMoves)) {
			if (huntersInCountry(UpperEurope, hunterLocs, SIZE_OF_UPPER_EUROPE) >= 1) {
				if (isValid("EC", validMoves, numValidMoves)) {
					registerBestPlay("EC", "liam neesons");
					return true;
				}
			}
		
		registerBestPlay("HA", "liam neesons");
		return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == HAMBURG) {
		if (isValid("BR", validMoves, numValidMoves)) {
			registerBestPlay("BR", "liam neesons");
			return true;
		}
	}

	if (pastLocs[numPastLocs - 1] == BERLIN) {
		if (isValid("PR", validMoves, numValidMoves)) {
			if (huntersNearCD(hunterLocs) >= 2) {
				for (int player = 0; player < 4; player++) {
					if (hunterLocs[player] == PRAGUE ||
						hunterLocs[player] == VIENNA ||
						hunterLocs[player] == ZAGREB ||
						hunterLocs[player] == MUNICH ||
						hunterLocs[player] == NUREMBURG ||
						hunterLocs[player] == BUDAPEST ||
						hunterLocs[player] == KLAUSENBURG) {
							registerBestPlay("LI", "liam neesons");
							return true;
					}
				}
			}
			registerBestPlay("PR", "liam neesons");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == ENGLISH_CHANNEL) {
		if (huntersInCountry(England, hunterLocs, SIZE_OF_ENGLAND)
			+ huntersInCountry(France, hunterLocs, SIZE_OF_FRANCE) <= 1) {
			if (isValid("PL", validMoves, numValidMoves)) {
				registerBestPlay("PL", "liam neesons");
				return true;
			}			
		}
		if (isValid("AO", validMoves, numValidMoves)) {
			registerBestPlay("AO", "liam neesons");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == PRAGUE) {
		if (isValid("VI", validMoves, numValidMoves)) {
			if (huntersNearCD(hunterLocs) >= 2) {
				return false;
			}
			registerBestPlay("VI", "liam neesons");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == PLYMOUTH) {
		if (isValid("LO", validMoves, numValidMoves)) {
			registerBestPlay("LO", "liam neesons");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == LONDON) {
		if (isValid("MN", validMoves, numValidMoves)) {
			registerBestPlay("MN", "liam neesons");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == MANCHESTER) {
		if (isValid("LV", validMoves, numValidMoves)) {
			registerBestPlay("LV", "liam neesons");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == LIVERPOOL) {
		if (huntersInCountry(England, hunterLocs, SIZE_OF_ENGLAND) >= 1) {
			if (isValid("IR", validMoves, numValidMoves)) {
				registerBestPlay("IR", "liam neesons");
				return true;
			}
		}
	}
	if (pastLocs[numPastLocs - 1] == IRISH_SEA) {
		if (isValid("DU", validMoves, numValidMoves)) {
			registerBestPlay("DU", "liam neesons");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == DUBLIN) {
		if (isValid("GW", validMoves, numValidMoves)) {
			registerBestPlay("GW", "liam neesons");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == GALWAY) {
		if (isValid("AO", validMoves, numValidMoves)) {
			registerBestPlay("AO", "liam neesons");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == MANCHESTER) {
		if (isValid("LV", validMoves, numValidMoves)) {
			registerBestPlay("LV", "liam neesons");
			return true;
		}
	}

	if (pastLocs[numPastLocs - 1] == VIENNA) {
		if (huntersNearCD(hunterLocs) >= 2) {
			return false;
		}
		bool canGoBD = true;
		bool canGoZA = true;
		for (int player = 0; player < 4; player++) {
			if (hunterLocs[player] == BUDAPEST) {
				canGoBD = false;
			}
			if (hunterLocs[player] == ZAGREB) {
				canGoZA = false;
			}
		}
		if (canGoBD) {
			if (isValid("BD", validMoves, numValidMoves)) {
				registerBestPlay("BD", "liam neesons");
				return true;
			}
		}
		if (canGoZA) {
			if (isValid("ZA", validMoves, numValidMoves)) {
				registerBestPlay("ZA", "liam neesons");
				return true;
			}
		}
	}
	if (pastLocs[numPastLocs - 1] == ZAGREB) {
		if (isValid("SJ", validMoves, numValidMoves)) {
			registerBestPlay("SJ", "liam neesons");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == SARAJEVO) {
		if (isValid("SO", validMoves, numValidMoves)) {
			registerBestPlay("SO", "liam neesons");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == SOFIA) {
		if (isValid("BC", validMoves, numValidMoves)) {
			registerBestPlay("BC", "liam neesons");
			return true;
		}
	}
	if (pastLocs[numPastLocs - 1] == BUDAPEST) {
		if (isValid("KL", validMoves, numValidMoves)) {
			if (huntersNearCD(hunterLocs) >= 2) {
				return false;
			}
			registerBestPlay("KL", "liam neesons");
			return true;
		}
	}	
	if (pastLocs[numPastLocs - 1] == BUCHAREST) {
		if (isValid("KL", validMoves, numValidMoves)) {
			registerBestPlay("KL", "liam neesons");
			return true;
		}
	}
	return false;
}

bool shouldIGoToCastleDrac(PlaceId *pastLocs, PlaceId *validMoves, int numPastLocs, int numValidMoves, int hunterNum, PlaceId hunterLocs[]) {
	for (int i = 0; i < numValidMoves; i++) {
		if (MoveToLoc(pastLocs, validMoves[i], &numPastLocs) == CASTLE_DRACULA && huntersNearCD(hunterLocs) <= hunterNum) {
			bool visitedGalatz = false;
			bool visitedKlaus = false;
			for (int k = 1; k <= numPastLocs && k < 6; k++) {
				if (pastLocs[numPastLocs - k] == GALATZ) visitedGalatz = true;
				if (pastLocs[numPastLocs - k] == KLAUSENBURG) visitedKlaus = true;
			}		
			if (huntersNearCD(hunterLocs) == 1) if (visitedKlaus) return false;
			if (visitedGalatz && visitedKlaus) return false;
			
			registerBestPlay(strdup(placeIdToAbbrev(validMoves[i])), "liam neesons?");
			return true;
		} 
	}
	return false;
}