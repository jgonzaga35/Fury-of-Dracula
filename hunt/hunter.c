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
#include "HunterView.h"

#define TRUE				1
#define FALSE				0
#define NUM_LOCS_NEAR_CD 	12

PlaceId doRandom(HunterView hv, Player hunter, PlaceId *places, int numLocs);
PlaceId moveComplement(HunterView hv, Player currHunter);
int isHunterAtLoc(HunterView hv, Player hunter, PlaceId loc);
static void getHunterLocs(HunterView hv, PlaceId hunterLocs[]);
static int isKnown(PlaceId location);
static void closestToVampire(HunterView hv, Player currHunter, int *locRank);

void decideHunterMove(HunterView hv) {
	Round round = HvGetRound(hv);
	Player name = HvGetPlayer(hv); // Which hunter?
	
	if (round == 0) { // FIRST ROUND
		char *location = NULL;
		switch(name) {
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
		Player currHunter = HvGetPlayer(hv);
		PlaceId hunterLocs[4]; 
		getHunterLocs(hv, hunterLocs); 
		
		PlaceId HunterLoc = HvGetPlayerLocation(hv, currHunter);
		int locRank[NUM_REAL_PLACES] = {0};		// REVIEW: Array containing the weighting of each places

		// if(currHunter != PLAYER_DR_SEWARD) {
		// 	printf("hunterLoc is %d %s %s\n", HunterLoc, placeIdToAbbrev(HunterLoc), placeIdToName(HunterLoc));

		int numLocs = -1;
		PlaceId *places = HvWhereCanIGo(hv, &numLocs);

		// ------------------Move to random loc (Safe with timing)------------------
		registerBestPlay(strdup(placeIdToAbbrev(doRandom(hv, currHunter, places, numLocs))), "general random");

		// ------------------If hunter health low, rest-----------------------------
		int currHunterHealth = HvGetHealth(hv, currHunter);
		if(currHunterHealth <= 3) {
			char *nextMove = strdup(placeIdToAbbrev(HunterLoc));
			locRank[HunterLoc] += 2;	// REVIEW: At low health: the currLocation +2 Weight
			registerBestPlay(nextMove, "Resting");
		}
		
		// -----------------Get last known Dracula location-------------------------
		/** Gets  Dracula's  last  known  real  location  as revealed in the 
			 * play string and sets *round to the number of the  latest  round  in  
			 * which Dracula moved there.*/
		Round LastDracRoundSeen = -1;
		PlaceId DraculaLoc = HvGetLastKnownDraculaLocation(hv, &LastDracRoundSeen);

		if(DraculaLoc != NOWHERE && LastDracRoundSeen != -1) { 
			// Dracula's last real location is known
			int diff = HvGetRound(hv) - LastDracRoundSeen; // how many rounds ago

			// Depending on how far away the hunter is from Dracula,
			// take different cases.
			if ( 0 <= diff && diff <= 5) {
				int pathLength = -1;
				PlaceId *path = HvGetShortestPathTo(hv, currHunter, 
													DraculaLoc, &pathLength);
				// NOTE: call to above function is very expensive and should be placed
				// near the end i.e. enough time + last resort
				printf("The value of curr round: %d\n", HvGetRound(hv));
				printf("The value of Dracula Loc : %d %s %s\n", DraculaLoc, placeIdToAbbrev(DraculaLoc), placeIdToName(DraculaLoc));
				printf("The value of LastDracSeen : %d\n", LastDracRoundSeen);
				printf("The value of pathLen : %d\n", pathLength);
				for(int i = 0; i < pathLength; i ++) {
					printf("/////////////////////////////////////////////////////////////////////////////\n");
					printf("%d %s %s\n", path[i], placeIdToAbbrev(path[i]), placeIdToName(path[i]));
				}

				if(pathLength == 0) { // already at pos
					registerBestPlay(strdup(placeIdToAbbrev(doRandom(hv, currHunter, path, numLocs))), "already at \"dracloc\"");
					if (diff == 1) {
						registerBestPlay(strdup(placeIdToAbbrev(HunterLoc)), "already at \"dracloc\"");
						locRank[HunterLoc] += 300;	// REVIEW:Stay in the position if dracula move in to the same city, as move happen before encounter
						printf("Dracula moved into hunter place, stay in the same city\n");
					}
				} else {
					char *nextMove = strdup(placeIdToAbbrev(path[0]));
					locRank[path[0]] += 100;		// REVIEW: If dracula's location is known and within 5 round goes there
					registerBestPlay(nextMove, "Moving Towards Drac");
				}
			} else {
				// Else Dracula was seen a pretty long time ago, no point trying to
				// use HvGetShortestPathTo. Instead, move towards opposite side of map
				int pathLength = -1;
				PlaceId *path = HvGetShortestPathTo(hv, currHunter, moveComplement(hv, currHunter),&pathLength);
				if(pathLength == 0) { // already at pos
					registerBestPlay(strdup(placeIdToAbbrev(doRandom(hv, currHunter, path, numLocs))), "already at \"dracloc\"");
				} else if(HvGetRound(hv) % 5 <= 2){ // so doesn't get stuck in loop
					registerBestPlay(strdup(placeIdToAbbrev(path[0])), "move complement");
				} else {
					registerBestPlay(strdup(placeIdToAbbrev(doRandom(hv, currHunter, places, numLocs))), "dracula trail random");
				}
			}
		// 		// Note:: If the move was a HIDE/DOUBLE_BACK move, then the move that
		// 		// the HIDE/DOUBLE_BACK refers to will be revealed (and so on
		// 		// until LOCATION is revealed)
		// 		// Therefore, it might not exactly be the 6th last move
		// 		char *nextMove = strdup(placeIdToAbbrev(HunterLoc));
		// 		registerBestPlay(nextMove, "Research"); // sends currLocofHunter back
		// 	}
		}

		if (currHunter == PLAYER_DR_SEWARD) {
			// Dr. Sewards Job is to stay around CD
			char *LocsNearCD[] = 
			{
				"JM", "SZ", "KL", "CD", "SJ", "BE", "BC", "GA", "SO", "VR", "CN", "BS"
			};

			srand(time(0));
			int pathL= 0;
			int randInd = rand() % NUM_LOCS_NEAR_CD;
			PlaceId *path;
			while(pathL == 0) {
				path = HvGetShortestPathTo(hv, currHunter, placeAbbrevToId(LocsNearCD[randInd]), &pathL);
				randInd = rand() % NUM_LOCS_NEAR_CD;
			}
			registerBestPlay(strdup(placeIdToAbbrev(path[0])), "ROUND");
			return;
		}

			
		// ------------------------If Dracula health <= x---------------------------
		if(HvGetHealth(hv, PLAYER_DRACULA) <= 20) {
			printf("HELLO\n");
			// If Dracula's health is less than x, move towards Castle Dracula
			int pathLength = -1;
			PlaceId *path = HvGetShortestPathTo(hv, currHunter, CASTLE_DRACULA, &pathLength);
			locRank[path[0]] += 2;		// REVIEW: If life of dracula low, go towards dracula
			char *nextMove = strdup(placeIdToAbbrev(path[0]));
			if(pathLength == 0) {registerBestPlay(strdup(placeIdToAbbrev(HunterLoc)), "stay");}
			else registerBestPlay(nextMove,"Moving to CD");
		}

		// ---------------shouldn't go to where other hunters are already at--------------------
		for (int i = 0; i < numLocs; i++) {
			for (int player = 0; player < 4; player++) {
				if (places[i] == hunterLocs[player])	locRank[places[i]] -= 1;
			}
		}

		// ----------Go to the vampire's location if it's known and the current player is the closest to vampire--------
		closestToVampire(hv, currHunter, &locRank[0]);
	}
}

// Return a random valid move
PlaceId doRandom(HunterView hv, Player hunter, PlaceId *places, int numLocs) {
	srand(time(0));
	PlaceId currLoc = HvGetPlayerLocation(hv, hunter);
	
	for (int i = 0; i < numLocs; i++) {
		printf("places[%d] is %s\n", i, placeIdToName(places[i]));
	}
	if(places == NULL) return currLoc;
	int loc = rand() % numLocs;
	while (places[loc] ==  currLoc)
		loc = rand() % numLocs;
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
