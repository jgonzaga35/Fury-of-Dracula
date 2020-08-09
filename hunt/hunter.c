////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "Game.h"
#include "hunter.h"
#include "Map.h"
#include "Places.h"
#include "HunterView.h"

#define TRUE				1
#define FALSE				0
#define NUM_LOCS_NEAR_CD 	12

#define NUM_PORT_CITIES 28
#define SIZE_OF_ENGLAND 6
#define SIZE_OF_SPAIN 6
#define SIZE_OF_ITALY 7

PlaceId doRandom(HunterView hv, Player hunter, PlaceId *places, int numLocs);
PlaceId moveComplement(HunterView hv, Player currHunter);
static void getHunterLocs(HunterView hv, PlaceId hunterLocs[]);
static int isKnown(PlaceId location);
static void closestToVampire(HunterView hv, Player currHunter, int *locRank);
PlaceId lowestRiskForDracula(HunterView hv, PlaceId *places, int numLocs, PlaceId hunterLocs[4]);
int hasHuntersThere(PlaceId hunterLocs[4], PlaceId location);
bool isCountry(PlaceId country[], PlaceId location, int size);
PlaceId neighbourCities(HunterView hv, PlaceId DraculaLoc, Player currHunter, PlaceId hunterLocs[4]);

int isThereCDInReachable(PlaceId *places, int numLocs);
int isPlayMinaDr(Player currHunter);

