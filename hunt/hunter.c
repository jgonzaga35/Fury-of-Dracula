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

#include "Game.h"
#include "hunter.h"
#include "HunterView.h"

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
		switch (name) {
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
		
		registerBestPlay(location, "");
	} else { // for all other rounds

		// Add ideas for strats before making function

		/** Strats:
		 * 	- 	Use HvWhereCanTheyGoByType (remove rail) function to decide 
		 * 		where exactly Dracula is able to travel. use this info to plan
		 * 		where hunter should move (possibly in 5 city radii around that
		 * 		location)
		 * 	- 	Whenever (round + num) % 4 ==3, decide if hunters can intercept 
		 * 		Dracula by moving via rail (Dracula's location decided by 
		 * 		HvGetLastKnownDraculaLocation)
		 * */
	}



	// TODO: Replace this with something better!
	registerBestPlay("TO", "Have we nothing Toulouse?");
}
