////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... The Fury of Dracula
// Map.h: an interface to a Map data type
// You can change this as much as you want!
// If you modify this, you should submit the new version.
//
// 2017-11-30   v1.0    Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31   v2.0    Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10   v3.0    Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdlib.h>

#include "Places.h"
#include "Game.h"
#include "Queue.h"

#ifndef FOD__MAP_H_
#define FOD__MAP_H_

typedef struct connNode *ConnList;
struct connNode {
	PlaceId p; // ALICANTE, etc.
	TransportType type; // ROAD, RAIL, BOAT
	ConnList next; // link to next node
};

// Map representation is hidden
typedef struct map *Map;

/** Creates a new map. */
Map MapNew(void);

/** Frees all memory allocated for the given map. */
void MapFree(Map m);

/** Prints a map to `stdout`. */
void MapShow(Map m);

/** Gets the number of places in the map. */
int MapNumPlaces(Map m);

/** Gets the number of connections of a particular type. */
int MapNumConnections(Map m, TransportType type);

/**
 *  Gets a list of connections from the given place.
 *  The returned list should NOT be modified or freed.
 */
ConnList MapGetConnections(Map m, PlaceId p);


///////////////////////////////////////////////////////////////////////////////////
// prints all nodes in connection list
void printConnList(ConnList L);

// returns path array from "from" city to all other cities
// through edges of "type"
// Player argument to satisfy conditions that Dracula
// cannot move to the hospital
// If no condition needed for argument Player enter -1
void bfsPath(Map m, PlaceId *visited, PlaceId from, bool road, bool rail, bool boat, Player p);

/** From list of connections (provided by MapGetconnections function),
 * scan through linked list, simultaneously adding
 * "type" connection to allowableCNC array.
 * Update number of unique locations added to array through numReturnedLocs */
void getRoadCNC(ConnList CNC, PlaceId *allowableCNC, int *numReturnedLocs, Player p);
void getRailCNC(ConnList CNC, PlaceId from, PlaceId *allowableCNC, int *numReturnedLocs, Round round, Player player, Map m);
void getBoatCNC(ConnList CNC, PlaceId *allowableCNC, int *numReturnedLocs, Player p);

// returns array of neighbouring PlaceIds
PlaceId *getConnection(Map map, PlaceId src, int *numReturnedLocs);

#endif // !defined(FOD__MAP_H_)