void decideHunterMove(HunterView hv) {
	Round round = HvGetRound(hv);
	Player currHunter = HvGetPlayer(hv); // Which hunter?
	
	int doneWithBestMove = FALSE;
	if (round == 0) { // FIRST ROUND
		char *location = NULL;
		switch(currHunter) {
			case PLAYER_LORD_GODALMING:
				location = "MN";
				break;
			case PLAYER_DR_SEWARD:
				location = "GA";
				break;
			case PLAYER_VAN_HELSING:
				location = "FR";
				break;
			case PLAYER_MINA_HARKER:
				location = "MA";
				break;
			default:
				break;
		}
		
		registerBestPlay(location, "--Start--");
		return;
	} else {
		// for all other rounds

		// ---------------Initialize some variables to be used--------------------
		PlaceId hunterLocs[4]; 
		getHunterLocs(hv, hunterLocs); 
		PlaceId currLoc = hunterLocs[currHunter];
		int maxByRail = (round + currHunter) % 4;

		int locRank[NUM_REAL_PLACES] = {0};
		
		int numLocs = -1;
		PlaceId *places = HvWhereCanIGo(hv, &numLocs);
		int canGoCD = isThereCDInReachable(places, numLocs);

		// ------------------Move to random loc (Safe with timing)------------------
		registerBestPlay(strdup(placeIdToAbbrev(doRandom(hv, currHunter, places, numLocs))), "general random");

		Round LastDracRoundSeen = -1;
		PlaceId DraculaLoc = HvGetLastKnownDraculaLocation(hv, &LastDracRoundSeen);

		/////////////////////////////////////////////////////////////////////////////
		// --------------------When we know where is Dracula---------------------- //
		/////////////////////////////////////////////////////////////////////////////
		if(DraculaLoc != NOWHERE) { 	//  && LastDracRoundSeen != -1
			int diff = HvGetRound(hv) - LastDracRoundSeen; // how many rounds ago

			printf("Dracula is at %s %s, %d rounds before\n", placeIdToAbbrev(DraculaLoc), placeIdToName(DraculaLoc), diff);

			// If Dracula is there in the past 10 rounds
			if (0 <= diff && diff <= 6) {
				doneWithBestMove = TRUE;
				int pathLength = -1;
				PlaceId *path = HvGetShortestPathTo(hv, currHunter, DraculaLoc, &pathLength);

				// If we are with Dracula this round / we can arrive at where Dracula is right now in a move, definitely stay / move to encounter
				if ((pathLength == 0 || pathLength == 1) && diff == 1) {
					registerBestPlay(strdup(placeIdToAbbrev(DraculaLoc)), "--Encounter--");
					printf("Player %d meet dracula at %s %s\n", currHunter, placeIdToAbbrev(DraculaLoc), placeIdToName(DraculaLoc));
					goto end;
				}

				// If we can arrive where Dracula is right now in two move
				else if (pathLength == 2 && diff <= 1) {
					registerBestPlay(strdup(placeIdToAbbrev(path[0])), "--Drac--");
				}	

				// If we are get the place where Dracula was in the last round, simply go to a neighbour that's the lowest risk for dracula
				else if (pathLength == 0 && diff == 2) {
					// Shouldn't go by rail, might go too far, since we are only one city away
					int numReturnedLocs = -1;
					PlaceId *notByRail = HvWhereCanIGoByType(hv, true, false, true, &numReturnedLocs);
					if (numReturnedLocs > 0) {
						PlaceId lowestRisk = lowestRiskForDracula(hv, notByRail, numReturnedLocs, hunterLocs);
						registerBestPlay(strdup(placeIdToAbbrev(lowestRisk)), "--Reachable--");
					// If no such exist, then go anythere reachable
					} else {
						PlaceId lowestRisk = lowestRiskForDracula(hv, places, numLocs, hunterLocs);
						registerBestPlay(strdup(placeIdToAbbrev(lowestRisk)), "--Reachable--");
					}
				}

				else if (maxByRail >= 2 && diff >= 3) {
					int numReturnedLocs = -1;
					PlaceId *byRail = HvWhereCanIGoByType(hv, false, true, false, &numReturnedLocs);

					if (numReturnedLocs > 0) {
						PlaceId lowestRisk = lowestRiskForDracula(hv, byRail, numReturnedLocs, hunterLocs);
						registerBestPlay(strdup(placeIdToAbbrev(lowestRisk)), "--Rail--");
					}
				}
				
				// In all other sitution, go to the neighbouring cities of where dracula was
				else {
					// Get the neighbouring cities of where dracula is in an array
					Map m = MapNew();	
					ConnList list = MapGetConnections(m, DraculaLoc);
					ConnList curr = list;
					PlaceId neighbouringCity[NUM_REAL_PLACES];
					int i = 0;
					while (curr != NULL) {
						neighbouringCity[i] = curr->p;
						i++;
						curr = curr->next;
					}

					if (i > 0) {
						PlaceId placeToGo = lowestRiskForDracula(hv, neighbouringCity, i, hunterLocs);

						int pathLengthToNeighbouring = -1;
						PlaceId *pathToNeighbouring = HvGetShortestPathTo(hv, currHunter, placeToGo, &pathLengthToNeighbouring);

						if (placeIsReal(pathToNeighbouring[0])) {
							registerBestPlay(strdup(placeIdToAbbrev(pathToNeighbouring[0])), "--neighbouring--");
						} else {
							registerBestPlay(strdup(placeIdToAbbrev(path[0])), "--Drac--");
						}
					} else {
						registerBestPlay(strdup(placeIdToAbbrev(path[0])), "--Drac--");
					}
				}
					
			// If Dracula isn't there recently, do research
			} else if (HvGetRound(hv) >= 6 && HvGetRound(hv) % 3 == 0) {
				doneWithBestMove = TRUE;
				registerBestPlay(strdup(placeIdToAbbrev(currLoc)), "Research");
			}
			return;
		} 

		/////////////////////////////////////////////////////////////////////////////
		// ----------------When we don't know where is Dracula-------------------- //
		/////////////////////////////////////////////////////////////////////////////

		if (doneWithBestMove == FALSE) {
			// ------------------If hunter health low, rest-----------------------------
			int currHunterHealth = HvGetHealth(hv, currHunter);
			if (currHunterHealth <= 3) {
				locRank[currLoc] += 2;
			}
			
			// --------------Can go to Castle if nearby-------------
			if (canGoCD) locRank[CASTLE_DRACULA] += 2;

			// ---------------shouldn't go to where other hunters are already at--------------------
			for (int i = 0; i < numLocs; i++) {
				for (int player = 0; player < 4; player++) {
					if (places[i] == hunterLocs[player]) locRank[places[i]] -= 2;
				}
			}

			// ----------Go to the vampire's location if it's known and the current player is the closest to vampire--------
			if (false) closestToVampire(hv, currHunter, locRank);

			// ----------- Don't go to the same location / SEA----------
			int numReturnedMoves;
			bool canFree;
			PlaceId *locationHistory = HvGetLocationHistory(hv, currHunter, &numReturnedMoves, &canFree);

			for (int i = 0; i < numReturnedMoves; i++) {
				if (placeIdToType(places[i]) == SEA) {
					if (isPlayMinaDr(currHunter)) locRank[places[i]] += 1;
					else locRank[places[i]] -= 1;
				}
				for (int j = 0; j < numLocs; j++) {
					if (places[j] == locationHistory[i]) locRank[places[j]] -= 10;
				}
			}

			// ----------Go to the locaion with the highest rank---------
			PlaceId max = places[0];
			for (int i = 0; i < numLocs; i++) {
				if (locRank[places[i]] > locRank[max]) max = places[i];
			}
			
			if (placeIsReal(max)) {
				registerBestPlay(strdup(placeIdToAbbrev(max)), "--Rank--");
			}
		}
	}
	end:
	return;
}

