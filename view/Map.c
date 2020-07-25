////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// Map.c: an implementation of a Map type
// You can change this as much as you want!
// If you modify this, you should submit the new version.
//
// 2017-11-30	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
//#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
//#include <sysexits.h>

#include "Map.h"
#include "Places.h"
#include "Queue.h"

struct map {
	int nV; // number of vertices
	int nE; // number of edges
	ConnList connections[NUM_REAL_PLACES];
};

static void addConnections(Map m);
static void addConnection(Map m, PlaceId v, PlaceId w, TransportType type);
static inline bool isSentinelEdge(Connection c);

static ConnList connListInsert(ConnList l, PlaceId v, TransportType type);
static bool connListContains(ConnList l, PlaceId v, TransportType type);
static int Dup(PlaceId *allowableCNC, PlaceId p, int *numReturedLocs);
static int EdgeDistLen(PlaceId *visited, PlaceId src, PlaceId dest);

////////////////////////////////////////////////////////////////////////

/** Creates a new map. */
// #vertices always same as NUM_REAL_PLACES
Map MapNew(void)
{
	Map m = malloc(sizeof(*m));
	if (m == NULL) {
		fprintf(stderr, "Couldn't allocate Map!\n");
		exit(EXIT_FAILURE);
	}

	m->nV = NUM_REAL_PLACES;
	m->nE = 0;
	for (int i = 0; i < NUM_REAL_PLACES; i++) {
		m->connections[i] = NULL;
	}

	addConnections(m);
	return m;
}

/** Frees all memory allocated for the given map. */
void MapFree(Map m)
{
	assert (m != NULL);

	for (int i = 0; i < m->nV; i++) {
		ConnList curr = m->connections[i];
		while (curr != NULL) {
			ConnList next = curr->next;
			free(curr);
			curr = next;
		}
	}
	free(m);
}

////////////////////////////////////////////////////////////////////////

/** Prints a map to `stdout`. */
void MapShow(Map m)
{
	assert(m != NULL);

	printf("V = %d, E = %d\n", m->nV, m->nE);
	for (int i = 0; i < m->nV; i++) {
		for (ConnList curr = m->connections[i]; curr != NULL; curr = curr->next) {
			printf("%s connects to %s by %s\n",
			       placeIdToName((PlaceId) i),
			       placeIdToName(curr->p),
			       transportTypeToString(curr->type)
			);
		}
	}
}

////////////////////////////////////////////////////////////////////////

/** Gets the number of places in the map. */
int MapNumPlaces(Map m)
{
	assert(m != NULL);
	return m->nV;
}

/** Gets the number of connections of a particular type. */
int MapNumConnections(Map m, TransportType type)
{
	assert(m != NULL);
	assert(transportTypeIsValid(type) || type == ANY);

	int nE = 0;
	for (int i = 0; i < m->nV; i++) {
		for (ConnList curr = m->connections[i]; curr != NULL; curr = curr->next) {
			if (curr->type == type || type == ANY) {
				nE++;
			}
		}
	}

	return nE;
}

////////////////////////////////////////////////////////////////////////

/// Add edges to Graph representing map of Europe
static void addConnections(Map m)
{
	assert(m != NULL);

	for (int i = 0; !isSentinelEdge(CONNECTIONS[i]); i++) {
		addConnection(m, CONNECTIONS[i].v, CONNECTIONS[i].w, CONNECTIONS[i].t);
	}
}

/// Add a new edge to the Map/Graph
static void addConnection(Map m, PlaceId start, PlaceId end, TransportType type)
{
	assert(m != NULL);
	assert(start != end);
	assert(placeIsReal(start));
	assert(placeIsReal(end));
	assert(transportTypeIsValid(type));

	// don't add edges twice
	if (connListContains(m->connections[start], end, type)) return;

	m->connections[start] = connListInsert(m->connections[start], end, type);
	m->connections[end]   = connListInsert(m->connections[end], start, type);
	m->nE++;
}

/// Is this the magic 'sentinel' edge?
static inline bool isSentinelEdge(Connection c)
{
	return c.v == -1 && c.w == -1 && c.t == ANY;
}

/// Insert a node into an adjacency list.
static ConnList connListInsert(ConnList l, PlaceId p, TransportType type)
{
	assert(placeIsReal(p));
	assert(transportTypeIsValid(type));

	ConnList new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate ConnNode");
		exit(EXIT_FAILURE);
	}
	
	new->p = p;
	new->type = type;
	new->next = l;
	return new;
}

/// Does this adjacency list contain a particular value?
static bool connListContains(ConnList l, PlaceId p, TransportType type)
{
	assert(placeIsReal(p));
	assert(transportTypeIsValid(type));

	for (ConnList curr = l; curr != NULL; curr = curr->next) {
		if (curr->p == p && curr->type == type) {
			return true;
		}
	}
	
	return false;
}

////////////////////////////////////////////////////////////////////////

ConnList MapGetConnections(Map m, PlaceId p)
{
	assert(placeIsReal(p));
	return m->connections[p];
}

////////////////////////////////////////////////////////////////////////
void printConnList(ConnList L) {
	if (L == NULL) return;
	for (ConnList curr = L; curr != NULL; curr = curr->next) {
		printf("%d-%d-", curr->p, curr->type);
		printf("%s ",placeIdToAbbrev(curr->p));
	}
	printf("\n");
}

// checks that p is not present in allowable CNC
// returns 1 if dup, 0 if no dup
static int Dup(PlaceId *allowableCNC, PlaceId p, int *numReturedLocs) {
	for (PlaceId i = 0; i < *numReturedLocs; i += 1) {
		if (p == allowableCNC[i]) return 1;
	}
	return 0;
}

