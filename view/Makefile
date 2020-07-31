########################################################################
# COMP2521 20T2 // the Fury of Dracula // the View
# view/Makefile: build tests for GameView/HunterView/DraculaView
#
# You can modify this if you add additional files
#
# 2018-12-31	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
# 2020-07-10    v2.0    Team Dracula <cs2521@cse.unsw.edu.au>
#
########################################################################

CC = gcc
CCFLAGS = -Wall -Werror -g
BINS = testGameView testHunterView testDraculaView testMap

all: $(BINS)

testGameView: testGameView.o testUtils.o GameView.o Map.o Places.o Queue.o
testGameView.o: testGameView.c GameView.h Map.h Places.h Game.h Queue.h Item.h

testHunterView: testHunterView.o testUtils.o HunterView.o GameView.o Map.o Places.o Queue.o
testHunterView.o: testHunterView.c HunterView.h GameView.h Map.h Places.h Game.h Queue.h

testDraculaView: testDraculaView.o testUtils.o DraculaView.o GameView.o Map.o Places.o Queue.o
testDraculaView.o: testDraculaView.c DraculaView.h GameView.h Map.h Places.h Game.h Queue.h

testMap: testMap.o Map.o Places.o Queue.o
testMap.o: testMap.c Map.h Places.h Queue.c

Places.o: Places.c Places.h Game.h
Map.o: Map.c Map.h Places.h Game.h Queue.h
Item.o : Item.h Map.h
Queue.o: Queue.c Queue.h Item.h Map.h
GameView.o:	GameView.c GameView.h Game.h
HunterView.o: HunterView.c HunterView.h Game.h
DraculaView.o: DraculaView.c DraculaView.h Game.h
testUtils.o: testUtils.c Places.h Game.h

.PHONY: clean
clean:
	-rm -f ${BINS} *.o core
