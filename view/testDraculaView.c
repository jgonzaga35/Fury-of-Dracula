////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// testDraculaView.c: test the DraculaView ADT
//
// As supplied, these are very simple tests.  You should write more!
// Don't forget to be rigorous and thorough while writing tests.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-02	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DraculaView.h"
#include "Game.h"
#include "Places.h"
#include "testUtils.h"

int main(void)
{
	{///////////////////////////////////////////////////////////////////
	
		printf("Test for basic functions, "
			   "just before Dracula's first move\n");

		char *trail =
			"GST.... SAO.... HZU.... MBB....";
		
		Message messages[] = {
			"Hello", "Goodbye", "Stuff", "..."
		};
		
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 0);
		assert(DvGetScore(dv) == GAME_START_SCORE);
		assert(DvGetHealth(dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert(DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(DvGetPlayerLocation(dv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert(DvGetPlayerLocation(dv, PLAYER_VAN_HELSING) == ZURICH);
		assert(DvGetPlayerLocation(dv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(DvGetPlayerLocation(dv, PLAYER_DRACULA) == NOWHERE);
		assert(DvGetVampireLocation(dv) == NOWHERE);
		int numTraps = -1;
		PlaceId *traps = DvGetTrapLocations(dv, &numTraps);
		assert(numTraps == 0);
		free(traps);

		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for encountering Dracula\n");

		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.V.. "
			"GGEVD.. SAO.... HCD.... MAO....";
		
		Message messages[] = {
			"Hello", "Goodbye", "Stuff", "...", "Mwahahah",
			"Aha!", "", "", ""
		};
		
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 1);
		assert(DvGetScore(dv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN);
		assert(DvGetHealth(dv, PLAYER_LORD_GODALMING) == 5);
		assert(DvGetHealth(dv, PLAYER_DRACULA) == 30);
		assert(DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING) == GENEVA);
		assert(DvGetPlayerLocation(dv, PLAYER_DRACULA) == GENEVA);
		assert(DvGetVampireLocation(dv) == NOWHERE);

		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for Dracula leaving minions 1\n");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DED.V.. "
			"GST.... SST.... HST.... MST.... DMNT... "
			"GST.... SST.... HST.... MST.... DLOT... "
			"GST.... SST.... HST.... MST.... DHIT... "
			"GST.... SST.... HST.... MST....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 4);
		assert(DvGetVampireLocation(dv) == EDINBURGH);
		int numTraps = -1;
		PlaceId *traps = DvGetTrapLocations(dv, &numTraps);
		assert(numTraps == 3);
		sortPlaces(traps, numTraps);
		assert(traps[0] == LONDON);
		assert(traps[1] == LONDON);
		assert(traps[2] == MANCHESTER);
		free(traps);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for Dracula's valid moves 1\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DCD.V.. "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[9] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
		assert(numMoves == 4);
		sortPlaces(moves, numMoves);
		assert(moves[0] == GALATZ);
		assert(moves[1] == KLAUSENBURG);
		assert(moves[2] == HIDE);
		assert(moves[3] == DOUBLE_BACK_1);
		free(moves);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for DvGetValidMoves/DvWhereCanIGo/DvWhereCanIGoByType no move made yet\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);
		assert(numLocs == 0);
		free(locs);

		numLocs = -1;
		locs =DvGetValidMoves(dv, &numLocs);
		assert(numLocs == 0);
		free(locs);

		numLocs = -1;
		locs = DvWhereCanIGoByType(dv, true, false, &numLocs);
		assert(numLocs == 0);
		free(locs);

		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for DvWhereCanIGoByType only boat\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DBI.V.. "
			"GGE.... SGE.... HGE.... MGE.... DNPT... "
			"GGE.... SGE.... HGE.... MGE.... DROT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		bool road = false;
		bool boat = true;
		PlaceId *locs = DvWhereCanIGoByType(dv, road, boat, &numLocs);
		assert(numLocs == 1);
		sortPlaces(locs, numLocs);
		assert(locs[0] == TYRRHENIAN_SEA);
		free(locs);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for DvWhereCanIGoByType only road\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DBI.V.. "
			"GGE.... SGE.... HGE.... MGE.... DNPT... "
			"GGE.... SGE.... HGE.... MGE.... DROT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		bool road = true;
		bool boat = false;
		PlaceId *locs = DvWhereCanIGoByType(dv, road, boat, &numLocs);
		assert(numLocs == 1);
		sortPlaces(locs, numLocs);
		assert(locs[0] == FLORENCE);
		free(locs);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for DvWhereCanIGoByType both road and boat\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DBI.V.. "
			"GGE.... SGE.... HGE.... MGE.... DNPT... "
			"GGE.... SGE.... HGE.... MGE.... DROT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		bool road = true;
		bool boat = true;
		PlaceId *locs = DvWhereCanIGoByType(dv, road, boat, &numLocs);
		assert(numLocs == 2);
		sortPlaces(locs, numLocs);
		assert(locs[0] == FLORENCE);
		assert(locs[1] == TYRRHENIAN_SEA);
		free(locs);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for DvWhereCanTheyGo: Lord Godalming from GENEVA\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DBI.V.. "
			"GGE.... SGE.... HGE.... MGE.... DNPT... "
			"GGE.... SGE.... HGE.... MGE.... DROT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanTheyGo(dv, PLAYER_LORD_GODALMING, &numLocs);
		assert(numLocs == 6);
		sortPlaces(locs, numLocs);
		assert(locs[0] == CLERMONT_FERRAND);
		assert(locs[1] == GENEVA);
		assert(locs[2] == MARSEILLES);
		assert(locs[3] == PARIS);
		assert(locs[4] == STRASBOURG);
		assert(locs[5] == ZURICH);
		free(locs);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for DvWhereCanTheyGoByType: Lord Godalming from GENEVA : Boat\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DBI.V.. "
			"GGE.... SGE.... HGE.... MGE.... DNPT... "
			"GGE.... SGE.... HGE.... MGE.... DROT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		bool road = false;
		bool rail = true;
		bool boat = false;
		PlaceId *locs = DvWhereCanTheyGoByType(dv, PLAYER_LORD_GODALMING, road, rail, boat, &numLocs);
		// There is no rail connection from GENEVA to GENEVA, so should I include it? not sure...
		assert(numLocs == 1);
		assert(locs[0] == GENEVA);
		free(locs);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	return EXIT_SUCCESS;
}