// Return a random neigbouring city
PlaceId doRandom(HunterView hv, Player hunter, PlaceId *places, int numLocs) {
	srand(time(0));
	PlaceId currLoc = HvGetPlayerLocation(hv, hunter);
	
	if(places == NULL) return currLoc;
	
	int loc = rand() % numLocs;
	while (places[loc] ==  currLoc) {
		loc = rand() % numLocs;
	}

	return places[loc];
}

// Get the location of the other hunters
static void getHunterLocs(HunterView hv, PlaceId hunterLocs[]) {
	for (int player = 0; player < 4; player++) {
		hunterLocs[player] = HvGetPlayerLocation(hv, player);
	}
}

// TODO: the logic is incorrect, if this is of high priority, then go there, increase the rank for the first step is useless
// Increase the rank if the currHunter is the closest to the vampire
static void closestToVampire(HunterView hv, Player currHunter, int locRank[NUM_REAL_PLACES]) {
	PlaceId vampireLoc = HvGetVampireLocation(hv);
	if (!isKnown(vampireLoc)) return;

	int pathLength[NUM_PLAYERS - 1];
	PlaceId *paths[NUM_PLAYERS - 1];

	for (int player = 0; player < 4; player++) {
		paths[player] = HvGetShortestPathTo(hv, player, vampireLoc, &pathLength[player]);
	}

	Player closest = PLAYER_LORD_GODALMING;
	for (int player = 0; player < 4; player++) {
		if (pathLength[player] < pathLength[closest]) closest = player;
	}

	if (closest == currHunter) locRank[paths[currHunter][0]] += 3;

	return;
}

// True is the location is known
static int isKnown(PlaceId location) {
	return (location != CITY_UNKNOWN && location != NOWHERE && location != SEA_UNKNOWN);
}

