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

PlaceId doRandom(HunterView hv, Player hunter);
PlaceId moveComplement(HunterView hv, Player currHunter);

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
	} else {
		// for all other rounds
		Player currHunter = HvGetPlayer(hv);
		
		// ---------------Get current location of the hunter--------------------
		PlaceId HunterLoc = 0;
		switch(currHunter) {
			case PLAYER_LORD_GODALMING:
				HunterLoc = HvGetPlayerLocation(hv, currHunter);
				break;
			case PLAYER_DR_SEWARD:
				HunterLoc = HvGetPlayerLocation(hv, currHunter);
				break;
			case PLAYER_VAN_HELSING:
				HunterLoc = HvGetPlayerLocation(hv, currHunter);
				break;
			case PLAYER_MINA_HARKER:
				HunterLoc = HvGetPlayerLocation(hv, currHunter);
				break;
			default:
				break;
		}

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
					char *nextMove = strdup(placeIdToAbbrev(path[0]));
					registerBestPlay(nextMove, "Moving Towards Drac");
				}
			} else {
				// Else Dracula was seen a pretty long time ago, no point trying to
				// use HvGetShortestPathTo. Instead, move towards opposite side of map
				int pathLength = -1;
				PlaceId *path = HvGetShortestPathTo(hv, currHunter, moveComplement(hv, currHunter),&pathLength);
				if(pathLength == 0) { // already at pos
					registerBestPlay(strdup(placeIdToAbbrev(doRandom(hv, currHunter))), "already at \"dracloc\"");
				} else if(HvGetRound(hv) % 5 <= 2){ // so doesn't get stuck in loop
					registerBestPlay(strdup(placeIdToAbbrev(path[0])), "move complement");
				} else {
					registerBestPlay(strdup(placeIdToAbbrev(doRandom(hv, currHunter))), "dracula trail random");
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
		
		// // ------------------------If Dracula health <= x---------------------------
		// if(HvGetHealth(hv, PLAYER_DRACULA) <= 15) {
		// 	printf("HELLO\n");
		// 	// If Dracula's health is less than x, move towards Castle Dracula
		// 	if(LastDracRoundSeen != -1) {
		// 		int pathLength = -1;
		// 		PlaceId *path = HvGetShortestPathTo(hv, currHunter, CASTLE_DRACULA, &pathLength);
		// 		char *nextMove = strdup(placeIdToAbbrev(path[0]));
		// 		registerBestPlay(nextMove,"Moving to CD");
		// 	}

		// 	// HOWEVER, if he is really far away, then try and kill him
		// }
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