#!/bin/sh
for i in {1..10}
do
    /web/cs2521/20T2/ass/ass2/hunt/dryrun/rungame.sh hunter.c dracula.c HunterView.c HunterView.h GameView.c GameView.h DraculaView.c DraculaView.h Map.c Map.h Queue.c Queue.h utils.c utils.h Makefile > OUTPUT.txt
    score=`tail -2 OUTPUT.txt | head -1 | cut -d ' ' -f3`
    echo "The score is:: $score"
done