// Compute the least risky location from Dracula's perspective
PlaceId lowestRiskForDracula(HunterView hv, PlaceId *places, int numLocs, PlaceId hunterLocs[4]) {
	// PlaceId PortCities[] = {BARI, ALICANTE, AMSTERDAM, ATHENS, CADIZ, GALWAY,
	// 						LISBON, BARCELONA, BORDEAUX, NANTES, SANTANDER,
	// 						CONSTANTA, VARNA, CAGLIARI, DUBLIN, EDINBURGH, 
	// 						LE_HAVRE, LONDON, PLYMOUTH, GENOA, HAMBURG,
	// 						SALONICA, VALONA, LIVERPOOL, SWANSEA, MARSEILLES,
	// 						NAPLES, ROME};
	PlaceId England[] = {EDINBURGH, LONDON, MANCHESTER, LIVERPOOL, SWANSEA, PLYMOUTH};
	// PlaceId Ireland[] = {GALWAY, DUBLIN, IRISH_SEA};
	// PlaceId Portugal[] = {LISBON};
	PlaceId Spain[] = {MADRID, GRANADA, ALICANTE, SARAGOSSA, BARCELONA, SANTANDER};
	// PlaceId France[] = {TOULOUSE, CLERMONT_FERRAND, PARIS, MARSEILLES, LE_HAVRE, NANTES,
	// 					GENEVA, STRASBOURG};
	PlaceId Italy[] = {GENOA, FLORENCE, ROME, NAPLES, BAY_OF_BISCAY, VENICE, MILAN};

	int riskLevel[NUM_REAL_PLACES] = {0};
	for (int i = 0; i < numLocs; i++) {
		PlaceId location = places[i];
		// FIXME: make some hunter favour sea otherwise we can't get to sea
		if (placeIdToType(location) == SEA) {
			if (isPlayMinaDr(HvGetPlayer(hv))) riskLevel[location] -= 2;
			else riskLevel[location] += 2;
		}
		if (hasHuntersThere(hunterLocs, location)) riskLevel[location] += 3;
	}

	// Locations that are reachable by hunters
	// **Note that locations reachable by multiple hunters will have up to +12 Risk!
	for (int player = 0; player < NUM_PLAYERS - 1; player++) {
		int numReturnedLocs = -1;
		PlaceId *reachableByHunter = HvWhereCanTheyGo(hv, player, &numReturnedLocs);
		for (int i = 0; i < numReturnedLocs; i++) riskLevel[reachableByHunter[i]] += 3;
	}

	// These countries are easy for hunters to corner Dracula.
	for (int i = 0; i < NUM_REAL_PLACES; i++) {
		if (isCountry(England, i, SIZE_OF_ENGLAND)) riskLevel[i] += 1;
		if (isCountry(Spain, i, SIZE_OF_SPAIN)) riskLevel[i] += 1;
		if (isCountry(Italy, i, SIZE_OF_ITALY)) riskLevel[i] += 1;
	}

	PlaceId min = places[0];
	for (int i = 1; i < numLocs; i++) {
		if (riskLevel[places[i]] <= riskLevel[min]) min = places[i];
	}

	return min;
}

int hasHuntersThere(PlaceId hunterLocs[4], PlaceId location) {
	for (int i = 0; i < NUM_PLAYERS - 1; i++) {
		if (hunterLocs[i] == location) return TRUE;
	}
	return FALSE;
}

bool isCountry(PlaceId country[], PlaceId location, int size) {
	for (int i = 0; i < size; i++) {
		if (location == country[i]) {
			return true;
		}
	}
	return false;
}

// Return the next move in order to get to a neighbouring city of Dracula's location
PlaceId neighbourCities(HunterView hv, PlaceId DraculaLoc, Player currHunter, PlaceId hunterLocs[4]) {
	Map m = MapNew();	

	ConnList list = MapGetConnections(m, DraculaLoc);
	ConnList curr = list;
	PlaceId neighbouringCity[NUM_REAL_PLACES];
	int i = 0;
	while (curr != NULL) {
		neighbouringCity[i] = curr->p;
		i++;
		curr = curr->next;
	}

	PlaceId placeToGo = lowestRiskForDracula(hv, neighbouringCity, i, hunterLocs);

	int pathLengthToNeighbouring = -1;
	PlaceId *pathToNeighbouring = HvGetShortestPathTo(hv, currHunter, placeToGo, &pathLengthToNeighbouring);

	return pathToNeighbouring[0];
}

int isThereCDInReachable(PlaceId *places, int numLocs) {
	for (int i = 0; i < numLocs; i++) {
		if (places[i] == CASTLE_DRACULA) return TRUE;
	}
	return FALSE;
}

int isPlayMinaDr(Player currHunter) {
	return (currHunter == PLAYER_MINA_HARKER || currHunter == PLAYER_DR_SEWARD);
}