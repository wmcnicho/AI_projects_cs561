OBJS = agent.o TreeItem.o GameTree.o MancalaBoard.o
CC = g++ -std=c++11
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

agent: $(OBJS) calibration_program.cpp
	$(CC) $(OBJS) -o agent
	$(CC) calibration_program.cpp -o calibration_program

run: agent
	./agent

calibrate: agent
	./calibration_program

newgame: 
	echo no data files

agent.o: agent.cpp TreeItem.h GameTree.h MancalaBoard.h
	$(CC) -c agent.cpp

TreeItem.o: TreeItem.h TreeItem.cpp
	$(CC) -c TreeItem.cpp

GameTree.o: GameTree.h GameTree.cpp TreeItem.h
	$(CC) -c GameTree.cpp	

MancalaBoard.o: MancalaBoard.h MancalaBoard.cpp
	$(CC) -c MancalaBoard.cpp

clean:
	\rm *.o agent calibration_program
