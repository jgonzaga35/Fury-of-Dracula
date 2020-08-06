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

#define NUM_LOCS_NEAR_CD 12

PlaceId doRandom(HunterView hv, Player hunter);
PlaceId moveComplement(HunterView hv, Player currHunter);
int isHunterAtLoc(HunterView hv, Player hunter, PlaceId loc);

void decideHunterMove(HunterView hv)
{
	Round round = HvGetRound(hv);
	Player name = HvGetPlayer(hv); // Which hunter?
	if (round == 0) { // FIRST ROUND
		char *location;
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
	} else if(round == 1) { // do research in first round
		char *nextMove = strdup(placeIdToAbbrev(HvGetPlayerLocation(hv, HvGetPlayer(hv))));
		registerBestPlay(nextMove, "Resting");
		return;
	} else {
		// for all other rounds
		Player currHunter = HvGetPlayer(hv);
		
		// ---------------Get current location of the hunter--------------------
		PlaceId HunterLoc = HvGetPlayerLocation(hv, currHunter);

		if(currHunter != PLAYER_DR_SEWARD) {
			printf("hunterLoc is %d %s %s\n", HunterLoc, placeIdToAbbrev(HunterLoc), placeIdToName(HunterLoc));

			// ---------------------------Move to random loc----------------------------
			// Move to a random location, (safe option - due to timing limit)
			registerBestPlay(strdup(placeIdToAbbrev(doRandom(hv, currHunter))), "general random");

			// ------------------If hunter health low, rest-----------------------------
			int currHunterHealth = HvGetHealth(hv, currHunter);
			if(currHunterHealth <= 3) {
				char *nextMove = strdup(placeIdToAbbrev(HunterLoc));
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
					// NOTE:: call to above function is very expensive and should be placed
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
						registerBestPlay(strdup(placeIdToAbbrev(doRandom(hv, currHunter))), "already at \"dracloc\"");
					} else {
						for(Player i = PLAYER_LORD_GODALMING; i <= PLAYER_MINA_HARKER; i++) {
							// if there's already a hunter at one of the locations, random move
							if(i != currHunter && isHunterAtLoc(hv, i, path[0])) {
								break;
							} else if(i == currHunter) {
								char *nextMove = strdup(placeIdToAbbrev(path[0]));
								registerBestPlay(nextMove, "Moving Towards Drac");
								return;
							}
						}
					}
				} else {
					// Else Dracula was seen a pretty long time ago, no point trying to
					// use HvGetShortestPathTo. Instead, move towards CD
					int pathLength = -1;
					PlaceId *path = HvGetShortestPathTo(hv, currHunter, CASTLE_DRACULA,&pathLength);
					if(pathLength == 0) { // already at pos
						registerBestPlay(strdup(placeIdToAbbrev(doRandom(hv, currHunter))), "already at \"dracloc\"");
					} else {
						registerBestPlay(strdup(placeIdToAbbrev(path[0])), "move too CD");
						return;
					}
				}
			} else if(HvGetRound(hv) >= 6) {
				// If Dracula's location not known, perform collab research
				// This allows us to know the 6th move in Dracula's trail immediately
				// NOTE:: Research can only occur after 6th round as there is no
				//		  move in the 5th position of Dracula's trail before round 6

				// Note:: If the move was a HIDE/DOUBLE_BACK move, then the move that
				// the HIDE/DOUBLE_BACK refers to will be revealed (and so on
				// until LOCATION is revealed)
				// Therefore, it might not exactly be the 6th last move
				char *nextMove = strdup(placeIdToAbbrev(HunterLoc));
				registerBestPlay(nextMove, "Research"); // sends currLocofHunter back
			}
			
			// ------------------------If Dracula health <= x---------------------------
			if(HvGetHealth(hv, PLAYER_DRACULA) <= 20) {
				printf("HELLO\n");
				// If Dracula's health is less than x, move towards Castle Dracula
				int pathLength = -1;
				PlaceId *path = HvGetShortestPathTo(hv, currHunter, CASTLE_DRACULA, &pathLength);
				char *nextMove = strdup(placeIdToAbbrev(path[0]));
				if(pathLength == 0) {registerBestPlay(strdup(placeIdToAbbrev(HunterLoc)), "stay");}
				else registerBestPlay(nextMove,"Moving to CD");
			}
		} else {
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

		
	}
}

PlaceId doRandom(HunterView hv, Player hunter) {
	srand(time(0));
	PlaceId currLoc = HvGetPlayerLocation(hv, hunter);
	int numLocs = -1;
	PlaceId *places = HvWhereCanIGo(hv, &numLocs);

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

int isHunterAtLoc(HunterView hv, Player hunter, PlaceId loc) {
	if(HvGetPlayerLocation(hv, hunter) == loc) return 0;
	return 1;
}