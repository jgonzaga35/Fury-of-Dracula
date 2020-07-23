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
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

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
/*
// May bug if there are more than 2 duplicates.
// Currently replaces any duplicates with the value of 'from' from
// MapGetConnections.
static void ensureNoDup(PlaceId *allowableCNC, PlaceId p, int *numReturnedLocs) {
	// for (int i = 0; i < *numReturnedLocs - 1; i++) {
	// 	for (int j = i + 1; j < *numReturnedLocs; j++) {
	// 		if (allowableCNC[i] == allowableCNC[j]) {
	// 			allowableCNC[i] = p;
	// 		}
	// 	}
	// }

}*/

// checks that p is not present in allowable CNC
// returns 1 if dup, 0 if no dup
static int Dup(PlaceId *allowableCNC, PlaceId p, int *numReturedLocs) {
	for (PlaceId i = 0; i < *numReturedLocs; i += 1) {
		if (p == allowableCNC[i]) return 1;
	}
	return 0;
}

// returns the distance in terms of edge length from one src to dest
int bfsPathDist(Map m, ConnList src, PlaceId dest) {
	assert(m != NULL);
	PlaceId *visited = malloc(MAX_REAL_PLACE * sizeof(PlaceId));
	// initalise visited array
	for (PlaceId i = 0; i < MAX_REAL_PLACE; i += 1) visited[i] = -1;

	Queue q = newQueue();
	QueueJoin(q, src->p);
	visited[src->p] = 1;

	int destFound = 0; // flag

	PlaceId i, left; // i is iterator & left is most recent item in q that left
	while (!destFound && !QueueIsEmpty(q)) {
		left = QueueLeave(q);
		for (src; src != NULL; src = src->next) { // loop through all adj nodes to curr
			if (left == dest) {destFound = 1; break;}
			visited[src->p] = left;
			QueueJoin(q, src->p);
		}
	}

	if (visited[dest] == -1) return 0; // no route found (not possible)

	// calculate distance in terms of edge length from src to dest
	int length = 1;
	PlaceId j = dest;
	while (j != src->p) {
		j = visited[j];
		length += 1;
	}

	dropQueue(q);
	free(visited);
	
	return length;
}

/** From list of connections (provided by MapGetconnections function),
 * scan through linked list, simultaneously adding
 * "type" connection to allowableCNC array.
 * Update number of unique locations added to array through numReturnedLocs */
/*
void getRoadCNC(ConnList CNC, PlaceId *allowableCNC, int *numReturnedLocs) {
	/* hey peter do you know how to implement this function?
	i am having a bit of trouble because it processes the connection type
	between Galatz and Bucharest as RAIL when there is also a ROAD connection
	between them. ConnList only stores one type so im not sure how to make it
	so that the function can detect if there are multiple types of connections
	between 2 locations. thanks - justin
	
	ConnList curr = CNC;
	printf("curr->p is %s\n", placeIdToAbbrev(curr->p));
	int j = 0;
	for (int i = 0; curr != NULL || i != MAX_REAL_PLACE; i++) {
		// There is a road from Bucharest to Galatz.
		// There is also a rail from Bucharest to Galatz.
		if (connListContains(curr, curr->p, ROAD) || curr->type == ROAD) {
			allowableCNC[i] = curr->p;
			printf("allowableCNC[%d] is %s\n", i, placeIdToAbbrev(curr->p));
			j++;
		}
		curr = curr->next;
		// For some reason if I do not include this I get a segmentation fault
		if (curr == NULL) break;
	}
	*numReturnedLocs = j;
	
}*/

void getRoadCNC(ConnList CNC, PlaceId *allowableCNC, int *numReturnedLocs) {
	if (CNC == NULL) return;
	ConnList curr = CNC;
	for (int i = *numReturnedLocs; curr != NULL && i != MAX_REAL_PLACE; i += 1) {
		// start adding road CNC from numReturnedLocs position in array
		if (curr->type == ROAD) {
			if (Dup(allowableCNC, curr->p, numReturnedLocs)) 
				continue; // do not add if already present in array
			allowableCNC[i] = curr->p;
			*numReturnedLocs += 1;
			printf("allowableCNC[%d] is %s\n", i, placeIdToAbbrev(curr->p));
		}
		curr = curr->next;
	}
	printf("value of numReturedLocs is : %d\n", *numReturnedLocs);
}

void getRailCNC(ConnList CNC, PlaceId *allowableCNC, int *numReturnedLocs, Round round, 
				Player player, Map m)
	{
	if (CNC == NULL) return;
	ConnList curr = CNC;
	int sum = (round + player) % 4; // max allowable station distances
	for (int i = *numReturnedLocs; curr != NULL || i != MAX_REAL_PLACE; i += 1) {
		// start adding rail CNC from numReturnedLocs position in array
		if (sum == 0) break; // cannot move from rail at all
		if (curr->type == RAIL) {
			int dist = bfsPathDist(m, CNC, curr->p);
			if (dist <= sum) { // add all distances less than max allowable dist
				allowableCNC[i] = curr->p; 
				(*numReturnedLocs) += 1;
			}
		}
	}
}

void getBoatCNC(ConnList CNC, PlaceId *allowableCNC, int *numReturnedLocs) {
	if (CNC == NULL) return;
	ConnList curr = CNC;
	for (int i = *numReturnedLocs; curr != NULL || i != MAX_REAL_PLACE; i += 1) {
		// start adding boat CNC from numReturnedLocs position in array
		if (curr->type == BOAT) {
			allowableCNC[i] = curr->p;
			*numReturnedLocs += 1;
		}
	}
}