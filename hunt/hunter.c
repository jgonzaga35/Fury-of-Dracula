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
int isHunterAtLoc(HunterView hv, Player hunter, PlaceId loc);
static void getHunterLocs(HunterView hv, PlaceId hunterLocs[]);
static int isKnown(PlaceId location);
static void closestToVampire(HunterView hv, Player currHunter, int *locRank);
PlaceId lowestRiskForDracula(HunterView hv, PlaceId *places, int numLocs, PlaceId hunterLocs[4]);
int hasHuntersThere(PlaceId hunterLocs[4], PlaceId location);
bool isCountry(PlaceId country[], PlaceId location, int size);

void decideHunterMove(HunterView hv) {
	Round round = HvGetRound(hv);
	Player currHunter = HvGetPlayer(hv); // Which hunter?
	
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
				location = "RO";
				break;
			case PLAYER_MINA_HARKER:
				location = "GR";
				break;
			default:
				break;
		}
		
		registerBestPlay(location, "Move To The Corners of Europe");
		return;
	} else {
		// for all other rounds

		// ---------------Initialize some variables to be used--------------------
		PlaceId hunterLocs[4]; 
		getHunterLocs(hv, hunterLocs); 
		PlaceId currLoc = hunterLocs[currHunter];

		int locRank[NUM_REAL_PLACES] = {0};

		//int maxByRail = (round + currHunter) % 4;

		// if(currHunter != PLAYER_DR_SEWARD) {
		// 	printf("hunterLoc is %d %s %s\n", currLoc, placeIdToAbbrev(currLoc), placeIdToName(currLoc));

		int numLocs = -1;
		PlaceId *places = HvWhereCanIGo(hv, &numLocs);

		// ------------------Move to random loc (Safe with timing)------------------
		registerBestPlay(strdup(placeIdToAbbrev(doRandom(hv, currHunter, places, numLocs))), "general random");


		
		// -----------------Get last known Dracula location-------------------------
		/** Gets  Dracula's  last  known  real  location  as revealed in the 
			 * play string and sets *round to the number of the  latest  round  in  
			 * which Dracula moved there.*/
		Round LastDracRoundSeen = -1;
		PlaceId DraculaLoc = HvGetLastKnownDraculaLocation(hv, &LastDracRoundSeen);

		int draculaFound = FALSE;
		// If we know where dracula is
		if(DraculaLoc != NOWHERE) { 	//  && LastDracRoundSeen != -1
			int diff = HvGetRound(hv) - LastDracRoundSeen; // how many rounds ago

			// If Dracula is there in the past 10 rounds
			if (1 <= diff && diff <= 8) {
				int pathLength = -1;
				draculaFound = TRUE;
				PlaceId *path = HvGetShortestPathTo(hv, currHunter, DraculaLoc, &pathLength);

				// printf("The value of curr round: %d\n", HvGetRound(hv));
				// printf("The value of Dracula Loc : %d %s %s\n", DraculaLoc, placeIdToAbbrev(DraculaLoc), placeIdToName(DraculaLoc));
				// printf("The value of LastDracSeen : %d\n", LastDracRoundSeen);
				// printf("The value of pathLen : %d\n", pathLength);
				// for(int i = 0; i < pathLength; i ++) {
				// 	printf("/////////////////////////////////////////////////////////////////////////////\n");
				// 	printf("%d %s %s\n", path[i], placeIdToAbbrev(path[i]), placeIdToName(path[i]));
				// }

				// If we are with Dracula this round / we can arrive at where Dracula is right now in a move, definitely stay / move to encounter
				if ((pathLength == 0 || pathLength == 1) && diff == 1) {
					registerBestPlay(strdup(placeIdToAbbrev(DraculaLoc)), "Encounter Dracula");
					printf("Dracula moved into hunter place, stay in the same city\n");
				}

				// If we can arrive where Dracula is right now in two move
				if (pathLength == 2 && diff == 1) {
					// TODO: If we can also move to the neighbouring of there in two move, move to the neighbouring.
					// If not, just go to where dracula is right now
					registerBestPlay(strdup(placeIdToAbbrev(path[0])), "Moving Towards Drac");
				}

				Map m = MapNew();	

				// if (pathLength > 2 && diff == 1) {
					// If it's too far, we go to the neighbouring of Dracula's location. Each hunter go to a different one, communicate via message
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
					registerBestPlay(strdup(placeIdToAbbrev(pathToNeighbouring[0])), "Moving Towards Drac's neighbouring");
				// }

				// if (diff >= 2 && diff <= 3) {
				// 	// TODO: go to the neighbouring cities of where dracula is
				// }

				// if (diff >= 4) {
				// 	// TODO: go to the second/ third level childs of where dracula is
				// }

			// If Dracula isn't there recently, do research
			} else if (HvGetRound(hv) >= 6 && HvGetRound(hv) % 4 == 0) {
				registerBestPlay(strdup(placeIdToAbbrev(currLoc)), "Research");
				locRank[currLoc] += 10;
			}
		} 

		// ------------------If hunter health low, rest-----------------------------
		int currHunterHealth = HvGetHealth(hv, currHunter);
		if (currHunterHealth <= 3) {
			locRank[currLoc] += 2;
		}

		// ------------------------If Dracula health <= x---------------------------
		if(HvGetHealth(hv, PLAYER_DRACULA) <= 20) {
			int pathLength = -1;
			PlaceId *path = HvGetShortestPathTo(hv, currHunter, CASTLE_DRACULA, &pathLength);
			locRank[path[0]] += 2;
		}

		// ---------------shouldn't go to where other hunters are already at--------------------
		for (int i = 0; i < numLocs; i++) {
			for (int player = 0; player < 4; player++) {
				if (places[i] == hunterLocs[player]) locRank[places[i]] -= 1;
			}
		}

		// ----------Go to the vampire's location if it's known and the current player is the closest to vampire--------
		closestToVampire(hv, currHunter, locRank);

		// ----------- Don't go to the same location ----------
		int numReturnedMoves;
		bool canFree;
		PlaceId *locationHistory = HvGetLocationHistory(hv, currHunter, &numReturnedMoves, &canFree);

		for (int i = 0; i < numReturnedMoves; i++) {
			for (int j = 0; j < numLocs; j++) {
				if (places[j] == locationHistory[i]) locRank[places[j]] -= 10;
			}
		}

		// ----------Go to the locaion with the highest rank---------
		if (draculaFound == FALSE) {
			PlaceId max = places[0];
			for (int i = 0; i < numLocs; i++) {
				if (locRank[places[i]] > locRank[max]) max = places[i];
			}
			registerBestPlay(strdup(placeIdToAbbrev(max)), "LOL");
		}
		return;
	}
}