static int EdgeDistLen(PlaceId *visited, PlaceId src, PlaceId dest) {
	if (visited[dest] == -1) return 0; // No route found (not possible)
	// calculate distance in terms of edge length from src to dest
	int length = 0;
	PlaceId j = dest;
	while (j != src) {
		if (j == -1) return 0;
		j = visited[j];
		length += 1;
	}
	j = visited[j];

	return length;
}

// returns the distance in terms of edge length from src to all edges of "type"
void bfsPath(Map m, PlaceId *visited, PlaceId from, 
				bool road, bool rail, bool boat, Player p) 
{
	assert(m != NULL);
	Queue q = newQueue();
	QueueJoin(q, from);
	visited[from] = from;

	PlaceId left; // left is most recent item in q that left
	ConnList curr; // iterator

	while (!QueueIsEmpty(q)) {
		left = QueueLeave(q);
		for (ConnList curr = m->connections[left]; curr != NULL; curr = curr->next) { 
			// loop through all adj nodes to "left"
			if (left != curr->p && visited[curr->p] == -1) {
				if (p == PLAYER_DRACULA && curr->p == ST_JOSEPH_AND_ST_MARY) continue;
				// curr not visited, join q + visit
				if (road && curr->type == ROAD) 
					{visited[curr->p] = left; QueueJoin(q, curr->p);}
				if (rail && curr->type == RAIL) 
					{visited[curr->p] = left; QueueJoin(q, curr->p);}
				if (boat && curr->type == SEA) 
					{visited[curr->p] = left; QueueJoin(q, curr->p);}
			}
		}
	}

	dropQueue(q);
}

/** From list of connections (provided by MapGetconnections function),
 * scan through linked list, simultaneously adding
 * "type" connection to allowableCNC array.
 * Update number of unique locations added to array through numReturnedLocs 
 * Note :: 'i' is iterated over in the following for loops to stop 
 * buffer overflow in allowableCNC array. otherwise, it serves no other
 * purpose in the function.*/

void getRoadCNC(ConnList CNC, PlaceId *allowableCNC, int *numReturnedLocs, Player p) {
	if (CNC == NULL) return;
	ConnList curr = CNC;
	for (int i = *numReturnedLocs; curr != NULL && i != MAX_REAL_PLACE; i += 1) {
		// start adding ONLY road CNC from numReturnedLocs position in array
		if (curr->type == ROAD) {
			if (curr->p == ST_JOSEPH_AND_ST_MARY && p == PLAYER_DRACULA) {
				curr = curr->next;
				continue; // Dracula cannot visit hospital
			}
			if (Dup(allowableCNC, curr->p, numReturnedLocs)) 
				continue; // do not add if already present in array
			allowableCNC[*numReturnedLocs] = curr->p;
			*numReturnedLocs += 1;
		}
		curr = curr->next;
	}
}

void getRailCNC(ConnList CNC, PlaceId from, PlaceId *allowableCNC, int *numReturnedLocs, Round round, 
				Player player, Map m)
	{
	if (player == PLAYER_DRACULA) return; // Dracula not allowed to travel by rail
	if (CNC == NULL) return;
	ConnList curr = CNC;

	// initalise visited array
	PlaceId *visited = malloc(MAX_REAL_PLACE * sizeof(PlaceId));
	for (PlaceId i = 0; i < MAX_REAL_PLACE; i += 1) visited[i] = -1;
	bfsPath(m, visited, from, false, true, false, player); // type rail path array

	int sum = (round + player) % 4; // max allowable station distances
	if (sum == 0) return; // cannot move from rail at all
	
	for (int i = 0; i < m->nV; i++) {
		for (ConnList curr = m->connections[i]; curr != NULL; curr = curr->next) {
			if (curr->type == RAIL) {
				if (Dup(allowableCNC, curr->p, numReturnedLocs)) 
					continue; // do not add if already present in array
				int dist = EdgeDistLen(visited, from, curr->p);
				if (0 < dist && dist <= sum) { // add all distances less than max allowable dist
					allowableCNC[*numReturnedLocs] = curr->p; 
					*numReturnedLocs += 1;
				}
			}
		}
	}
	
	free(visited);
}

void getBoatCNC(ConnList CNC, PlaceId *allowableCNC, int *numReturnedLocs, Player p) {
	if (CNC == NULL) return;
	ConnList curr = CNC;
	for (int i = *numReturnedLocs; curr != NULL && i != MAX_REAL_PLACE; i += 1) {
		// start adding ONLY boat CNC from numReturnedLocs position in array
		if (curr->type == BOAT) {
			if (p == PLAYER_DRACULA && curr->p == ST_JOSEPH_AND_ST_MARY) continue;
			if (Dup(allowableCNC, curr->p, numReturnedLocs)) 
				continue; // do not add if already present in array
			allowableCNC[*numReturnedLocs] = curr->p;
			*numReturnedLocs += 1;
		}
		curr = curr->next;
	}
}

PlaceId *getConnection(Map map, PlaceId src, Player hunter, Round round, int *numReturnedLocs)
{
	// TODO: 
	PlaceId *edges = malloc(NUM_REAL_PLACES * sizeof(PlaceId));
	for (int i = 0; NUM_REAL_PLACES; i++) 
	{
		edges[i] = -1;
	}

	PlaceId dest = 0;
	Queue locationQ = newQueue();
	QueueJoin(locationQ, src);
	for (dest = 0; dest < map->nV; dest++) 
	{
		QueueJoin(locationQ, dest);

	}

	//*numReturnedLocs = create another function to get this
	return edges;
}
