//
//  MancalaBoard.h
//  mancala_competition
//
//  Created by Hunter McNichols on 11/9/15.
//  Copyright © 2015 hunter_mc. All rights reserved.
//

#ifndef MancalaBoard_h
#define MancalaBoard_h

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
//Game representation
class MancalaBoard{
    /**Represents the board of the game. Does this all through a single array and an interaction interface**/
public:
    MancalaBoard(std::vector<int>&p2_state, std::vector<int>&p1_state, int p2S, int p1S);
    ~MancalaBoard();//delete cells
    //copy constructor
    MancalaBoard(const MancalaBoard& source);
    //assignment operator
    MancalaBoard& operator=(const MancalaBoard& source);
    
    bool make_move(int player_num, int cell_num);//true if there's a bonus move
    
    int moves_available(int player_num);
    
    int util_value(int player_turn, int root_player);
    
    //returns an array of the indicies of the next valid moves for the player, BE SURE TO DELETE THIS
    int *next_moves(int player_num);
    
    void print();
    void print_to_file(std::ofstream &output);
    
    void checkGameOver(int player_turn);
private:
    int *cells;
    int p1_mancala;
    int p2_mancala;
    int p1_cell_start;
    int p2_cell_start;
    int p1_cell_end;
    int p2_cell_end;
    int num_cells;//number of cells for each player
    
    
};


#endif /* MancalaBoard_h */