// Return a random neigbouring city
PlaceId doRandom(HunterView hv, Player hunter, PlaceId *places, int numLocs) {
	srand(time(0));
	PlaceId currLoc = HvGetPlayerLocation(hv, hunter);
	
	// for (int i = 0; i < numLocs; i++) {
	// 	printf("places[%d] is %s\n", i, placeIdToName(places[i]));
	// }
	
	if(places == NULL) return currLoc;
	
	int loc = rand() % numLocs;
	while (places[loc] ==  currLoc) {
		loc = rand() % numLocs;
	}

	return places[loc];
}

PlaceId moveComplement(HunterView hv, Player currHunter) {
	printf("/////////////////////////////////////////////////////////////////////////\n");
	printf("value ot Player loc: %d\n", HvGetPlayerLocation(hv, currHunter));
	printf("MAX - Loc = %d %s %s ", MAX_REAL_PLACE - HvGetPlayerLocation(hv, currHunter), placeIdToAbbrev( MAX_REAL_PLACE - HvGetPlayerLocation(hv, currHunter)), placeIdToName( MAX_REAL_PLACE - HvGetPlayerLocation(hv, currHunter)));
	printf("/////////////////////////////////////////////////////////////////////////\n");
	return MAX_REAL_PLACE - HvGetPlayerLocation(hv, currHunter);
}

// Get the location of the other hunters
static void getHunterLocs(HunterView hv, PlaceId hunterLocs[]) {
	for (int player = 0; player < 4; player++) {
		hunterLocs[player] = HvGetPlayerLocation(hv, player);
	}
}

static void closestToVampire(HunterView hv, Player currHunter, int *locRank) {
	PlaceId vampireLoc = HvGetVampireLocation(hv);
	if (!isKnown(vampireLoc)) return;

	int pathLength[NUM_PLAYERS];
	PlaceId *paths[NUM_PLAYERS];
	for (int player = 0; player < 4; player++) {
		paths[player] = HvGetShortestPathTo(hv, player, vampireLoc, &pathLength[player]);
	}

	Player closest = PLAYER_LORD_GODALMING;
	for (int player = 0; player < 4; player++) {
		if (pathLength[player] < pathLength[closest]) closest = player;
	}

	if (closest == currHunter) locRank[paths[currHunter][0]] += 3;	// REVIEW: If hunter is closest to vampire, increase the rank

	return;
}

static int isKnown(PlaceId location) {
	return (location != CITY_UNKNOWN && location != NOWHERE && location != SEA_UNKNOWN);
}

int isHunterAtLoc(HunterView hv, Player hunter, PlaceId loc) {
	if(HvGetPlayerLocation(hv, hunter) == loc) return 0;
	return 1;
}

// Compute the least risky location Dracula will go
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
		if (placeIdToType(location) == SEA) riskLevel[location] += 2;
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