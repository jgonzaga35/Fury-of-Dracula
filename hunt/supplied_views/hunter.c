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

void decideHunterMove(HunterView hv)
{
	Round round = HvGetRound(hv);
	Player name = HvGetPlayer(hv); // Which hunter?
	if (round == 0) { // FIRST ROUND
		char *location;
		// Depending on the hunter, move to a predetermined location
		// Best stratergy is to choose locations away from other hunters

		// I chose corners of the map - not sure if best choice
		// (need at least one hunter in middle)
		// Need to discuss this
		switch(name) {
			case PLAYER_LORD_GODALMING:
				location = "ED";
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
		PlaceId HunterLoc, DraculaLoc;
		Round currRound = HvGetRound(hv);
		Player currHunter = HvGetPlayer(hv);
		
		

		// ---------------Get current location of the hunter--------------------
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

		// ---------------------------Move to random loc----------------------------
		// Move to a random location, (safe option - due to timing limit)
		registerBestPlay(strdup(placeIdToAbbrev(doRandom(hv, currHunter))), "rand");
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
		DraculaLoc = HvGetLastKnownDraculaLocation(hv, &LastDracRoundSeen);

		if(DraculaLoc != NOWHERE && LastDracRoundSeen != -1) { 
			// Dracula's last real location is known
			int diff = currRound - LastDracRoundSeen; // how many rounds ago

			// Depending on how far away the hunter is from Dracula,
			// take different cases.
			if ( 0 <= diff && diff <= 5) {
				int pathLength = -1;
				PlaceId *path = HvGetShortestPathTo(hv, currHunter, 
													DraculaLoc, &pathLength);
				// NOTE:: call to above function is very expensive and should be placed
				// near the end i.e. enough time + last resort
				char *nextMove = strdup(placeIdToAbbrev(path[0]));
				registerBestPlay(nextMove, "Moving Towards Drac");
			}
			// Else Dracula was seen a pretty long time ago, no point trying to
			// use HvGetShortestPathTo.
			// Instead, do a random move
			else {
				int numReturnedLocs;
				PlaceId *canTravelTo = HvWhereCanIGo(hv, &numReturnedLocs);
				int randomCityIndex = 0;
				while (randomCityIndex == 0 || randomCityIndex >= numReturnedLocs)
				{
					randomCityIndex = rand()%numReturnedLocs;
				}
				char *nextMove = strdup(placeIdToAbbrev(canTravelTo[randomCityIndex]));
				registerBestPlay(nextMove, "Moving To Random Location");
			}
		} else if(currRound >= 6) {
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
		if(HvGetHealth(hv, PLAYER_DRACULA) <= 15) {
			// If Dracula's health is less than x, move towards Castle Dracula
			if(LastDracRoundSeen != -1) {
				int pathLength = -1;
				PlaceId *path = HvGetShortestPathTo(hv, currHunter, CASTLE_DRACULA, &pathLength);
				char *nextMove = strdup(placeIdToAbbrev(path[0]));
				registerBestPlay(nextMove,"Moving to CD");
			}

			// HOWEVER, if he is really far away, then try and kill him
		}
	}
}

PlaceId doRandom(HunterView hv, Player hunter) {
	srand(time(0));
	int numLocs = -1;
	PlaceId *places = HvWhereCanIGo(hv, &numLocs);
	if(places == NULL) return UNKNOWN_PLACE;
	int loc = rand() % numLocs;
	while (loc == 0 || loc > numLocs)
		loc = rand() % numLocs;
	return places[loc];
}